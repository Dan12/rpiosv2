#include <stdint.h>

#if !defined(_SMEMORY)
#define _SMEMORY
#define NULL 0x0

void init_smemory(uint32_t mem_ptr, uint32_t size);

// returns 16 byte aligned blocks
void* salloc(uint32_t size);

void sfree(void* ptr);

#endif // _SMEMORY
