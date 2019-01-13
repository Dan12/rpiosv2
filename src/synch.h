#if !defined(_SYNCH)
#define _SYNCH

typedef struct semaphore semaphore_t;

semaphore_t* semaphore_create(int cnt);

void semaphore_P(semaphore_t *sem);

void semaphore_V(semaphore_t *sem);

#endif // _SYNCH
