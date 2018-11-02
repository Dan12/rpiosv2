#include "atag.h"
#include "stdlib.h"
#include "uart.h"

uint32_t get_mem_size(atag_t *tag) {
  while (tag->tag != NONE) {
    if (tag->tag == MEM) {
      return tag->mem.size;
    }
    tag = (atag_t *)(((uint32_t *)tag) + tag->tag_size);
  }

  // 1<<27 = 128mb
  // we will play it safe by only saying 1/4th of that, which is still 32mb
  return 1 << 24;
}

void dump_atag(atag_t *tag) {
  while(tag->tag != NONE) {
    uart_puts("Tag: ");
    uart_puts(itoa(tag->tag, 16));
    uart_puts("\r\n");

    uart_puts("size: ");
    uart_puts(itoa(tag->tag_size, 16));
    uart_puts("\r\n");

    if (tag->tag != CORE && tag->tag != MEM && tag->tag != INITRD2 && tag->tag != CMDLINE) {
      break;
    }

    tag = tag + 1;
  }
}