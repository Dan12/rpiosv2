#include <stdint.h>
#include "uart.h"
#include "atag.h"
#include "stdlib.h"
#include "gpio.h"
#include "timer.h"
#include "interrupts.h"

extern uint8_t __end;

static int led_state = 1;

// ~.5 seconds
void handle_timer() {
  gpio_write_led(led_state);
  led_state = !led_state;
  timer_set(500000);
}

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags) {
  (void)r0;
  (void)r1;
  (void)atags;

  uart_init();
  uart_puts("Hello, kernel World!\r\n");

  // free space from __end to mem_size
  uint32_t kernel_top = (uint32_t)&__end;

  uart_puts(itoa(kernel_top, 16));
  uart_puts("\r\n");

  uint32_t mem_size = get_mem_size((atag_t *) atags);
  uart_puts(itoa(mem_size, 16));
  uart_puts("\r\n");

  // interrupts_init();

  timer_init(handle_timer);

  gpio_led_init();

  // timer_set(500000);

  uart_puts("R0: ");
  uart_puts(itoa(r0, 16));
  uart_puts("\r\n");
  uart_puts("R1: ");
  uart_puts(itoa(r1, 16));
  uart_puts("\r\n");
  uart_puts("ATags at: ");
  uart_puts(itoa(atags, 16));
  uart_puts("\r\n");

  while(1) {
    uart_puts("Hello, kernel World!\r\n");
    udelay(1000000);
    gpio_write_led(led_state);
    led_state = !led_state;
  }

  while (1) {
    uart_putc(uart_getc());
    uart_puts("\r\n");
    uart_puts(itoa(get_time_high(), 16));
    uart_puts(itoa(get_time_low(), 16)+2);
    uart_puts("\r\n");
    gpio_write_led(led_state);
    led_state = !led_state;
  }
}