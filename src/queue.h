#if !defined(_QUEUE)
#define _QUEUE

typedef struct queue queue_t;

queue_t* queue_new();

int queue_prepend(queue_t*, void*);

int queue_append(queue_t*, void*);

int queue_dequeue(queue_t*, void**);

typedef void (*func_t)(void*, void*);
int queue_iterate(queue_t*, func_t, void*);

int queue_free (queue_t*);

int queue_length(const queue_t* queue);

int queue_delete(queue_t* queue, void* item);

int queue_delete_fast(queue_t *queue, void* item);

typedef int (*func_compare)(void*, void*);
int queue_find(queue_t *queue, func_compare f, void *arg, void **item);

#endif // _QUEUE