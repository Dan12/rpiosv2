#include "framebuffer.h"
#include "gpu.h"
#include "mailbox.h"
#include "stdlib.h"
#include "uart.h"

uint32_t framebuffer_init(void) {
  property_message_tag_t tags[6];

  tags[0].proptag = FB_SET_PHYSICAL_DIMENSIONS;
  tags[0].value_buffer.fb_screen_size.width = 656;
  tags[0].value_buffer.fb_screen_size.height = 416;
  tags[1].proptag = FB_SET_VIRTUAL_DIMENSIONS;
  tags[1].value_buffer.fb_screen_size.width = 656;
  tags[1].value_buffer.fb_screen_size.height = 416;
  tags[2].proptag = FB_SET_BITS_PER_PIXEL;
  tags[2].value_buffer.fb_bits_per_pixel = COLORDEPTH;
  tags[3].proptag = FB_ALLOCATE_BUFFER;
  tags[3].value_buffer.fb_allocate_align = 16;
  tags[4].proptag = NULL_TAG;

  // Send over the initialization
  if (send_messages(tags) != 0) {
    return -1;
  }

  fbinfo.width = tags[0].value_buffer.fb_screen_size.width;
  fbinfo.height = tags[0].value_buffer.fb_screen_size.height;
  fbinfo.chars_width = fbinfo.width / CHAR_WIDTH;
  fbinfo.chars_height = fbinfo.height / CHAR_HEIGHT;
  fbinfo.chars_x = 0;
  fbinfo.chars_y = 0;
  fbinfo.pitch = fbinfo.width * BYTES_PER_PIXEL;

  uart_puts("framebuffer width: ");
  uart_puts(itoa(fbinfo.width, 16));
  uart_puts("\r\n");

  uart_puts("framebuffer height: ");
  uart_puts(itoa(fbinfo.height, 16));
  uart_puts("\r\n");

  // fbinfo.buf = tags[3].value_buffer.fb_allocate_res.fb_addr;
  // map frame buffer from bus address to physical address space
  fbinfo.buf = (void*) (((uint32_t) tags[3].value_buffer.fb_allocate_res.fb_addr) & 0x3FFFFFFF);
  fbinfo.buf_size = tags[3].value_buffer.fb_allocate_res.fb_size;

  uart_puts("framebuffer buffer: ");
  uart_puts(itoa((uint32_t)fbinfo.buf, 16));
  uart_puts("\r\n");

  uart_puts("framebuffer size: ");
  uart_puts(itoa(fbinfo.buf_size, 16));
  uart_puts("\r\n");

  return 0;
}