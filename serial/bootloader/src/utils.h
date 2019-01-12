#include <stdint.h>

#if !defined(_UTILS)
#define _UTILS

static inline void mmio_write(uint32_t reg, uint32_t data) {
  *(volatile uint32_t*)reg = data;
}

static inline uint32_t mmio_read(uint32_t reg) {
  return *(volatile uint32_t*)reg;
}

// Loop <delay> times in a way that the compiler won't optimize away
static inline void delay(int32_t count) {
  asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
               : "=r"(count)
               : [count] "0"(count)
               : "cc");
}

#endif // _UTILS
