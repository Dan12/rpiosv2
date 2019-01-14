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

#include "process.h"
#include "queue.h"
#include "interrupts.h"
#include "smemory.h"
#include "stdout.h"
#include "stdlib.h"
#include "utils.h"
#include "synch.h"
#include "arm_timer.h"

typedef void* stack_pointer_t;

typedef struct {  // lower memory address
    uint32_t cpsr;
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
    uint32_t r12;
    uint32_t cleanup;
    uint32_t lr;  // higher memory address
} proc_saved_state_t;

extern void context_switch(proc_saved_state_t** old_sp, proc_saved_state_t** new_sp);

typedef struct pcb {
  // required for queue
  struct pcb *next;
  struct pcb *prev;
  // used to free the stack
  stack_pointer_t stackbase;
  // process ID
  int pid;
  // needed to restore the thread's state
  proc_saved_state_t* current_stack_pointer;
} pcb;

// globals
pcb *current_context_pcb;
pcb *idle_process;
int next_pid = 0;

// queues
queue_t* run_queue;
queue_t* dead_queue;
semaphore_t *dead_queue_semaphore;

#define STACKSIZE 0x8000

void cleanup() {
  SET_INTERRUPT_LEVEL(DISABLED);
  queue_append(dead_queue, get_current_process());

  semaphore_V(dead_queue_semaphore);

  schedule_next_process();
}

process_t* process_fork(proc_t proc, arg_t arg) {
  process_t* new_proc = create_process(proc, arg);
  process_start(new_proc);
  return new_proc;
}

void process_start(process_t *t) {
  interrupt_level_t prev_level = SET_INTERRUPT_LEVEL(DISABLED);
  queue_append(run_queue, t);
  SET_INTERRUPT_LEVEL(prev_level);
}

void process_stop() {
  // schedule without reappending to run queue
  schedule_next_process();
}

process_t* get_current_process() {
  return current_context_pcb;
}

process_t* create_process(proc_t proc, arg_t arg) {
  process_t *new_process = salloc(sizeof(process_t));

  interrupt_level_t prev_level = SET_INTERRUPT_LEVEL(DISABLED);
  new_process->pid = next_pid++;
  SET_INTERRUPT_LEVEL(prev_level);

  new_process->current_stack_pointer = NULL;

  new_process->stackbase = salloc(STACKSIZE);
  new_process->current_stack_pointer = (proc_saved_state_t*) (((uint8_t*) new_process->stackbase) + STACKSIZE - sizeof(proc_saved_state_t));
  mzero(new_process->current_stack_pointer, sizeof(proc_saved_state_t));

  // first link register goes to the process
  new_process->current_stack_pointer->lr = (uint32_t) proc;
  // since the context switch code loads the old stack pointer into the lr, it will jump to cleanup after the process finishes
  new_process->current_stack_pointer->cleanup = (uint32_t) cleanup;
  new_process->current_stack_pointer->r0 = (uint32_t) arg;
  // default mode bits and IRQ mask disabled (irq's enabled)
  new_process->current_stack_pointer->cpsr = 0x15A;

  return new_process;
}

void process_yield() {
  // invariant: the current process is not on the run queue
  process_start(get_current_process());
  schedule_next_process();
}

void schedule_next_process() {
  SET_INTERRUPT_LEVEL(DISABLED);

  // get the current process
  process_t* cur_proc = get_current_process();

  process_t* next_proc;

  if (queue_length(run_queue) > 0) {
    queue_dequeue(run_queue, (void **) &next_proc);
  } else {
    next_proc = idle_process;
  }

  current_context_pcb = next_proc;

  context_switch(&cur_proc->current_stack_pointer, &next_proc->current_stack_pointer);

  // enable interrupts after emerging from context switch
  SET_INTERRUPT_LEVEL(ENABLED);
}

int reaper_process(arg_t arg) {
  // never return
  while(1) {
    semaphore_P(dead_queue_semaphore);
    
    process_t *to_free;
    interrupt_level_t prev_level = SET_INTERRUPT_LEVEL(DISABLED);
    queue_dequeue(dead_queue, (void **) &to_free);
    SET_INTERRUPT_LEVEL(prev_level);

    sfree(to_free->stackbase);
    sfree(to_free);
  }
}

// initalize idle thread (this)
void initalize_idle_process(){
  idle_process = salloc(sizeof(process_t));

  idle_process->prev = NULL;
  idle_process->next = NULL;
  idle_process->stackbase = NULL;
  idle_process->pid = next_pid++;
  idle_process->current_stack_pointer = NULL;
}

void init_queues() {
  run_queue = queue_new();
  dead_queue = queue_new();
}

void init_semaphores(){
  dead_queue_semaphore = semaphore_create(0);
}

void clock_handler() {
  process_yield();
}

void process_system_initialize(proc_t mainproc, arg_t mainarg) {
  init_queues();
  init_semaphores();
  initalize_idle_process();

  current_context_pcb = idle_process;

  process_fork(reaper_process, NULL);

  process_fork(mainproc, mainarg);

  set_timer_handler(clock_handler);
  ENABLE_INTERRUPTS();

  // stop idle process
  process_stop();

  // idle loop
  while(1) {
    schedule_next_process();
  }
}