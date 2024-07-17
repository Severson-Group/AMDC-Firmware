#ifndef TASK_BLINK_H
#define TASK_BLINK_H

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"

// Frequency that this task is called (in Hz)
//
// Must be less than or equal to scheduler updates per second
// This value is defined in sys/scheduler.h and defaults to 10kHz.
// Note that it can be overridden via usr/user_config.h
#define TASK_BLINK_UPDATES_PER_SEC (5)

// Microseconds interval between when task is called
//
// This is what scheduler actually uses to run task,
// but is generated via define above
#define TASK_BLINK_INTERVAL_TICKS (pdMS_TO_TICKS(1000.0 / TASK_BLINK_UPDATES_PER_SEC))

// Called in app init function to set up task (or via command)
int task_blink_init(void);
int task_blink_deinit(void);

// Callback function which scheduler calls periodically
void task_blink(void *arg);

// Print the statistics gathered by the scheduler
void task_blink_stats_print(void);

// Reset the statistics gathered by the scheduler
void task_blink_stats_reset(void);

#endif // TASK_BLINK_H
