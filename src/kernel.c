#include <stdint.h>
#include "atomics.h"
#include "gpio.h"
#include "mailbox.h"
#include "memory.h"
#include "stdlib.h"
#include "sys_timer.h"
#include "uart.h"
#include "gpu.h"
#include "stdout.h"
#include "interrupts.h"
#include "arm_timer.h"

extern uint8_t __end;
extern uint8_t __bss_start;
extern uint8_t __bss_end;

static volatile uint32_t times_entered = 0;

void get_system_config() {
  property_message_tag_t tags[2];
  tags[0].proptag = GET_BOARD_REV;
  tags[0].value_buffer.get_board_data.bd = 0x12345678;
  tags[1].proptag = NULL_TAG;

  uint32_t res = send_messages(tags);
  uart_puts("tags resp: ");
  uart_puts(itoa(res, 16));
  uart_puts("\r\n");

  uint32_t board_info = tags[0].value_buffer.get_board_data.bd;
  uart_puts("board revision: ");
  uart_puts(itoa(board_info, 16));
  uart_puts("\r\n");

  tags[0].proptag = GET_ARM_MEMORY;
  tags[1].proptag = NULL_TAG;

  res = send_messages(tags);
  uart_puts("tags resp: ");
  uart_puts(itoa(res, 16));
  uart_puts("\r\n");

  uint32_t mem_start = tags[0].value_buffer.get_arm_memory.base_addr;
  uart_puts("mem start: ");
  uart_puts(itoa(mem_start, 16));
  uart_puts("\r\n");
  uint32_t mem_size = tags[0].value_buffer.get_arm_memory.size;
  uart_puts("mem size: ");
  uart_puts(itoa(mem_size, 16));
  uart_puts("\r\n");

  tags[0].proptag = GET_CLOCK_RATE;
  tags[0].value_buffer.get_clock_rate.clock_id = UART_CLOCK_ID;
  tags[1].proptag = NULL_TAG;

  res = send_messages(tags);
  uart_puts("tags resp: ");
  uart_puts(itoa(res, 16));
  uart_puts("\r\n");

  uint32_t uart_clock_rate = tags[0].value_buffer.get_clock_rate.rate;
  uart_puts("uart clock rate: ");
  uart_puts(itoa(uart_clock_rate, 16));
  uart_puts("\r\n");

  tags[0].proptag = FB_GET_PHYSICAL_DIMENSIONS;
  tags[1].proptag = NULL_TAG;

  res = send_messages(tags);
  uart_puts("tags resp: ");
  uart_puts(itoa(res, 16));
  uart_puts("\r\n");

  uint32_t width = tags[0].value_buffer.fb_screen_size.width;
  uart_puts("width: ");
  uart_puts(itoa(width, 16));
  uart_puts("\r\n");

  uint32_t height = tags[0].value_buffer.fb_screen_size.height;
  uart_puts("height: ");
  uart_puts(itoa(height, 16));
  uart_puts("\r\n");
}

void dump(uint32_t* pos, uint32_t num) {
  uint32_t i;
  for (i = 0; i < num; i++) {
    if (i % 4 == 0) {
      prntf("\r\n");
    }
    prntf("%x ", *(pos+i));
  }
}

void do_stuff() {
  prntf("Hello, kernel World!\r\n");
  // uart_puts("Hello, kernel World!\r\n");

  get_system_config();

  arm_timer_init(0x00001800);

  int i = 0;
  while(1) {
    prntf("hello %d\r\n", i++);
    gpio_invert_led_2();
    udelay(1000000);
  }

  while (1) {
    uart_putc(uart_getc());
    uart_puts("\r\n");
    uart_puts(itoa(get_time_high(), 16));
    uart_puts(itoa(get_time_low(), 16) + 2);
    uart_puts("\r\n");
    uart_puts(itoa(times_entered, 10));
    uart_puts("\r\n");
    gpio_invert_led();
    gpio_invert_led_2();
  }
}

void init_systems() {
  // free space from __end to mem_size
  uint32_t kernel_top = (uint32_t)&__end;

  init_memory(kernel_top);

  uart_init();

  sys_timer_init();

  gpio_led_init();

  gpu_init();

  interrupts_init();
}

void kernel_main(uint32_t r0) {
  // atomic_inc(&times_entered);
  times_entered++;

  // R0 should hold cpu info at start time (this should halt the )
  if ((r0 & 0x3) != 0) {
    while (1) {
      asm volatile("wfi");
    }
  }

  // as per c spec, unitialized bss must be zeroed out
  uint8_t* pBSS;
  for (pBSS = &__bss_start; pBSS < &__bss_end; pBSS++) {
    *pBSS = 0;
  }

  // init kernel systems
  init_systems();

  do_stuff();
}