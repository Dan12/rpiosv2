.section ".text.boot"

.global _start
_start:
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
data_handler:       .word hang
hyp_handler:        .word hang
irq_handler:        .word irq
fiq_handler:        .word hang

reset:
  // set the interrupt vector location to address #0x8000 instead of #0x0000 
  // the top of this file is at that address, so everything works out and we don't
  // have to copy over stuff
  mov r0,#0x8000
  MCR p15, 4, r0, c12, c0, 0

  mrc p15, #0, r0, c0, c0, #5

  // set the stack pointer
  mov sp, #0x8000

  // Call kernel_main (should never return)
  bl kernel_main
hang: b hang

irq:
  push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
  bl c_irq_handler
  pop  {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
  // use eret since we are probably in HYP mode due to new start.elf and bootcode.bin
  // also takes care of address realignmnet
  eret