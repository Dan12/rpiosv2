/*
 * (C) 2017, Cornell University
 * All rights reserved.
 */
/*
 * Generic queue manipulation functions
 */
#ifndef __QUEUE_H__
#define __QUEUE_H__

/*
 * IMPORTANT queue invariant:
 * an element that wants to be added to a queue needs to have
 * it's first 2 elements be prev and next pointers.
 * Adding an element to more than 1 queue or more than once to the
 * same queue will produce undefined behavior
 */


/*
 * queue_t is a pointer to an internally maintained data structure.
 * Clients of this package do not need to know how queues are
 * represented.  They see and manipulate only queue_t's.
 */
typedef struct queue queue_t;

/*
 * Return an empty queue.  Returns NULL on error.
 */
queue_t* queue_new();

/*
 * Prepend a void* to a queue (both specified as parameters).
 * Returns 0 (success) or -1 (failure).
 */
int queue_prepend(queue_t*, void*);

/*
 * Appends a void* to a queue (both specified as parameters).  Return
 * 0 (success) or -1 (failure).
 */
int queue_append(queue_t*, void*);

/*
 * Dequeue and return the first void* from the queue.
 * Return 0 (success) and first item if queue is nonempty, or -1 (failure) and
 * NULL if queue is empty.
 */
int queue_dequeue(queue_t*, void**);

/*
 * queue_iterate(q, f, t) calls f(x,t) for each x in q.
 * q and f should be non-null.
 *
 * returns 0 (success) or -1 (failure)
 */
typedef void (*func_t)(void*, void*);
int queue_iterate(queue_t*, func_t, void*);

/*
 * Free the queue and return 0 (success) or -1 (failure).
 * Failure cases include NULL queue and non-empty queue.
 */
int queue_free (queue_t*);

/*
 * Return the number of items in the queue, or -1 if an error occurred
 */
int queue_length(const queue_t* queue);

/*
 * Delete the first instance of the specified item from the given queue.
 * Returns 0 if an element was deleted, or -1 otherwise.
 */
int queue_delete(queue_t* queue, void* item);

/**
 * Removes [item] from [queue] ASSUMING item is in the queue.
 * Returns 0 on success and -1 on failure
 */
int queue_delete_fast(queue_t *queue, void* item);

/*
 * queue find (q, f, t, item) is iterate over all x in q until 
 * f(x,t) returns true, then puts x item
 * Returns 0 on success and -1 on failure
 */
typedef int (*func_compare)(void*, void*);
int queue_find(queue_t *queue, func_compare f, void *arg, void **item);

#endif /*__QUEUE_H__*/