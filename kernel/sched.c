#include "sched.h"
#include "kprint.h"
#include <stdint.h>

extern void context_switch(ktask_t *prev, ktask_t *next);
extern void kprint(const char *s);
extern uint32_t tick_count; /* from timer.c */

ktask_t *current_task = 0;
ktask_t *task_queue = 0;
static int next_pid = 1;

/* Append to circular queue */
static void enqueue_task(ktask_t *t) {
    if (!task_queue) {
        task_queue = t;
        t->next = t;
    } else {
        t->next = task_queue->next;
        task_queue->next = t;
    }
}

/* Create kernel thread that runs fn() */
ktask_t *create_kthread(void (*fn)(void), const char *name) {
    ktask_t *t = (ktask_t*) (/* use kernel stack memory allocated inside struct */ 0);
    /* allocate from static memory: use malloc-like static array handled here */
    /* For simplicity, allocate from a small bump allocator stored in static array */
    static uint8_t pool[16 * 4096];
    static int pool_used = 0;
    if (pool_used + sizeof(ktask_t) > (int)sizeof(pool)) {
        kprint("Out of task memory\n");
        return 0;
    }
    t = (ktask_t*)(pool + pool_used);
    pool_used += sizeof(ktask_t);

    t->pid = next_pid++;
    t->next = 0;

    /* Setup initial stack for the task so that popad/popad + ret will transfer control
       Stack layout expected by popad + ret:
       [EDI][ESI][EBP][ESP][EBX][EDX][ECX][EAX][return_eip]
       We'll fill 8 zeros for registers and set return_eip = fn
    */
    uint32_t *stack_top = (uint32_t*)( (uint8_t*)t + sizeof(ktask_t) );
    /* place stack_base at end of t (stack grows down) */
    uint32_t *sp = (uint32_t*)(((uint8_t*)t) + sizeof(ktask_t));
    /* reserve 9 uint32_t slots */
    sp -= 9;
    sp[0] = 0; /* EDI */
    sp[1] = 0; /* ESI */
    sp[2] = 0; /* EBP */
    sp[3] = 0; /* ESP (ignored) */
    sp[4] = 0; /* EBX */
    sp[5] = 0; /* EDX */
    sp[6] = 0; /* ECX */
    sp[7] = 0; /* EAX */
    sp[8] = (uint32_t)fn; /* return_eip -> entry point for thread */

    t->esp = (uint32_t)sp;

    enqueue_task(t);
    return t;
}

/* Simple round-robin schedule */
void scheduler_init(void) {
    task_queue = 0;
    current_task = 0;
}

/* Called from timer interrupt */
void scheduler_tick(void) {
    if (!current_task) {
        if (task_queue) current_task = task_queue;
        return;
    }
    /* choose next */
    ktask_t *next = current_task->next;
    if (next == current_task) return; /* only one task */
    ktask_t *prev = current_task;
    current_task = next;
    /* context switch */
    context_switch(prev, next);
}

/* voluntary yield */
void yield(void) {
    if (!current_task || !current_task->next) return;
    ktask_t *prev = current_task;
    ktask_t *next = current_task->next;
    current_task = next;
    context_switch(prev, next);
}

/* --- Demo kernel thread functions --- */
void thread_func_a(void) {
    while (1) {
        kprint("Thread A running\n");
        /* busy wait a bit for visible interleaving */
        for (volatile int i=0;i<100000;i++);
        yield();
    }
}

void thread_func_b(void) {
    while (1) {
        kprint("Thread B running\n");
        for (volatile int i=0;i<100000;i++);
        yield();
    }
}

/* create two demo threads and set current_task to head */
void start_demo_threads(void) {
    create_kthread(thread_func_a, "A");
    create_kthread(thread_func_b, "B");
    if (task_queue) {
        current_task = task_queue->next; /* first created */
    }
}

