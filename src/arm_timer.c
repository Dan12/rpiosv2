#include "arm_timer.h"
#include "utils.h"
#include "gpio.h"
#include "stdout.h"
#include "sys_timer.h"
#include "smemory.h"

static timer_handler h = NULL;

void handler() {
  if (h) {
    h();
  }
}

void set_timer_handler(timer_handler handler) {
  h = handler;
}

void clearer() {
  mmio_write(armTimerIRQClear, 0);
}

void arm_timer_init() {
  register_irq_handler(ARM_TIMER, handler, clearer);

  // value * 256 is time is microseconds for timer
  mmio_write(armTimerLoad, TIMER_PERIOD_US >> 8);
  // 32 bit timer, enable timer and timer irq, 256 prescalar
  mmio_write(armTimerControl, 0x000000aa);
}