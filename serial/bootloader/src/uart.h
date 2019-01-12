#if !defined(_UART)
#define _UART

#define UART_CLOCK_ID 0x000000002

void uart_init();
void uart_putc(unsigned char c);
unsigned char uart_getc();
void uart_puts(const char* str);

#endif // _UART
