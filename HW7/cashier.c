#include "cashier.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

// calculate the log2
int log2_base2(uint64_t x) {
  // if zero, return -1
  if (x == 0) {
    return -1;
  }
  // if not zero, return the log2
  int ret = -1;
  // right shift until zero
  while (x != 0) {
    // right shift
    x >>= 1;
    // increment the return value
    ret++;
  }
  // return the log2
  return ret;
}

struct cashier *cashier_init(struct cache_config config) {
  // Allocate memory for the cache structure
  struct cashier *cache = (struct cashier *)malloc(sizeof(struct cashier));
  // Check for memory allocation failure
  if (cache == NULL) {
    // Memory allocation failure
    return NULL;
  }

  // Set the cache simulator configuration
  cache->config = config;

  // Calculate the number of bits for tag, index, and offset
  cache->tag_bits = config.address_bits - (uint64_t)log2_base2(config.line_size) - (uint64_t)log2_base2(config.lines);
  cache->index_bits = (uint64_t)log2_base2(config.lines) - (uint64_t)log2_base2(config.ways);
  cache->offset_bits = (uint64_t)log2_base2(config.line_size);
  
  // Calculate the bit masks for tag, index, and offset
  cache->tag_mask = ((uint64_t)1 << cache->tag_bits) - 1;
  cache->index_mask = ((uint64_t)1 << cache->index_bits) - 1;
  cache->offset_mask = ((uint64_t)1 << cache->offset_bits) - 1;

  // Calculate the total size of the cache in bytes
  cache->size = config.line_size * config.lines;

  // Allocate memory for the cache lines
  cache->lines = (struct cache_line *)malloc(sizeof(struct cache_line) * config.lines);
  // Check for memory allocation failure
  if (cache->lines == NULL) {
    // Memory allocation failure, free previously allocated memory
    free(cache);
    // Memory allocation failure
    return NULL;
  }

  // Initialize the cache lines
  for (uint64_t i = 0; i < config.lines; i++) {
    // Initialize the cache line
    cache->lines[i].valid = false;
    // Initialize the cache line
    cache->lines[i].dirty = false;
    // Initialize the cache line
    cache->lines[i].tag = 0;
    // Initialize the cache line
    cache->lines[i].last_access = 0;
    // Allocate memory for the cache line data
    cache->lines[i].data = (uint8_t *)malloc(sizeof(uint8_t) * config.line_size);
    // Check for memory allocation failure
    if (cache->lines[i].data == NULL) {
      // Memory allocation failure, free previously allocated data memory
      for (uint64_t j = 0; j < i; j++) {
        // Memory allocation failure, free previously allocated data memory
        free(cache->lines[j].data);
      }
      // Memory allocation failure, free previously allocated cache lines
      free(cache->lines);
      // Memory allocation failure, free previously allocated cache structure
      free(cache);
      // Memory allocation failure
      return NULL;
    }
  }
  // return the cache structure
  return cache;
}

void cashier_release(struct cashier *cache) {
  // Check for NULL pointer
  if (cache == NULL) {
    // NULL pointer,
    return;
  }

  // consider the sets with the number of cache lines
  for (uint64_t i = 0; i < cache->config.ways; i++) {
    // consider the cache lines
    for (uint64_t j = 0; j < (cache->config.lines / cache->config.ways); j++) {
      // if the cache line is valid, free the allocated memory for the cache line data
      if (cache->lines[(i * (cache->config.lines / cache->config.ways)) + j].valid) {
        // Free the allocated memory for the cache line data
        free(cache->lines[(i * (cache->config.lines / cache->config.ways)) + j].data);
      }
      // if the cache line is not valid, firstly write back to memory
      else {
        // if the cache line is dirty, write back to memory
        if (cache->lines[(i * (cache->config.lines / cache->config.ways)) + j].dirty) {
          // write back to memory
          before_eviction((i * (cache->config.lines / cache->config.ways)) + j, &cache->lines[(i * (cache->config.lines / cache->config.ways)) + j]);
        }
      }
    }
  }
  // Free the allocated memory for cache lines
  free(cache->lines);

  // Free the allocated memory for the cache structure
  free(cache);
}

bool cashier_read(struct cashier *cache, uint64_t addr, uint8_t *byte) {
  // Check for NULL pointer
  if (cache == NULL) {
    // NULL pointer
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
