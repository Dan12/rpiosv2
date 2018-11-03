#include <stdint.h>

#if !defined(_ATOMICS)
#define _ATOMICS

__attribute__((always_inline)) static inline uint32_t __LDREXW(volatile uint32_t *addr) {
  uint32_t result;

  asm volatile("ldrex %0, %1" : "=r"(result) : "Q"(*addr));
  return (result);
}

__attribute__((always_inline)) static inline uint32_t __STREXW(uint32_t value, volatile uint32_t *addr) {
  uint32_t result;

  asm volatile("strex %0, %2, %1" : "=&r"(result), "=Q"(*addr) : "r"(value));
  return (result);
}

__attribute__((always_inline)) static inline void atomic_inc(volatile uint32_t *addr) {
  while(1) {
    uint32_t old = __LDREXW(addr);
    old += 1;
    if (__STREXW(old, addr) == 0) {
      break;
    }
  }
}



#endif  // _ATOMICS
