#include "peripheral.h"

#if !defined(_GPIO)
#define _GPIO

enum {
  // The GPIO registers base address.
  GPIO_BASE = PERIPHERAL_BASE + GPIO_OFFSET,

  GPPUD = (GPIO_BASE + 0x94),
  GPPUDCLK0 = (GPIO_BASE + 0x98),

  GPFSEL4 = (GPIO_BASE + 0x10),
  GPSET1 = (GPIO_BASE + 0x20),
  GPCLR1 = (GPIO_BASE + 0x2C),
};

void gpio_led_init();
void gpio_write_led(int status);

#endif  // _GPIO