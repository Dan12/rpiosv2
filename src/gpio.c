#include "gpio.h"
#include <stdint.h>
#include "utils.h"

void gpio_led_init() {
  // init the led select function
  // init green led
  mmio_write(GPFSEL4, 1 << 21);
  // init red led
  mmio_write(GPFSEL3, 1 << 15);
  // default on
  gpio_invert_led_2();
}

static volatile uint32_t led_state = 1;

void gpio_invert_led() {
  // pin 15 wired to the ACT led (green)
  uint32_t ledNum = 1 << 15;
  if (led_state) {
    // turn on
    mmio_write(GPSET1, ledNum);
  } else {
    // clear
    mmio_write(GPCLR1, ledNum);
  }
  led_state = !led_state;
}

static volatile uint32_t led_state_2 = 1;

void gpio_invert_led_2() {
  // pin 15 wired to the other led (red?)
  uint32_t ledNum = 1 << 3;
  if (led_state_2) {
    // turn on
    mmio_write(GPSET1, ledNum);
  } else {
    // clear
    mmio_write(GPCLR1, ledNum);
  }
  led_state_2 = !led_state_2;
}