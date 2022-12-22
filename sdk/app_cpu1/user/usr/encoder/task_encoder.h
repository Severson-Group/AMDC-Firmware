#ifndef TASK_ENCODER_H
#define TASK_ENCODER_H

#include "sys/scheduler.h"

// Frequency that this task is called (in Hz)
//
// Must be less than or equal to scheduler updates per second
// This value is defined in sys/scheduler.h and defaults to 10kHz.
// Note that it can be overridden via usr/user_config.h
#define TASK_ENCODER_UPDATES_PER_SEC (5)

// Microseconds interval between when task is called
//
// This is what scheduler actually uses to run task,
// but is generated via define above
#define TASK_ENCODER_INTERVAL_USEC (USEC_IN_SEC / TASK_ENCODER_UPDATES_PER_SEC)

// Called in app init function to set up task (or via command)
int task_encoder_init(void);
int task_encoder_deinit(void);

// Callback function which scheduler calls periodically
void task_encoder_callback(void *arg);

// Print the statistics gathered by the scheduler
void task_encoder_stats_print(void);

// Reset the statistics gathered by the scheduler
void task_encoder_stats_reset(void);

#endif // TASK_ENCODER_H
