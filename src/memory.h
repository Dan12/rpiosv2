#include <stdint.h>

#if !defined(_MEMORY)
#define _MEMORY

void init_memory(uint32_t mem_ptr);

void* get_mem(uint32_t size);

#endif // _MEMORY
