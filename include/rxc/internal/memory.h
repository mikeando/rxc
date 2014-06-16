#ifndef RXC_INTERNAL_MEMORY_H
#define RXC_INTERNAL_MEMORY_H

#include <stdlib.h>
#include <string.h>

static inline void* rxc__malloc(size_t length) {
  void * rv = malloc(length);
  memset(rv, 0xAB, length);
  return rv;
}

static inline void rxc__free(void * p ) {
  free(p);
}


#endif

