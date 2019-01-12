#include "arm_timer.h"
#include "utils.h"
#include "gpio.h"
#include "stdout.h"

void handler() {
  gpio_invert_led();
  prntf("interrupt handler!\r\n");
}

void clearer() {
  mmio_write(armTimerIRQClear, 0);
}

void arm_timer_init(uint32_t value) {
  register_irq_handler(ARM_TIMER, handler, clearer);

  // timer value to wait for
  mmio_write(armTimerLoad, value);  
  // 32 bit timer, enable timer and timer irq, 256 prescalar
  mmio_write(armTimerControl, 0x000000aa);
}