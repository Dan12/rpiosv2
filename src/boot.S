.section ".text.boot"

.global _start
// put interrupt vector at 0x8000
_start:
  // first instruction jumps to reset handler, which is where we want to go on startup
  ldr pc,reset_handler
  ldr pc,undefined_handler
  ldr pc,swi_handler
  ldr pc,prefetch_handler
  ldr pc,data_handler
  ldr pc,hyp_handler
  ldr pc,irq_handler
  ldr pc,fiq_handler
reset_handler:      .word reset
undefined_handler:  .word hang
swi_handler:        .word hang
prefetch_handler:   .word hang
data_handler:       .word data_abort_expn_handler
hyp_handler:        .word hang
irq_handler:        .word irq
fiq_handler:        .word hang

reset:
  // set the interrupt vector location to address #0x8000 instead of #0x0000 
  // the top of this file is at that address, so everything works out and we don't
  // have to copy over stuff
  mov r0,#0x8000
  mcr p15, 4, r0, c12, c0, 0

  // set the stack pointer (grows down)
  mov sp, #0x8000

  // Call kernel_main (should never return)
  bl kernel_main
// if it does return, just hang here
.global hang
hang: b hang

data_abort_expn_handler:
  mrs r0, ELR_hyp
  mov r1, r3
  mov r2, fp
  bl data_abort_expn

irq:
  push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
  mrs r0, ELR_hyp
  mrs r1, cpsr
  push {r0, r1}
  bl c_irq_handler
  pop {r0, r1}
  msr ELR_hyp, r0 
  msr cpsr, r1
  pop  {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
  // use eret since we are probably in HYP mode due to new start.elf and bootcode.bin
  // also takes care of address realignmnet
  eret

.global context_switch
context_switch:
  push    {lr}
  push    {lr}
  push    {r0-r12}
  mrs     r12, cpsr
  push    {r12}
  str     sp, [r0]
  ldr     sp, [r1]
  pop     {r12}
  msr     cpsr, r12
  pop     {r0-r12}
  pop     {lr, pc}

.global get_cpsr
get_cpsr:
  mrs     r0, cpsr
  bx lr