/*
 * (C) 2017, Cornell University
 * All rights reserved.
 */
/*
 * Generic queue implementation.
 *
 */
#include "queue.h"
#include "smemory.h"

/*
 * queue_node is a node in a double linked lists
 */
typedef struct queue_node_header {
  struct queue_node_header *next;
  struct queue_node_header *prev;
} queue_node_header;

typedef struct queue {
  queue_node_header *sentinal;
  int length;
} queue;

queue_t* queue_new() {
  queue_t *new_queue = salloc(sizeof(queue_t));
  if (new_queue == NULL) return NULL;

  // sentinal node
  queue_node_header *sentinal = salloc(sizeof(queue_node_header));
  if (sentinal == NULL) { // salloc failed
    // free memory, wont be used
    sfree(new_queue);
    return NULL;
  }

  sentinal->next = sentinal;
  sentinal->prev = sentinal;

  new_queue->length = 0;
  new_queue->sentinal = sentinal;

  return new_queue;
}

int queue_prepend(queue_t *queue, void* item) {
  if (queue == NULL) return -1;

  if (item == NULL) return -1;
  queue_node_header *new_node = (queue_node_header *) item;

  new_node->next = queue->sentinal->next;
  new_node->prev = queue->sentinal;
  new_node->next->prev = new_node;
  new_node->prev->next = new_node;

  queue->length++;

  return 0;
}

int queue_append(queue_t *queue, void* item) {
  if (queue == NULL) return -1;

  if (item == NULL) return -1;
  queue_node_header *new_node = (queue_node_header *) item;

  new_node->next = queue->sentinal;
  new_node->prev = queue->sentinal->prev;
  new_node->next->prev = new_node;
  new_node->prev->next = new_node;

  queue->length++;

  return 0;
}

int queue_dequeue(queue_t *queue, void** item) {
  if (queue == NULL || queue->length == 0) {
    if (item != NULL) {
      *item = NULL;
    }
    return -1;
  }

  queue_node_header *to_dequeue = queue->sentinal->next;

  to_dequeue->prev->next = to_dequeue->next;
  to_dequeue->next->prev = to_dequeue->prev;
  *item = to_dequeue;

  queue->length--;

  return 0;
}

int queue_iterate(queue_t *queue, func_t f, void* arg) {
  if (queue == NULL) return -1;

  queue_node_header *cur = queue->sentinal->next;
  while (cur != queue->sentinal) {
    f(cur, arg);
    cur = cur->next;
  }

  return 0;
}

int queue_free (queue_t *queue) {
  if (queue == NULL || queue->length != 0) return -1;

  // free sentinal and queue
  sfree(queue->sentinal);
  sfree(queue);

  return 0;
}

int queue_length(const queue_t *queue) {
  if (queue == NULL) return -1;
  return queue->length;
}

int queue_delete(queue_t *queue, void* item) {
  if (queue == NULL) return -1;

  queue_node_header *cur = queue->sentinal->next;
  while (cur != queue->sentinal) {
    if (cur == item) {
      cur->next->prev = cur->prev;
      cur->prev->next = cur->next;
      queue->length--;
      return 0;
    }
    cur = cur->next;
  }
  return -1;
}

int queue_delete_fast(queue_t *queue, void* item) {
  if (queue == NULL) return -1;

  queue_node_header *node = (queue_node_header *) item;

  node->next->prev = node->prev;
  node->prev->next = node->next;
  queue->length--;
  return 0;
}

int queue_find(queue_t *queue, func_compare f, void *arg, void **item) {
  if (queue == NULL || item == NULL) return -1;
  
  queue_node_header *cur = queue->sentinal->next;
  while (cur != queue->sentinal) {
    if (f(cur, arg)) {
      *item = cur;
      return 0;
    }
    cur = cur->next;
  }
  
  *item = NULL;
  return -1;
}