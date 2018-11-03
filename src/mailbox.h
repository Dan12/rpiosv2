#include <stdint.h>
#include "peripheral.h"

#if !defined(_MAILBOX)
#define _MAILBOX

#define MAILBOX_BASE (PERIPHERAL_BASE + MAILBOX_OFFSET)
#define MAIL0_READ (0x00 + MAILBOX_BASE)
#define MAIL0_STATUS (0x18 + MAILBOX_BASE)
#define MAIL0_WRITE (0x20 + MAILBOX_BASE)
#define PROPERTY_CHANNEL 8

/* Bit 31 set in status register if the write mailbox is full */
#define MAILBOX_FULL 0x80000000

/* Bit 30 set in status register if the read mailbox is empty */
#define MAILBOX_EMPTY 0x40000000

uint32_t mailbox_read(uint8_t channel);
void mailbox_send(uint32_t msg, uint8_t channel);

/**
 * A property message can either be a request, or a response, and a response can
 * be successfull or an error
 */
typedef enum {
  REQUEST = 0x00000000,
  RESPONSE_SUCCESS = 0x80000000,
  RESPONSE_ERROR = 0x80000001
} buffer_req_res_code_t;

/*
 * A buffer that holds many property messages.
 * The last tag must be a 4 byte zero, and then padding to make the whole thing
 * 4 byte aligned
 */
typedef struct {
  uint32_t size;  // Size includes the size itself
  buffer_req_res_code_t req_res_code;
  uint32_t tags[1];  // A concatenated sequence of tags. will use overrun to
                     // make large enough
                     // padding will go after this
} property_message_buffer_t;

/**
 * A message is identified by a tag. These are some of the possible tags
 */
typedef enum {
  NULL_TAG = 0,
  GET_BOARD_MODEL = 0x00010001,
  GET_BOARD_REV = 0x00010002,
  GET_ARM_MEMORY = 0x00010005,
} property_tag_t;

/**
 * For each possible tag, we create a struct corresponding to the request value
 * buffer, and the response value buffer
 */
typedef struct {
  uint32_t bd;
} get_board_data_t;

typedef struct {
  uint32_t base_addr;
  uint32_t size;
} get_arm_memory_t;

/*
 * The value buffer can be any one of these types
 */
typedef union {
  get_board_data_t get_board_data;
  // get_arm_memory_t get_arm_memory;
} value_buffer_t;

/*
 * A message_buffer can contain any number of these
 */
typedef struct {
    property_tag_t proptag;
    value_buffer_t value_buffer;
} property_message_tag_t;

/**
 * given an array of tags, will send all of the tags given, and will populate that array with the responses.
 * the given array should end with a "null tag" with the proptag field set to 0.
 * returns 0 on success
 */
int send_messages(property_message_tag_t * tags);

#endif  // _MAILBOX
