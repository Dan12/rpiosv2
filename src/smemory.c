#include "smemory.h"
#include "stdout.h"
#include "interrupts.h"

typedef struct mem_block {
  uint32_t size;
  struct mem_block* next_block;
  uint64_t padding;
} mem_block;

static mem_block* block_list_head;

mem_block* blockify(uint32_t head_ptr, uint32_t given_size) {
  mem_block* block = (mem_block*) head_ptr;
  block->next_block = NULL;
  block->size = given_size - sizeof(mem_block);

  return block;
}

static uint32_t heap_ptr;

void init_smemory(uint32_t mem_ptr, uint32_t size) {
  block_list_head = blockify(mem_ptr, size);
  // heap_ptr = mem_ptr;
}

void insert_new_block(mem_block* block) {
  block->next_block = block_list_head;
  block_list_head = block;
}

void split(mem_block* block, uint32_t req_size) {
  uint32_t new_prt = ((uint32_t) block) + sizeof(mem_block) + req_size;
  uint32_t new_avail_size = block->size - req_size - sizeof(mem_block);
  mem_block* new_block = blockify(new_prt, new_avail_size);
  insert_new_block(new_block);

  block->size = req_size;
}

int is_free(mem_block* block) {
  return (block->size & 0x1) == 0;
}

void toggle_alloc(mem_block* block) {
  block->size ^= 0x1;
}

// s prefix for simple memory allocator

void* salloc(uint32_t size) {
  interrupt_level_t prev_level = SET_INTERRUPT_LEVEL(DISABLED);
  // round up to nearest multiple of 16
  size = ((size >> 4) + 1) << 4;

  mem_block* block = block_list_head;
  while(block) {
    if (is_free(block) && block->size >= size) {
      if (block->size >= size+sizeof(mem_block)+16) {
        split(block, size);
      }
      toggle_alloc(block);
      return (void*) (((uint8_t*) block) + sizeof(mem_block));
    }
    block = block->next_block;
  }
  SET_INTERRUPT_LEVEL(prev_level);
  return NULL;

  // really simple
  // size = ((size >> 4) + 1) << 4;	

  // void* ret = (void*) heap_ptr;	
  // heap_ptr += size;	
  // SET_INTERRUPT_LEVEL(prev_level);
  // return ret;
}

void sfree(void* ptr) {
  if (ptr != NULL) {
    mem_block* block = (mem_block*) (((uint8_t*) ptr) - sizeof(mem_block));
    toggle_alloc(block);
  }
}

void dump(uint32_t* pos, uint32_t num) {
  uint32_t i;
  for (i = 0; i < num; i++) {
    if (i % 4 == 0) {
      prntf("\r\n");
      prntf("%x: ", ((uint32_t) pos) + i*4);
    }
    prntf("%x ", *(pos+i));
  }
  prntf("\r\n");
}