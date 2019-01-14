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
#include "smemory.h"
#include "process.h"
#include "synch.h"

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

semaphore_t* sema;

int proc_2(arg_t arg) {
  int i = 0;
  while(1) {
    prntf("hello from 2 %d\r\n", i++);
    // gpio_invert_led_2();
    udelay(2000000);
  }
  return 0;
}

int proc_1(arg_t arg) {
  int i = 0;
  while(1) {
    prntf("hello from 1 %d\r\n", i++);
    // gpio_invert_led_2();
    udelay(1000000);
  }
  return 0;
}

int main_proc(arg_t arg) {
  prntf("Hello, kernel World!\r\n");

  get_system_config();
  
  prntf("Running main proc\r\n");
  prntf("Interrupts: %d\r\n", INTERRUPTS_ENABLED());
  process_fork(proc_1, NULL);
  process_fork(proc_2, NULL);
  return 0;
}

void init_systems() {
  // free space from __end to mem_size
  uint32_t kernel_top = (uint32_t)&__end;

  // the heap will be from the end of the text/data/bss segment
  // to the end of memory (growing up)
  // 0x100000 is kinda arbitrary rn but 1mb should be enough
  // could use the tags to more accurately determine memory size
  init_smemory(kernel_top, 0x100000);

  uart_init();

  sys_timer_init();

  gpio_led_init();

  gpu_init();

  interrupts_init();

  arm_timer_init();
}

void kernel_main(void) {
  // as per c spec, unitialized bss must be zeroed out
  uint8_t* pBSS;
  for (pBSS = &__bss_start; pBSS < &__bss_end; pBSS++) {
    *pBSS = 0;
  }

  // init kernel systems
  init_systems();

  // initialize processes
  process_system_initialize(main_proc, NULL);
}