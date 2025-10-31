#ifndef SCHED_H
#define SCHED_H

#include <stdint.h>

#define KSTACK_SIZE 4096

typedef struct ktask {
    uint32_t esp;               // saved stack pointer
    int pid;
    struct ktask *next;
    uint8_t stack[KSTACK_SIZE];
} ktask_t;

void scheduler_init(void);
ktask_t *create_kthread(void (*fn)(void), const char *name);
void schedule(void);
void scheduler_tick(void);
void yield(void);

extern ktask_t *current_task;

#endif
