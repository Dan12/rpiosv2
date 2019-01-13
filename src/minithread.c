/*
 * (C) 2017, Cornell University
 * All rights reserved.
 */
/*
 * minithread.c:
 *      This file provides a few function headers for the procedures that
 *      you are required to implement for the minithread assignment.
 *
 *      EXCEPT WHERE NOTED YOUR IMPLEMENTATION MUST CONFORM TO THE
 *      NAMING AND TYPING OF THESE PROCEDURES.
 *
 */

#include "minithread.h"
#include "queue.h"
#include "interrupts.h"
#include "smemory.h"
#include "stdout.h"
#include "stdlib.h"
#include "utils.h"

typedef void* stack_pointer_t;
extern void hang (void);
extern void jump (uint32_t);

/*
 * A minithread should be defined either in this file or in a private
 * header file.  Minithreads have a stack pointer with to make procedure
 * calls, a stackbase which points to the bottom of the procedure
 * call stack, the ability to be enqueueed and dequeued, and any other state
 * that you feel they must have.
 */

// Status Enum
typedef enum {NASCENT, RUNNABLE, RUNNING, DEAD, STOPPED} minithread_statuses;

typedef struct {  // lower memory address
    uint32_t r0;
    uint32_t r1; 
    uint32_t r2; 
    uint32_t r3; 
    uint32_t r4; 
    uint32_t r5; 
    uint32_t r6; 
    uint32_t r7; 
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t cpsr; // techincally r12
    uint32_t cleanup;
    uint32_t lr;  // higher memory address
} proc_saved_state_t;

extern void context_switch(proc_saved_state_t** old_sp, proc_saved_state_t** new_sp);
extern void proc_switch(proc_saved_state_t** new_sp);
extern void proc_save(proc_saved_state_t** old_sp);

typedef struct minithread {
  // required for queue
  struct minithread *next;
  struct minithread *prev;
  // used to free the stack
  stack_pointer_t stackbase;
  // MiniThread ID
  int mt_id;
  // needed to restore the thread's state
  proc_saved_state_t* current_stack_pointer;
} minithread;

// globals
minithread *current_context_minithread;
// Constant Main/Initial stack does not have a TCB and have thread id 0
minithread *idle_thread;
int next_mtid = 0;

#define STACKSIZE 0x8000

void cleanup() {
  prntf("want to cleanup thread\r\n");
  minithread_yield();
}

minithread_t* minithread_create(proc_t proc, arg_t arg) {
  minithread_t *new_minithread = salloc(sizeof(*new_minithread));

  if (new_minithread == NULL) {
    prntf("Unable to create new minithread. Exiting\n");
    hang();
  }

  // semaphore_P(next_mtid_lock); // aquire lock
  new_minithread->mt_id = next_mtid++;
  // semaphore_V(next_mtid_lock); // release lock

  new_minithread->current_stack_pointer = NULL;

  new_minithread->stackbase = salloc(STACKSIZE);
  new_minithread->current_stack_pointer = (proc_saved_state_t*) (((uint8_t*) new_minithread->stackbase) + STACKSIZE - sizeof(proc_saved_state_t));
  mzero(new_minithread->current_stack_pointer, sizeof(proc_saved_state_t));

  if (new_minithread->stackbase == NULL) {
    prntf("Unable to create new minithread stack. Exiting\n");
    hang();
  }

  // first link register goes to the process
  new_minithread->current_stack_pointer->lr = (uint32_t) proc;
  // since the context switch code loads the old stack pointer into the lr, it will jump to cleanup after the process finishes
  new_minithread->current_stack_pointer->cleanup = (uint32_t) cleanup;
  new_minithread->current_stack_pointer->r0 = (uint32_t) arg;
  // default mode bits and IRQ mask disabled (irq's enabled)
  new_minithread->current_stack_pointer->cpsr = 0x15A;

  return new_minithread;
}

// initalize idle thread (this)
void initalize_idle_thread(){
  idle_thread = salloc(sizeof(minithread_t));

  if (idle_thread == NULL) {
    prntf("Unable to create idle thread. Exiting\n");
    hang();
  }

  idle_thread->prev = NULL;
  idle_thread->next = NULL;
  idle_thread->stackbase = NULL;
  idle_thread->mt_id = next_mtid++;
  idle_thread->current_stack_pointer = NULL;
}

void minithread_yield() {
  context_switch(&current_context_minithread->current_stack_pointer, &idle_thread->current_stack_pointer);
}

void minithread_system_initialize(proc_t mainproc, arg_t mainarg) {
  initalize_idle_thread();

  current_context_minithread = idle_thread;

  minithread_t* main = minithread_create(mainproc, mainarg);
  current_context_minithread = main;
  DISABLE_INTERRUPTS();
  context_switch(&idle_thread->current_stack_pointer, &main->current_stack_pointer);
  prntf("returned to idle thread\r\n");
  context_switch(&idle_thread->current_stack_pointer, &main->current_stack_pointer);
  prntf("returned to idle thread after cleanup called\r\n");
  // idle loop
  while(1) {}
}