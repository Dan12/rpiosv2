#include "mailbox.h"
#include "memory.h"
#include "stdlib.h"
#include "uart.h"
#include "utils.h"

uint32_t mailbox_read(uint8_t channel) {
  // uint32_t stat;
  uint32_t res;

  // Make sure that the message is from the right channel
  do {
    // Make sure there is mail to recieve
    do {
      // flushcache();

    } while (mmio_read(MAIL0_STATUS) & MAILBOX_EMPTY);

    // Get the message
    // dmb();
    res = mmio_read(MAIL0_READ);
    // dmb();
  } while ((res & 0xF) != channel);

  return res;
}

void mailbox_send(uint32_t msg, uint8_t channel) {
  // Make sure you can send mail
  do {
    // flushcache();
  } while (mmio_read(MAIL0_STATUS) & MAILBOX_FULL);

  // send the message
  // dmb();
  mmio_write(MAIL0_WRITE, msg | channel);
}

/**
 * returns the max of the size of the request and result buffer for each tag
 */
static uint32_t get_value_buffer_len(property_message_tag_t* tag) {
  switch (tag->proptag) {
    case GET_ARM_MEMORY:
    case GET_CLOCK_RATE:

    // FB stuff
    case FB_ALLOCATE_BUFFER:
    case FB_GET_PHYSICAL_DIMENSIONS:
    case FB_SET_PHYSICAL_DIMENSIONS:
    case FB_GET_VIRTUAL_DIMENSIONS:
    case FB_SET_VIRTUAL_DIMENSIONS:
      return 8;
    case GET_BOARD_MODEL:
    case GET_BOARD_REV:

    // FB stuff
    case FB_GET_BITS_PER_PIXEL:
    case FB_SET_BITS_PER_PIXEL:
    case FB_GET_BYTES_PER_ROW:
      return 4;
    case FB_RELESE_BUFFER:
    default:
      return 0;
  }
}

int send_messages(property_message_tag_t* tags) {
  property_message_buffer_t* msg;
  uint32_t mail;
  uint32_t bufsize = 0, i, len, bufpos;

  // Calculate the sizes of each tag
  for (i = 0; tags[i].proptag != NULL_TAG; i++) {
    bufsize += get_value_buffer_len(&tags[i]) + 3 * sizeof(uint32_t);
  }

  // Add the buffer size, buffer request/response code and buffer end tag sizes
  bufsize += 3 * sizeof(uint32_t);

  // buffer size must be 16 byte aligned
  bufsize += (bufsize % 16) ? 16 - (bufsize % 16) : 0;

  msg = get_mem(bufsize);
  if (!msg) return -1;

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

  // Send the message (add offset)
  mail = ((uint32_t)msg);
  mailbox_send(mail, PROPERTY_CHANNEL);
  mail = mailbox_read(PROPERTY_CHANNEL);

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