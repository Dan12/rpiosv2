/*
 * (C) 2017, Cornell University
 * All rights reserved.
 */
/*
 * minithread.h:
 *  Definitions for minithreads.
 *
 *  Your assignment is to implement the functions defined in this file.
 *  You must use the names for types and procedures that are used here.
 *  Your procedures must take the exact arguments that are specified
 *  in the comments.  See minithread.c for prototypes.
 */

#ifndef __MINITHREAD_H__
#define __MINITHREAD_H__

typedef int *arg_t;           /* function argument */
typedef int (*proc_t)(arg_t); /* generic function pointer */


// note: dereferencing a stack_pointer_t should return the value of the stack
// at the value pointed to by the value in the stack_pointer_t (i.e. its a pointer)

/*
 * struct minithread:
 *  This is the key data structure for the thread management package.
 *  You must define the thread control block as a struct minithread.
 */

typedef struct minithread minithread_t;

/*
 * minithread_yield()
 *  Forces the caller to relinquish the processor and be put to the end of
 *  the ready queue.  Allows another thread to run.
 */
void minithread_yield();

/*
 * minithread_system_initialize(proc_t mainproc, arg_t mainarg)
 *  Initialize the system to run the first minithread at
 *  mainproc(mainarg).  This procedure should be called from your
 *  main program with the callback procedure and argument specified
 *  as arguments.
 */
void minithread_system_initialize(proc_t mainproc, arg_t mainarg);


#endif /*__MINITHREAD_H__*/