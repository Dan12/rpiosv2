#include "peripheral.h"
#include "interrupts.h"

#if !defined(_ARM_TIMER)
#define _ARM_TIMER

#define armTimerLoad (INTERRUPTS_BASE + 0x400)
#define armTimerValue (INTERRUPTS_BASE + 0x404)
#define armTimerControl (INTERRUPTS_BASE + 0x408)
#define armTimerIRQClear (INTERRUPTS_BASE + 0x40c)

void arm_timer_init();

#define TIMER_PERIOD_US 50000

typedef void (*timer_handler)();
void set_timer_handler(timer_handler handler);

#endif // _ARM_TIMER
