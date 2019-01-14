#if !defined(_PROCESS)
#define _PROCESS

typedef int *arg_t;           /* function argument */
typedef int (*proc_t)(arg_t); /* generic function pointer */

typedef struct pcb process_t;

void process_yield();

process_t* process_fork(proc_t proc, arg_t arg);

process_t* create_process(proc_t proc, arg_t arg);

void process_start(process_t *t);

void process_stop();

void schedule_next_process();

process_t* get_current_process();

void process_system_initialize(proc_t mainproc, arg_t mainarg);

void proc_sleep_ms(int sleepms);

#endif // _PROCESS