#include "cashier.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

struct cashier *cashier_init(struct cache_config config) {
  // YOUR CODE HERE
}

void cashier_release(struct cashier *cache) {
  // YOUR CODE HERE
}

bool cashier_read(struct cashier *cache, uint64_t addr, uint8_t *byte) {
  // YOUR CODE HERE
}

bool cashier_write(struct cashier *cache, uint64_t addr, uint8_t byte) {
  // YOUR CODE HERE
}
