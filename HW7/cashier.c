#include "cashier.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

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

void invalid_cache(struct cashier *cache, uint64_t line_index, uint64_t tag, bool valid, bool dirty, uint64_t addr) {
    // update the tag
    cache->lines[line_index].tag = tag;
    // update the valid bit
    cache->lines[line_index].valid = valid;
    // update the dirty bit
    cache->lines[line_index].dirty = dirty;
    // update the last access timestamp
    cache->lines[line_index].last_access = get_timestamp();
    // read the data from memory
    for (uint64_t j = 0; j < cache->config.line_size; j++) {
      // read the data from memory
      cache->lines[line_index].data[j] = mem_read(addr - (addr & cache->offset_mask) + j);
    }
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
  cache->tag_bits = config.address_bits - (uint64_t)log2_base2(config.line_size) - (uint64_t)log2_base2(config.lines / config.ways);
  // Calculate the number of bits for index
  cache->index_bits = (uint64_t)log2_base2(config.lines / config.ways);
  // Calculate the number of bits for offset
  cache->offset_bits = (uint64_t)log2_base2(config.line_size);
  
  // Calculate the bit masks for tag, index, and offset
  cache->tag_mask = (((uint64_t)1 << cache->tag_bits) - 1) << (cache->offset_bits + cache->index_bits);
  // Calculate the bit masks for index
  cache->index_mask = (((uint64_t)1 << cache->index_bits) - 1) << cache->offset_bits;
  // Calculate the bit masks for offset
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
      // if the cache line is valid, firstly write back to memory
      if (cache->lines[(j * cache->config.ways) + i].valid) {
        // call the before_eviction function
        before_eviction(j, &cache->lines[(j * cache->config.ways) + i]);
        // if the cache line is dirty, write back to memory
        if (cache->lines[(j * cache->config.ways) + i].dirty) {
          // write back to memory
          for (uint64_t k = 0; k < cache->config.line_size; k++) {
            // printf("11111111 %d\n", cache->lines[(j * cache->config.ways) + i].data[k]);
            // printf("11111111 %llu\n", cache->lines[(j * cache->config.ways) + i].tag << (cache->index_bits + cache->offset_bits) | (j << cache->offset_bits) + k);
            // write back to memory
            mem_write(((cache->lines[(j * cache->config.ways) + i].tag << (cache->index_bits + cache->offset_bits)) | (j << cache->offset_bits)) + k, 
              cache->lines[(j * cache->config.ways) + i].data[k]);
          }
        }
        free(cache->lines[(j * cache->config.ways) + i].data);
      }
      // if the cache line is not valid, free the allocated memory for the cache line data
      if (!cache->lines[(j * cache->config.ways) + i].valid) {
        // Free the allocated memory for the cache line data
        free(cache->lines[(j * cache->config.ways) + i].data);
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
  uint64_t tag = (addr & cache->tag_mask) >> (cache->config.address_bits - cache->tag_bits);
  // Extract the tag, index, and offset from the address
  uint64_t index = (addr & cache->index_mask) >> cache->offset_bits;
  // Extract the tag, index, and offset from the address
  uint64_t offset = addr & cache->offset_mask;

  // calculate the corresponding cache line index and loop the cache lines
  for (uint64_t i = 0; i < cache->config.ways; i++) {
    // calculate the corresponding cache line index
    uint64_t line_index = i + index * cache->config.ways;
    // if the cache line is valid and the tag matches, it is a cache hit
    if (cache->lines[line_index].valid && cache->lines[line_index].tag == tag) {
      // Cache hit, read the data from the cache line and set the last access timestamp
      // read the data from the cache line
      *byte = cache->lines[line_index].data[offset];
      // update the last access timestamp
      cache->lines[line_index].last_access = get_timestamp();
      // return true
      return true;
    }
  }

  // Cache miss, with lru replacement policy, firstly find the least recently used cache line
  uint64_t lru_index = index * cache->config.ways;
  // calculate the corresponding cache line index and loop the cache lines
  for (uint64_t i = 0; i < cache->config.ways; i++) {
    // calculate the corresponding cache line index
    uint64_t line_index = i + index * cache->config.ways;
    // if the cache line is not valid, it is the least recently used cache line
    if (!cache->lines[line_index].valid) {
      // deal with the invalid cache line
      invalid_cache(cache, line_index, tag, true, false, addr);
      // read the data from the cache line
      *byte = cache->lines[line_index].data[offset];
      // return false
      return false;
    }
    // if the cache line is valid and the last access timestamp is smaller than the lru index, update the lru index
    if (cache->lines[line_index].last_access < cache->lines[lru_index].last_access) {
      // update the lru index
      lru_index = line_index;
    }
  }
  // call the before_eviction function
  before_eviction(index, &cache->lines[lru_index]);
  // if the cache line is dirty, write back to memory
  if (cache->lines[lru_index].dirty) {
    // write back to memory
    for (uint64_t i = 0; i < cache->config.line_size; i++) {
      // write back to memory
      mem_write(((cache->lines[lru_index].tag << (cache->index_bits + cache->offset_bits)) | (index << cache->offset_bits)) + i,
        cache->lines[lru_index].data[i]);
    }
  }
  // deal with the invalid cache line
  invalid_cache(cache, lru_index, tag, true, false, addr);
  // read the data from the cache line
  *byte = cache->lines[lru_index].data[offset];
  // return false
  return false;
}

bool cashier_write(struct cashier *cache, uint64_t addr, uint8_t byte) {
  // Check for NULL pointer
  if (cache == NULL) {
    // NULL pointer
    return false;
  }

  // Extract the tag, index, and offset from the address
  uint64_t tag = (addr & cache->tag_mask) >> (cache->index_bits + cache->offset_bits);
  // Extract the tag, index, and offset from the address
  uint64_t index = (addr & cache->index_mask) >> cache->offset_bits;
  // Extract the tag, index, and offset from the address
  uint64_t offset = addr & cache->offset_mask;

  // Find the cache line corresponding to the address
  for (uint64_t i = 0; i < cache->config.ways; i++) {
    // calculate the corresponding cache line index
    uint64_t line_index = i + index * cache->config.ways;
    // if the cache line is valid and the tag matches, it is a cache hit
    if (cache->lines[line_index].valid && cache->lines[line_index].tag == tag) {
      // Cache hit, write the data to the cache line and set the last access timestamp
      // write the data to the cache line
      cache->lines[line_index].data[offset] = byte;
      // update the last access timestamp
      cache->lines[line_index].last_access = get_timestamp();
      // update the dirty bit
      cache->lines[line_index].dirty = true;
      // return true
      return true;
    }
  }

  // case cashier miss, with lru replacement policy, firstly find the least recently used cache line
  uint64_t lru_index = index * cache->config.ways;
  // calculate the corresponding cache line index and loop the cache lines
  for (uint64_t i = 0; i < cache->config.ways; i++) {
    // calculate the corresponding cache line index
    uint64_t line_index = i + index * cache->config.ways;
    // if the cache line is not valid, it is the least recently used cache line
    if (!cache->lines[line_index].valid) {
      // deal with the invalid cache line
      invalid_cache(cache, line_index, tag, true, true, addr);
      // write the data to the cache line
      cache->lines[line_index].data[offset] = byte;
      // return false
      return false;
    }
    // if the cache line is valid and the last access timestamp is smaller than the lru index, update the lru index
    if (cache->lines[line_index].last_access < cache->lines[lru_index].last_access) {
      // update the lru index
      lru_index = line_index;
    }
  }

  // call the before_eviction function
  before_eviction(index, &cache->lines[lru_index]);
  // if the cache line is dirty, write back to memory
  if (cache->lines[lru_index].dirty) {
    // write back to memory
    for (uint64_t i = 0; i < cache->config.line_size; i++) {
      // write back to memory
      mem_write(((cache->lines[lru_index].tag << (cache->index_bits + cache->offset_bits)) | (index << cache->offset_bits)) + i,
        cache->lines[lru_index].data[i]);
    }
  }
  // deal with the invalid cache line
  invalid_cache(cache, lru_index, tag, true, true, addr);
  // write the data to the cache line
  cache->lines[lru_index].data[offset] = byte;
  // return false
  return false;
}
