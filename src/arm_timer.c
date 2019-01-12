#include "arm_timer.h"
#include "utils.h"
#include "gpio.h"

void handler() {
  gpio_invert_led();
}

void clearer() {
  mmio_write(armTimerIRQClear, 0);
}

void init_timer() {
  register_irq_handler(ARM_TIMER, handler, clearer);

  mmio_write(armTimerLoad, 0x00001800);
  mmio_write(armTimerControl, 0x000000aa);
}