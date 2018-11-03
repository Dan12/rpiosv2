.section ".text.boot"

.global _start

_start:
  mrc p15, #0, r0, c0, c0, #5

  // set the stack pointer
  mov sp, #0x8000

  // Call kernel_main (should never return)
  bl kernel_main