#include "interrupts.h"
#include "stdlib.h"
#include "gpio.h"
#include "utils.h"
#include "arm_timer.h"
#include "stdout.h"

static interrupt_registers_t *interrupt_regs;

static interrupt_handler_f handlers[NUM_IRQS];
static interrupt_clearer_f clearers[NUM_IRQS];

extern void move_exception_vector(void);
extern uint32_t exception_vector;

void interrupts_init(void) {
  interrupt_regs = (interrupt_registers_t *)INTERRUPTS_PENDING;
  mzero(handlers, sizeof(interrupt_handler_f) * NUM_IRQS);
  mzero(clearers, sizeof(interrupt_clearer_f) * NUM_IRQS);
  interrupt_regs->irq_basic_disable = 0xffffffff;  // disable all interrupts
  interrupt_regs->irq_gpu_disable1 = 0xffffffff;
  interrupt_regs->irq_gpu_disable2 = 0xffffffff;
}

/**
 * this function is going to be called by the processor.  Needs to check pending
 * interrupts and execute handlers if one is registered
 */
void c_irq_handler(void) {
  int j;
  for (j = 0; j < NUM_IRQS; j++) {
    // If the interrupt is pending and there is a handler, run the handler
    if (IRQ_IS_PENDING(interrupt_regs, j) && (handlers[j] != 0)) {
      clearers[j]();
      // enable interrupts to allow nesting of interrupts
      ENABLE_INTERRUPTS();
      handlers[j]();
      DISABLE_INTERRUPTS();
      return;
    }
  }
}

void data_abort_expn(int location, int sr1, int sr2) {
  prntf("Data abort expn at address %x hex %x\r\n", location-8, *((uint32_t*) (location-8)));
  prntf("Select registers %x - %x\r\n", sr1, sr2);
  while(1);
}

void register_irq_handler(irq_number_t irq_num, interrupt_handler_f handler, interrupt_clearer_f clearer) {
  uint32_t irq_pos;
  if (IRQ_IS_BASIC(irq_num)) {
    irq_pos = irq_num - 64;
    handlers[irq_num] = handler;
    clearers[irq_num] = clearer;
    interrupt_regs->irq_basic_enable |= (1 << irq_pos);
  } else if (IRQ_IS_GPU2(irq_num)) {
    irq_pos = irq_num - 32;
    handlers[irq_num] = handler;
    clearers[irq_num] = clearer;
    interrupt_regs->irq_gpu_enable2 |= (1 << irq_pos);
  } else if (IRQ_IS_GPU1(irq_num)) {
    irq_pos = irq_num;
    handlers[irq_num] = handler;
    clearers[irq_num] = clearer;
    interrupt_regs->irq_gpu_enable1 |= (1 << irq_pos);
  } else {
    // printf("ERROR: CANNOT REGISTER IRQ HANDLER: INVALID IRQ NUMBER: %d\n", irq_num);
  }
}

void unregister_irq_handler(irq_number_t irq_num) {
  uint32_t irq_pos;
  if (IRQ_IS_BASIC(irq_num)) {
    irq_pos = irq_num - 64;
    handlers[irq_num] = 0;
    clearers[irq_num] = 0;
    // Setting the disable bit clears the enabled bit
    interrupt_regs->irq_basic_disable |= (1 << irq_pos);
  } else if (IRQ_IS_GPU2(irq_num)) {
    irq_pos = irq_num - 32;
    handlers[irq_num] = 0;
    clearers[irq_num] = 0;
    interrupt_regs->irq_gpu_disable2 |= (1 << irq_pos);
  } else if (IRQ_IS_GPU1(irq_num)) {
    irq_pos = irq_num;
    handlers[irq_num] = 0;
    clearers[irq_num] = 0;
    interrupt_regs->irq_gpu_disable1 |= (1 << irq_pos);
  } else {
    // printf("ERROR: CANNOT UNREGISTER IRQ HANDLER: INVALID IRQ NUMBER: %d\n", irq_num);
  }
}
