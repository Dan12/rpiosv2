#include <stdint.h>
#include "stdout.h"

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

void dump(uint32_t* pos, uint32_t num) {
  uint32_t i;
  for (i = 0; i < num; i++) {
    if (i % 4 == 0) {
      prntf("\r\n");
      prntf("%x: ", ((uint32_t) pos) + i*4);
    }
    prntf("%x ", *(pos+i));
  }
  prntf("\r\n");
}

#define flushcache() \
  asm volatile("mcr p15, #0, %[zero], c7, c14, #0" : : [zero] "r"(0))
#define DataSyncBarrier() \
  asm volatile("mcr p15, 0, %0, c7, c10, 4" : : "r"(0) : "memory")
#define DataMemBarrier() \
  asm volatile("mcr p15, 0, %0, c7, c10, 5" : : "r"(0) : "memory")

#endif // _UTILS
