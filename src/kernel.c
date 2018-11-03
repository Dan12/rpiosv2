#include <stdint.h>
#include "uart.h"
#include "stdlib.h"
#include "gpio.h"
#include "timer.h"
#include "interrupts.h"
#include "memory.h"
#include "mailbox.h"

extern uint8_t __end;
extern uint8_t __bss_start;
extern uint8_t __bss_end;

static int led_state = 1;

static volatile int times_entered = 0;

// ~.5 seconds
void handle_timer() {
  gpio_write_led(led_state);
  led_state = !led_state;
  timer_set(500000);
}

void do_stuff() {
  uart_init();
  uart_puts("Hello, kernel World!\r\n");

  // free space from __end to mem_size
  uint32_t kernel_top = (uint32_t)&__end;

  uart_puts(itoa(kernel_top, 16));
  uart_puts("\r\n");

  // uint32_t mem_size = get_mem_size((atag_t *) atags);
  uint32_t mem_size = 1<<24;
  uart_puts(itoa(mem_size, 16));
  uart_puts("\r\n");

  // interrupts_init();

  timer_init(handle_timer);

  gpio_led_init();

  init_memory(kernel_top);

  property_message_tag_t tags[2];
  tags[0].proptag = GET_BOARD_REV;
  tags[0].value_buffer.get_board_data.bd = 0x12345678;
  tags[1].proptag = NULL_TAG;

  uint32_t res = send_messages(tags);
  uart_puts("tags resp: ");
  uart_puts(itoa(res, 16));
  uart_puts("\r\n");

  uint32_t board_info = tags[0].value_buffer.get_board_data.bd;
  uart_puts("board model: ");
  uart_puts(itoa(board_info, 16));
  uart_puts("\r\n");

  // timer_set(500000);

  while (1) {
    uart_putc(uart_getc());
    uart_puts("\r\n");
    uart_puts(itoa(get_time_high(), 16));
    uart_puts(itoa(get_time_low(), 16)+2);
    uart_puts("\r\n");
    uart_puts(itoa(times_entered, 10));
    uart_puts("\r\n");
    gpio_write_led(led_state);
    led_state = !led_state;
  }
}

void kernel_main(uint32_t r0) {
  times_entered++;

  // R0 should hold cpu info at start time (this should halt the )
  if ((r0 & 0x3) != 0) {
    while(1) {
      asm volatile ("wfi");
    }
  }

  // as per c spec, unitialized bss must be zeroed out
  uint8_t* pBSS;
	for (pBSS = &__bss_start; pBSS < &__bss_end; pBSS++) {
		*pBSS = 0;
	}

  do_stuff();
}