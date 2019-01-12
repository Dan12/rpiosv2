#include "timer.h"

static timer_registers_t *timer_regs;

void timer_init() {
  timer_regs = (timer_registers_t *)SYSTEM_TIMER_BASE;
}

__attribute__((optimize(0))) uint32_t get_time_low() {
  return timer_regs->counter_low;
}

__attribute__((optimize(0))) uint32_t get_time_high() {
  return timer_regs->counter_high;
}

__attribute__((optimize(0))) void udelay(uint32_t usecs) {
  volatile uint32_t curr = timer_regs->counter_low;
  volatile uint32_t x = timer_regs->counter_low - curr;
  while (x < usecs) {
    x = timer_regs->counter_low - curr;
  }
}
