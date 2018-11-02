#include "timer.h"
#include "interrupts.h"

static timer_registers_t *timer_regs;
static void(*handle_timer)(void);

static void timer_irq_handler(void) { handle_timer(); }

static void timer_irq_clearer(void) { timer_regs->control.timer1_matched = 1; }

void timer_init(void(*handler)(void)) {
  handle_timer = handler;
  timer_regs = (timer_registers_t *)SYSTEM_TIMER_BASE;
  // register_irq_handler(SYSTEM_TIMER_1, timer_irq_handler, timer_irq_clearer);
}

void timer_set(uint32_t usecs) {
  timer_regs->timer1 = timer_regs->counter_low + usecs;
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
