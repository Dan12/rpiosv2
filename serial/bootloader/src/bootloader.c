#include <stdint.h>
#include "uart.h"

extern uint8_t __end;
extern uint8_t __bss_start;
extern uint8_t __bss_end;

extern void BRANCHTO (uint32_t);

void dump(uint8_t* start, uint32_t num) {
  uart_puts("\r\nStarting dump\r\n");
  uint32_t i;
  for (i = 0; i < num; i++) {
    if (i%32 == 0) {
      uart_puts("\r\n");
    }
    uart_putc((unsigned char) *(start+i));
  }
  uart_puts("\r\nFinished dump\r\n");
}

void bootloader_main(void) {

  // as per c spec, unitialized bss must be zeroed out
  uint8_t* pBSS;
  for (pBSS = &__bss_start; pBSS < &__bss_end; pBSS++) {
    *pBSS = 0;
  }

  uart_init();

  uart_puts("Waiting for input\r\n");

  unsigned char packet[10];
  // 0 -> type
  // if type == 1
  // 1-4 -> address
  // 5-8 -> data
  // if type == 2
  // 1-4 -> address to jump to
  // 5-8 -> 0xff
  // if type == 4
  // 1-4 -> address to read
  // 5-8 -> 0xff
  // 9 -> xor of previous 9 bytes

  int pidx = 0;

  while (1) {
    packet[pidx++] = uart_getc();
    if (pidx == 10) {
      pidx = 0;
      unsigned char cs = 0;
      int i;
      // calculate check sum
      for(i = 0; i < 9; i++) {
        cs = cs ^ packet[i];
      }
      // make sure check sum is correct
      if (cs == packet[9]) {
        uint32_t p1 = 0;
        // all packets have an address
        for (i = 4; i >= 1; i--) {
          p1 <<= 8;
          p1 |= packet[i];
        }
        if (packet[0] == 1) {
          // store the 4 bytes at the designated location
          for (i = 0; i < 4; i++) {
            *(((uint8_t*) p1) + i) = packet[5+i];
          }
        } else if (packet[0] == 2) {
          if (packet[5] == 0xff) {
            // dump((uint8_t*)0x8000, 0x100);
            // jump to the designated location (usually 0x8000)
            BRANCHTO(p1);
          } else {
            // invalid jump
            uart_putc(0x00);
            uart_putc(0x01);
          }
        } else if (packet[0] == 4) {
          if (packet[5] == 0xff) {
            // read the specified byte
            uart_putc(0x88);
            uart_putc((unsigned char) *(((uint8_t*)p1)));
          } else {
            // uart_puts("Invalid read\r\n");  
            uart_putc(0x00);
            uart_putc(0x02);
          }
        } else {
          // uart_puts("Invalid packet type\r\n");  
          uart_putc(0x00);
          uart_putc(0x03);
        }
      } else {
        // uart_puts("Invalid packet\r\n");
        uart_putc(0x00);
        uart_putc(0x04);
      }
      uart_putc(0xff);
    }
  }
}