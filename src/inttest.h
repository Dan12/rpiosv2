#include <stdint.h>
#include "stdlib.h"

#if !defined(_INTTEST)
#define _INTTEST

#define BASE_IRQ_ADDR       0x3F00B218
  
/* Base Interrupt enable register */
volatile __no_init union
{
    uint32_t Base_IRQ_enable;
    struct Base_IRQ_enable_bits {
        uint32_t ARM_Timer          : 1;
        uint32_t ARM_Mailbox        : 1;
        uint32_t ARM_Doorbell_0     : 1;
        uint32_t ARM_Doorbell_1     : 1;
        uint32_t GPU_0_Halted       : 1;
        uint32_t GPU_1_Halted       : 1;
        uint32_t Access_err_type_1  : 1;
        uint32_t Access_err_type_0  : 1;
    } Base_IRQ_enable_bit;
} @ BASE_IRQ_ADDR;
  
  
#define TIMER_CNT_23BIT             1
#define TIMER_CNT_16BIT             0
  
#define TIMER_PRESCALE_256          2
  
#define TIMER_LOAD_ADDR             0x3F00B400
#define TIMER_CLR_ADDR              0x3F00B40C
#define TIMER_CTRL_ADDR             0x3F00B408
  
/* Timer Load register */
volatile __no_init uint32_t Timer_load @ TIMER_LOAD_ADDR;
  
/* Timer IRQ clear register */
volatile __no_init uint32_t Timer_irq_clear @ TIMER_CLR_ADDR;
  
/* Timer control register */
volatile __no_init union
{
    uint32_t Timer_ctrl;
    struct Timer_ctrl_bits {
        uint32_t unused1            : 1;
        uint32_t count16_23         : 1;
        uint32_t prescale           : 2;
        uint32_t unused2            : 1;
        uint32_t irq_enable         : 1;
        uint32_t unused3            : 1;
        uint32_t enable             : 1;
        uint32_t debug_halt         : 1;
        uint32_t free_run           : 1;
        uint32_t unused4            : 6;
        uint32_t free_prescale      : 8;
    } Timer_ctrl_bit;
} @ TIMER_CTRL_ADDR;
  
  
#define BCM_START_ADDR 0x00008000
#define BCM_VECT_ADDR  0x00000000
  
volatile __no_init struct reset_struct
{
    uint32_t Reset;
    uint32_t Undefined;
    uint32_t SWI;
    uint32_t Prefetch;
    uint32_t Abort;
    uint32_t Reserved;
    uint32_t IRQ;
    uint32_t FIQ;
    uint32_t jump_table[7];
} Reset_vector @ BCM_VECT_ADDR;

/*--------------------------------------------------*/
static void timer_config(void)
{
    // Copy reset vector to address 0x0 for interrupts to work
    mcpy((char*)&Reset_vector, (char*)BCM_START_ADDR, 
           sizeof(struct reset_struct));
      
    // Enable timer and interrupts
    Base_IRQ_enable_bit.ARM_Timer = 1;
    Timer_load = TIMERCNT;
      
    Timer_ctrl_bit.count16_23 = TIMER_CNT_23BIT;
    Timer_ctrl_bit.enable     = 1;
    Timer_ctrl_bit.irq_enable = 1;
    Timer_ctrl_bit.prescale   = TIMER_PRESCALE_256;
      
    __enable_interrupt();
}
  
/*--------------------------------------------------*/
__irq __arm void IRQ_Handler(void)
{
    Timer_irq_clear = 1;
  
    if (toggle) {
        GPSET1 = GPSET_35;
        toggle = 0;
    } else {
        GPCLR1 = GPCLR_35;
        toggle = 1;
    }
}

#endif // _INTTEST
