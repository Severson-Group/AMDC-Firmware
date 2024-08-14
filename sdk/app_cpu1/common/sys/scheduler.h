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

// Timing Threshold - 60ns
//   When the scheduler checks to see if a task should be scheduled in scheduler_run(),
//   the task's measured loop time is subtracted from the target loop time. If the former
//   is larger than the latter, the result will be negative and obviously the task should be
//   scheduled. However, it is possible with floating point numbers that the measured loop
//   time will be *just less* than the target loop time, in which case the subtraction
//   result will be a very small positive number. We still want the task to be run in this
//   case, so instead of checking if the subtraction result is less than 0, we check that
//   it is less than a very small positive variance value. Typically, this variance of the
//   measured interval from the expected interval is no more than 20ns, but we'll use a
//   tolerance of three times this for safety :)
//   NOTE: this is with the default timing settings of 100 kHz PWM Carrier and
//   a Timing Manager ratio of 10 events. For abnormal timing settings, the
//   magnitude of the tolerance can be overridden in user_config.h
#ifndef USER_CONFIG_SCHEDULER_INTERVAL_TOLERANCE_USEC
#define SCHEDULER_INTERVAL_TOLERANCE_USEC (0.06)
#else
#define SCHEDULER_INTERVAL_TOLERANCE_USEC (USER_CONFIG_SCHEDULER_INTERVAL_TOLERANCE_USEC)
#endif

// Callback into application when task is run:
typedef void (*task_callback_t)(void *);

// TCB of each task
typedef struct task_control_block_t {
    int id;
    const char *name;
    bool is_registered;
    task_callback_t callback;
    void *callback_arg;
    double interval_usec;
    double last_run_usec;

    task_stats_t stats;

    struct task_control_block_t *next;
} task_control_block_t;

void scheduler_tick(void);
void scheduler_run(void);

void scheduler_tcb_init(
    task_control_block_t *tcb, task_callback_t callback, void *callback_arg, const char *name, double interval_usec);
int scheduler_tcb_register(task_control_block_t *tcb);
int scheduler_tcb_register_high_priority(task_control_block_t *tcb);
int scheduler_tcb_unregister(task_control_block_t *tcb);
bool scheduler_tcb_is_registered(task_control_block_t *tcb);

double scheduler_get_elapsed_usec(void);

#endif // SCHEDULER_H
