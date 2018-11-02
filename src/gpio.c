#include "gpio.h"
#include <stdint.h>
#include "utils.h"

void gpio_led_init() {
  // init the led select function
  mmio_write(GPFSEL4, 1 << 21);
}

void gpio_write_led(int status) {
  // pin 15 wired to the ACT led (green)
  uint32_t ledNum = 1 << 15;
  if (status) {
    // turn on
    mmio_write(GPSET1, ledNum);
  } else {
    // clear
    mmio_write(GPCLR1, ledNum);
  }
}