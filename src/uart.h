#if !defined(_UART)
#define _UART

void uart_init();
void uart_putc(unsigned char c);
unsigned char uart_getc();
void uart_puts(const char* str);

#endif // _UART
