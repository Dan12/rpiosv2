.section ".text.boot"

.global _start
_start:
  // set the stack pointer
  mov sp, #0x8000

  // Call kernel_main (should never return)
  bl kernel_main
hang: b hang

.globl PUT32
PUT32:
    str r1,[r0]
    bx lr

.globl GET32
GET32:
    ldr r0,[r0]
    bx lr

.globl dummy
dummy:
    bx lr