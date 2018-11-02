#include <stddef.h>
#include <stdint.h>
#include "peripheral.h"
#include "gpio.h"
#include "utils.h"

// Got these values from here: https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
enum {
  // The base address for UART.
  UART0_BASE = PERIPHERAL_BASE + UART0_OFFSET,

  UART0_DR = (UART0_BASE + 0x00),
  UART0_RSRECR = (UART0_BASE + 0x04),
  UART0_FR = (UART0_BASE + 0x18),
  UART0_ILPR = (UART0_BASE + 0x20),
  UART0_IBRD = (UART0_BASE + 0x24),
  UART0_FBRD = (UART0_BASE + 0x28),
  UART0_LCRH = (UART0_BASE + 0x2C),
  UART0_CR = (UART0_BASE + 0x30),
  UART0_IFLS = (UART0_BASE + 0x34),
  UART0_IMSC = (UART0_BASE + 0x38),
  UART0_RIS = (UART0_BASE + 0x3C),
  UART0_MIS = (UART0_BASE + 0x40),
  UART0_ICR = (UART0_BASE + 0x44),
  UART0_DMACR = (UART0_BASE + 0x48),
  UART0_ITCR = (UART0_BASE + 0x80),
  UART0_ITIP = (UART0_BASE + 0x84),
  UART0_ITOP = (UART0_BASE + 0x88),
  UART0_TDR = (UART0_BASE + 0x8C),
};

// got some of this info from here: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0183g/I49860.html
void uart_init() {
  // Disable UART0.
  mmio_write(UART0_CR, 0x00000000);

  // Setup the GPIO pin 14 && 15.
  // Disable pull up/down for all GPIO pins & delay for 150 cycles.
  mmio_write(GPPUD, 0x00000000);
  delay(150);
  
  // Disable pull up/down for pin 14,15 & delay for 150 cycles.
  mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
  delay(150);

  // Write 0 to GPPUDCLK0 to make it take effect.
  mmio_write(GPPUDCLK0, 0x00000000);

  // Clear pending interrupts.
  mmio_write(UART0_ICR, 0x7FF);

  // Set integer & fractional part of baud rate.
  // Divider = UART_CLOCK/(16 * Baud)
  // Fraction part register = (Fractional part * 64) + 0.5
  // UART_CLOCK = 3000000; Baud = 115200.

  uint32_t baud = 115200;
  uint32_t UART_CLOCK = 48000000;
  uint32_t baud_16 = baud * 16;
  uint32_t int_div = UART_CLOCK / (baud_16);
  uint32_t frac_div_2 = (UART_CLOCK % baud_16) * 8 / baud;
	uint32_t frac_div = frac_div_2 / 2 + frac_div_2 % 2;

  // Divider = 3000000 / (16 * 115200) = 1.627 = ~1.
  mmio_write(UART0_IBRD, int_div);
  // Fractional part register = (.627 * 64) + 0.5 = 40.6 = ~40.
  mmio_write(UART0_FBRD, frac_div);

  // Enable FIFO & 8 bit data transmissio (1 stop bit, no parity).
  mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));

  // Mask all interrupts.
  mmio_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
            (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));

  // Enable UART0, receive & transfer part of UART.
  mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

void uart_putc(unsigned char c) {
  while (mmio_read(UART0_FR) & (1 << 5)) {
  }
  mmio_write(UART0_DR, c);
}

unsigned char uart_getc() {
  while (mmio_read(UART0_FR) & (1 << 4)) {
  }
  return mmio_read(UART0_DR);
}

void uart_puts(const char* str) {
  for (size_t i = 0; str[i] != '\0'; i++) uart_putc((unsigned char)str[i]);
}