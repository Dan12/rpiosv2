.section ".text.boot"

.global _start
_start:
  b _boot

.space 0x20000-4,0
_boot:
  // set the stack pointer
  mov sp,#0x200000

  // Call kernel_main (should never return)
  bl bootloader_main

.globl BRANCHTO
BRANCHTO:
    bx r0