#include "synch.h"
#include "queue.h"
#include "smemory.h"
#include "interrupts.h"
#include "process.h"

struct semaphore {
  int count;
  // queue of threads waiting on this semaphore
  queue_t *wait_queue;
};

semaphore_t* semaphore_create(int cnt) {
  semaphore_t *new_semaphore = salloc(sizeof(semaphore_t));

  new_semaphore->wait_queue = queue_new();
  new_semaphore->count = cnt;

  return new_semaphore;
}

void semaphore_P(semaphore_t *sem) {
  interrupt_level_t prev_level = SET_INTERRUPT_LEVEL(DISABLED);
  sem->count--;

  // block proc if semaphore is now negative
  if (sem->count < 0) {
    process_t *this_proc = get_current_process();

    // add this proc to the waiting queue
    queue_append(sem->wait_queue, this_proc);

    // stop this proc's execution
    process_stop();
  }
  SET_INTERRUPT_LEVEL(prev_level);
}

void semaphore_V(semaphore_t *sem) {
  interrupt_level_t prev_level = SET_INTERRUPT_LEVEL(DISABLED);
  sem->count++;

  // if there are thread's blocked, start up the first one
  if (queue_length(sem->wait_queue) > 0) {
    process_t *to_start;
    queue_dequeue(sem->wait_queue, (void **) &to_start);
    process_start(to_start);
  }
  SET_INTERRUPT_LEVEL(prev_level);
}