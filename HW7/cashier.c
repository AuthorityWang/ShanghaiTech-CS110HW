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
}


// function cashier_release: release the resources allocated for the cache simulator.
// parameters: cache: struct cashier * address to a cache simulator created by cashier_init.
// returns: NONE.
// note
// NULL parameters is allowed.
// Make sure to eliminate possibility of memory leak.
void cashier_release(struct cashier *cache) {
  // YOUR CODE HERE
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
}
