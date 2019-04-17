#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include "defines.h"


/**
 * SysTick
 *
 * The basic time quantum is defined to be SYS_TICK_FREQ (10kHz)
 * At 666.6 MHz DSP clock, we have 66.6k cycles per time slice,
 * meaning all tasks combined have to consume <= 66.6k cycles
 */
#define SYS_TICK_FREQ	(10000) // Hz
#define SYS_TICK_USEC	(SEC_TO_USEC(1) / SYS_TICK_FREQ)



/**
 * Tasks
 */
#define MAX_NUM_TASKS (16)
typedef void (*task_callback_t)(void);

void scheduler_init(void);
void scheduler_run(void);
void scheduler_register_task(task_callback_t callback, uint32_t interval_usec);


#endif // SCHEDULER_H
