#include "stdout.h"
#include <stdarg.h>
#include "uart.h"
#include "gpu.h"
#include "stdlib.h"

void putc(char c) {
    gpu_putc(c);
    uart_putc(c);
}

void puts(const char * str) {
    int i;
    for (i = 0; str[i] != '\0'; i ++)
        putc(str[i]);
}

void prntf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  for (; *fmt != '\0'; fmt++) {
    if (*fmt == '%') {
      switch (*(++fmt)) {
        case '%':
          putc('%');
          break;
        case 'd':
          puts(itoa(va_arg(args, int), 10));
          break;
        case 'x':
          puts(itoa(va_arg(args, int), 16));
          break;
        case 's':
          puts(va_arg(args, char *));
          break;
      }
    } else
      putc(*fmt);
  }

  va_end(args);
}
