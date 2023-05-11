#include "cashier.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

// Access to cache are done via call to cashier_read and cashier_write.
// On cache-miss, you should make several calls to mem_read to fetch the cache line from data memory.
// If one cache line with dirty bit set is evicted, the contents should be written to memory via mem_write.
// All the cache lines are considered evicted on cashier_release.
// Before evicting a cache line, you should call the before_eviction function on that line. The exception to this is when you are replacing is invalidated cache line. Do not call before_eviction on invalid ones.
// No memory leak is allowed.


// function cashier_init: create a cache simulator with a set of parameters.
// parameters: config: struct cache_config
// specifies the number of bits of the address space config.address_bits.
// the number of cache lines in the cache config.lines.
// the size of each cache line config.line_size.
// the associativity, i.e., the number of ways in the set-associative cache. config.ways.
// the configuration validity is guaranteed
// line_size is a power of 2
// lines is a power of 2
// ways is a power of 2, ways is a factor of lines.
// returns: struct cashier* address to the initialized cache simulator.
// note
// You should return NULL on error.
// You don’t need to validate the parameters.
// Make sure no memory leak on error.
struct cashier *cashier_init(struct cache_config config) {
  // YOUR CODE HERE
  if (config.line_size == 0 || config.lines == 0 || config.address_bits == 0) {
    return NULL;
  }
  struct cashier *cache = malloc(sizeof(struct cashier));
  if (cache == NULL) {
    return NULL;
  }
  cache->config = config;
  cache->size = config.line_size * config.lines;
  cache->tag_bits = config.address_bits - (config.ways + config.line_size);
  cache->tag_mask = (1 << cache->tag_bits) - 1;
  cache->index_bits = config.address_bits - config.line_size;
  cache->index_mask = (1 << cache->index_bits) - 1;
  cache->offset_bits = config.line_size;
  cache->offset_mask = (1 << cache->offset_bits) - 1;
  cache->lines = malloc(sizeof(struct cache_line) * config.lines);
  if (cache->lines == NULL) {
    free(cache);
    return NULL;
  }
  cache->lines->data = malloc(sizeof(uint8_t) * config.line_size);
  if (cache->lines->data == NULL) {
    free(cache->lines);
    free(cache);
    return NULL;
  }
  return cache;
}


// function cashier_release: release the resources allocated for the cache simulator.
// parameters: cache: struct cashier * address to a cache simulator created by cashier_init.
// returns: NONE.
// note
// NULL parameters is allowed.
// Make sure to eliminate possibility of memory leak.
void cashier_release(struct cashier *cache) {
  // YOUR CODE HERE
  if (cache == NULL) {
    return;
  }
  for (uint64_t i = 0; i < cache->config.lines; i++) {
    if (cache->lines[i].valid) {
      free(cache->lines[i].data);
    }
  }
  free(cache->lines);
  free(cache);
}


// function cashier_read: read one byte of data at a specific address.
// parameters:
// cache: struct cashier * a cache simulator created by cashier_init, not NULL.
// addr: uint64_t the byte address.
// byte: uint8_t * used to store the result, guaranteed not to be a valid writable memory address.
// returns: true on cache hit, false on cache miss.
// note:
// To access memory on miss, always use mem_read and mem_write. Do not directly dereference addr or SIGSEGV we be raised.
// Call before_eviction if you have to evict a cache line.
// Placing the cache line at a previously invalidated slot is not considered as an eviction.
bool cashier_read(struct cashier *cache, uint64_t addr, uint8_t *byte) {
  // YOUR CODE HERE
  if (cache == NULL) {
    return false;
  }
  uint64_t tag = addr >> (cache->index_bits + cache->offset_bits);
  uint64_t index = (addr >> cache->offset_bits) & cache->index_mask;
  uint64_t offset = addr & cache->offset_mask;
  for (uint64_t i = 0; i < cache->config.ways; i++) {
    if (cache->lines[index * cache->config.ways + i].valid &&
        cache->lines[index * cache->config.ways + i].tag == tag) {
      *byte = cache->lines[index * cache->config.ways + i].data[offset];
      return true;
    }
  }
  return false;
}


// function cashier_write: write one byte of data at a specific address.
// parameters:
// cache: struct cashier * a cache simulator created by cashier_init, not NULL.
// addr: uint64_t the byte address.
// byte: uint8_t the data to be written into memory.
// returns: true on cache hit, false on cache miss.
// note:
// To access memory on miss, always use mem_read and mem_write. Do not directly dereference addr or SIGSEGV we be raised.
// Call before_eviction if you have to evict a cache line.
// Placing the cache line at a previously invalidated slot is not considered as an eviction.
// Write-back is expected rather than write-through.
bool cashier_write(struct cashier *cache, uint64_t addr, uint8_t byte) {
  // YOUR CODE HERE
  // avoid singal sigsegv
  if (cache == NULL) {
    return false;
  }
  uint64_t tag = addr >> (cache->index_bits + cache->offset_bits);
  uint64_t index = (addr >> cache->offset_bits) & cache->index_mask;
  uint64_t offset = addr & cache->offset_mask;
  for (uint64_t i = 0; i < cache->config.ways; i++) {
    if (cache->lines[index * cache->config.ways + i].valid &&
        cache->lines[index * cache->config.ways + i].tag == tag) {
      cache->lines[index * cache->config.ways + i].data[offset] = byte;
      cache->lines[index * cache->config.ways + i].dirty = true;
      return true;
    }
  }
  return false;
}
