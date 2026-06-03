#ifndef clox_memory_h
#define clox_memory_h

#include <cstdlib>
#include <cstdio>
#include <cstring>

// Unified memory allocation/reallocation/deallocation function
// If previous is NULL, allocates new memory
// If newSize is 0, frees the memory
// Otherwise, reallocates the memory
inline void* reallocate(void* previous, size_t oldSize, size_t newSize) {
  if (newSize == 0) {
    free(previous);
    return NULL;
  }

  void* result = realloc(previous, newSize);
  if (result == NULL) {
    fprintf(stderr, "Not enough memory to allocate\n");
    exit(1);
  }
  return result;
}

#endif
