#include "cashier.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

// get current timestamp: monotonic increasing positive integer value
uint64_t get_timestamp(void) {
  // Implementation depends on the platform
  // Return a monotonic increasing positive integer value
  // Example implementation:
  static uint64_t timestamp = 0;
  return ++timestamp;
}

// read a byte from the data memory
uint8_t mem_read(uint64_t addr) {
  // Implementation depends on the platform
  // Read a byte from the data memory at the given address
  // Return the value read
  // Example implementation:
  uint8_t *data_memory = (uint8_t *)addr;
  return *data_memory;
}

// write a byte into the data memory
void mem_write(uint64_t addr, uint8_t byte) {
  // Implementation depends on the platform
  // Write the byte into the data memory at the given address
  // Example implementation:
  uint8_t *data_memory = (uint8_t *)addr;
  *data_memory = byte;
}

void before_eviction(uint64_t set_index, struct cache_line *victim) {
  // Implementation of eviction handling
  // This function is called before evicting a cache line
  // You can perform any necessary operations on the victim cache line
  // For example, if the dirty bit is set, you can write the contents back to memory
  // In this case, since write-back is expected, we will write back the contents if the dirty bit is set
  if (victim->dirty) {
    uint64_t addr = (victim->tag << (victim->cache->index_bits + victim->cache->offset_bits)) |
                    (set_index << victim->cache->offset_bits);
    for (uint64_t i = 0; i < victim->cache->config.line_size; i++) {
      mem_write(addr + i, victim->data[i]);
    }
  }
}

struct cashier *cashier_init(struct cache_config config) {
  // Allocate memory for the cache structure
  struct cashier *cache = (struct cashier *)malloc(sizeof(struct cashier));
  if (cache == NULL) {
    return NULL; // Memory allocation failure
  }

  // Set the cache simulator configuration
  cache->config = config;

  // Calculate the number of bits for tag, index, and offset
  cache->tag_bits = config.address_bits - (uint64_t)log2(config.line_size) - (uint64_t)log2(config.lines);
  cache->index_bits = (uint64_t)log2(config.lines) - (uint64_t)log2(config.ways);
  cache->offset_bits = (uint64_t)log2(config.line_size);
  
  // Calculate the bit masks for tag, index, and offset
  cache->tag_mask = ((uint64_t)1 << cache->tag_bits) - 1;
  cache->index_mask = ((uint64_t)1 << cache->index_bits) - 1;
  cache->offset_mask = ((uint64_t)1 << cache->offset_bits) - 1;

  // Calculate the total size of the cache in bytes
  cache->size = config.line_size * config.lines;

  // Allocate memory for the cache lines
  cache->lines = (struct cache_line *)malloc(sizeof(struct cache_line) * config.lines);
  if (cache->lines == NULL) {
    free(cache); // Memory allocation failure, free previously allocated memory
    return NULL;
  }

  // Initialize the cache lines
  for (uint64_t i = 0; i < config.lines; i++) {
    cache->lines[i].valid = false;
    cache->lines[i].dirty = false;
    cache->lines[i].tag = 0;
    cache->lines[i].last_access = 0;
    cache->lines[i].data = (uint8_t *)malloc(sizeof(uint8_t) * config.line_size);
    if (cache->lines[i].data == NULL) {
      for (uint64_t j = 0; j < i; j++) {
        free(cache->lines[j].data); // Memory allocation failure, free previously allocated data memory
      }
      free(cache->lines); // Memory allocation failure, free previously allocated cache lines
      free(cache); // Memory allocation failure, free previously allocated cache structure
      return NULL;
    }
  }

  return cache;
}

void cashier_release(struct cashier *cache) {
  if (cache == NULL) {
    return;
  }

  // Write back dirty cache lines before releasing resources
  for (uint64_t i = 0; i < cache->config.lines; i++) {
    if (cache->lines[i].valid && cache->lines[i].dirty) {
      before_eviction(i >> cache->index_bits, &cache->lines[i]);
    }
  }

  // Free the allocated memory for data and cache lines
  for (uint64_t i = 0; i < cache->config.lines; i++) {
    free(cache->lines[i].data);
  }
  free(cache->lines);

  // Free the allocated memory for the cache structure
  free(cache);
}

bool cashier_read(struct cashier *cache, uint64_t addr, uint8_t *byte) {
  if (cache == NULL) {
    return false;
  }

  // Extract the tag, index, and offset from the address
  uint64_t tag = addr >> (cache->index_bits + cache->offset_bits);
  uint64_t index = (addr >> cache->offset_bits) & cache->index_mask;
  uint64_t offset = addr & cache->offset_mask;

  // Find the cache line corresponding to the address
  for (uint64_t i = 0; i < cache->config.ways; i++) {
    uint64_t line_index = (index << cache->index_bits) | i;
    if (cache->lines[line_index].valid && cache->lines[line_index].tag == tag) {
      // Cache hit
      cache->lines[line_index].last_access = get_timestamp();
      *byte = cache->lines[line_index].data[offset];
      return true;
    }
  }

  // Cache miss, fetch the cache line from memory
  uint64_t line_index = (index << cache->index_bits) | 0; // Use the first slot in the set for replacement
  if (cache->lines[line_index].valid && cache->lines[line_index].dirty) {
    before_eviction(index, &cache->lines[line_index]); // Evict the cache line and write back if dirty
  }

  // Read the cache line from memory
  uint64_t line_addr = (tag << (cache->index_bits + cache->offset_bits)) | (index << cache->offset_bits);
  for (uint64_t i = 0; i < cache->config.line_size; i++) {
    cache->lines[line_index].data[i] = mem_read(line_addr + i);
  }

  // Update the cache line information
  cache->lines[line_index].valid = true;
  cache->lines[line_index].dirty = false;
  cache->lines[line_index].tag = tag;
  cache->lines[line_index].last_access = get_timestamp();

  *byte = cache->lines[line_index].data[offset];

  return false;
}

bool cashier_write(struct cashier *cache, uint64_t addr, uint8_t byte) {
  if (cache == NULL) {
    return false;
  }

  // Extract the tag, index, and offset from the address
  uint64_t tag = addr >> (cache->index_bits + cache->offset_bits);
  uint64_t index = (addr >> cache->offset_bits) & cache->index_mask;
  uint64_t offset = addr & cache->offset_mask;

  // Find the cache line corresponding to the address
  for (uint64_t i = 0; i < cache->config.ways; i++) {
    uint64_t line_index = (index << cache->index_bits) | i;
    if (cache->lines[line_index].valid && cache->lines[line_index].tag == tag) {
      // Cache hit
      cache->lines[line_index].last_access = get_timestamp();
      cache->lines[line_index].data[offset] = byte;
      cache->lines[line_index].dirty = true;
      return true;
    }
  }

  // Cache miss, fetch the cache line from memory
  uint64_t line_index = (index << cache->index_bits) | 0; // Use the first slot in the set for replacement
  if (cache->lines[line_index].valid && cache->lines[line_index].dirty) {
    before_eviction(index, &cache->lines[line_index]); // Evict the cache line and write back if dirty
  }

  // Read the cache line from memory
  uint64_t line_addr = (tag << (cache->index_bits + cache->offset_bits)) | (index << cache->offset_bits);
  for (uint64_t i = 0; i < cache->config.line_size; i++) {
    cache->lines[line_index].data[i] = mem_read(line_addr + i);
  }

  // Update the cache line information
  cache->lines[line_index].valid = true;
  cache->lines[line_index].dirty = true;
  cache->lines[line_index].tag = tag;
  cache->lines[line_index].last_access = get_timestamp();

  cache->lines[line_index].data[offset] = byte;

  return false;
}
