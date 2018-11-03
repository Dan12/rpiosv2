#include "mailbox.h"
#include "memory.h"
#include "stdlib.h"
#include "uart.h"

mail_message_t mailbox_read(uint8_t channel) {
  uint32_t stat;
  mail_message_t res;

  // Make sure that the message is from the right channel
  do {
    // Make sure there is mail to recieve
    do {
      // flushcache();
      stat = *MAIL0_STATUS;
    } while (stat & MAILBOX_EMPTY);

    // Get the message
    // dmb();
    res = *MAIL0_READ;
    // dmb();
  } while (res.channel != channel);

  return res;
}

void mailbox_send(mail_message_t msg, uint8_t channel) {
  uint32_t stat;
  msg.channel = channel;

  // Make sure you can send mail
  do {
    // flushcache();
    stat = *MAIL0_STATUS;
  } while (stat & MAILBOX_FULL);

  // send the message
  // dmb();
  *MAIL0_WRITE = msg;
}

/**
 * returns the max of the size of the request and result buffer for each tag
 */
static uint32_t get_value_buffer_len(property_message_tag_t* tag) {
  switch (tag->proptag) {
    case GET_BOARD_MODEL:
    case GET_BOARD_REV:
      return 4;
    default:
      return 0;
  }
}

int send_messages(property_message_tag_t* tags) {
  property_message_buffer_t* msg;
  mail_message_t mail;
  uint32_t bufsize = 0, i, len, bufpos;

  // Calculate the sizes of each tag
  for (i = 0; tags[i].proptag != NULL_TAG; i++) {
    bufsize += get_value_buffer_len(&tags[i]) + 3 * sizeof(uint32_t);
  }

  // Add the buffer size, buffer request/response code and buffer end tag sizes
  bufsize += 3 * sizeof(uint32_t);

  uart_puts("bsize: ");
  uart_puts(itoa((uint32_t) bufsize, 16));
  uart_puts("\r\n");

  // buffer size must be 16 byte aligned
  bufsize += (bufsize % 16) ? 16 - (bufsize % 16) : 0;

  msg = get_mem(bufsize);
  if (!msg) return -1;

  uart_puts("msg: ");
  uart_puts(itoa((uint32_t) msg, 16));
  uart_puts("\r\n");

  uart_puts("bsize: ");
  uart_puts(itoa((uint32_t) bufsize, 16));
  uart_puts("\r\n");

  msg->size = bufsize;
  msg->req_res_code = REQUEST;

  // Copy the messages into the buffer
  for (i = 0, bufpos = 0; tags[i].proptag != NULL_TAG; i++) {
    len = get_value_buffer_len(&tags[i]);
    msg->tags[bufpos++] = tags[i].proptag;
    msg->tags[bufpos++] = len;
    msg->tags[bufpos++] = 0;
    mcpy(msg->tags + bufpos, &tags[i].value_buffer, len);
    bufpos += len / 4;
  }

  msg->tags[bufpos] = 0;

  uart_puts("bufpos: ");
  uart_puts(itoa((uint32_t) bufpos, 16));
  uart_puts("\r\n");

  // Send the message (add offset)
  mail.data = ((uint32_t)msg) >> 4;

  uart_puts("mail at: ");
  uart_puts(itoa((uint32_t) &mail, 16));
  uart_puts("\r\n");

  uart_puts("mail in: ");
  uart_puts(itoa((uint32_t) msg->req_res_code, 16));
  uart_puts("\r\n");
  uart_puts(itoa((uint32_t) mail.data, 16));
  uart_puts("\r\n");
  mailbox_send(mail, PROPERTY_CHANNEL);
  uart_puts("sent mail");
  uart_puts("\r\n");
  mail = mailbox_read(PROPERTY_CHANNEL);
  uart_puts("mail out: ");
  uart_puts(itoa((uint32_t) msg->req_res_code, 16));
  uart_puts("\r\n");
  uart_puts(itoa((uint32_t) mail.data, 16));
  uart_puts("\r\n");

  if (msg->req_res_code == REQUEST) {
    // kfree(msg);
    return 1;
  }
  // Check the response code
  if (msg->req_res_code == RESPONSE_ERROR) {
    // kfree(msg);
    return 2;
  }

  if (msg->req_res_code == RESPONSE_SUCCESS) {
    // Copy the tags back into the array
    for (i = 0, bufpos = 0; tags[i].proptag != NULL_TAG; i++) {
      len = get_value_buffer_len(&tags[i]);
      bufpos += 3;  // skip over the tag bookkepping info
      mcpy(&tags[i].value_buffer, msg->tags + bufpos, len);
      bufpos += len / 4;
    }

    // kfree(msg);
    return 0;
  } else {
    // kfree(msg);
    return -1;
  }
}