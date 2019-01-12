#include "memory.h"

static uint32_t heap_ptr;

void init_memory(uint32_t mem_ptr) {
  heap_ptr = mem_ptr;
}

// really dump memory allocator (no method to free)
void* get_mem(uint32_t size) {
  size = ((size >> 4) + 1) << 4;

  void* ret = (void*) heap_ptr;
  heap_ptr += size;
  return ret;
}