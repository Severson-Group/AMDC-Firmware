#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdbool.h>
#include <stdint.h>

#include "sys/defines.h"
#include "sys/task_stats.h"
#include "usr/user_config.h"

// Utility defines for time conversions
#define USEC_IN_SEC       (1000000)
#define SEC_TO_USEC(sec)  (sec * USEC_IN_SEC)
#define USEC_TO_SEC(usec) (usec / USEC_IN_SEC)

// SysTick
//
// The basic time quantum is defined to be SYS_TICK_FREQ.
//
// As an example, say we set this to 10kHz. At 666.6 MHz DSP clock,
// we will have 66.6k cycles per time slice, meaning all tasks
// combined have to consume <= 66.6k cycles
#ifndef SYS_TICK_FREQ
#define SYS_TICK_FREQ (10000) // Hz
#endif
#define SYS_TICK_USEC (SEC_TO_USEC(1) / SYS_TICK_FREQ)

// Callback into application when task is run:
typedef void (*task_callback_t)(void *);

// TCB of each task
typedef struct task_control_block_t {
    int id;
    const char *name;
    bool is_registered;
    task_callback_t callback;
    void *callback_arg;
    uint32_t interval_usec;
    uint32_t last_run_usec;

    task_stats_t stats;

    struct task_control_block_t *next;
} task_control_block_t;

void scheduler_init(void);
void scheduler_run(void);

void scheduler_tcb_init(
    task_control_block_t *tcb, task_callback_t callback, void *callback_arg, const char *name, uint32_t interval_usec);
int scheduler_tcb_register(task_control_block_t *tcb);
int scheduler_tcb_unregister(task_control_block_t *tcb);
bool scheduler_tcb_is_registered(task_control_block_t *tcb);

uint32_t scheduler_get_elapsed_usec(void);

#endif // SCHEDULER_H
