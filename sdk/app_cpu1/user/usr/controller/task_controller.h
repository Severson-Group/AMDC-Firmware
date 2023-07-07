#ifndef TASK_CONTROLLER_H
#define TASK_CONTROLLER_H

#include "sys/scheduler.h"

// Frequency that this task is called (in Hz)
//
// Must be less than or equal to scheduler updates per second
// This value is defined in sys/scheduler.h and defaults to 10kHz.
// Note that it can be overridden via usr/user_config.h
#define TASK_CONTROLLER_UPDATES_PER_SEC (10000)

// Microseconds interval between when task is called
//
// This is what scheduler actually uses to run task,
// but is generated via define above
#define TASK_CONTROLLER_INTERVAL_USEC   (USEC_IN_SEC / TASK_CONTROLLER_UPDATES_PER_SEC)

// Called in app init function to set up task (or via command)
int task_controller_init(void);
// Print out the controller stats
void task_controller_stats_print(void);
// Reset controller stats
void task_controller_stats_reset(void);

int task_controller_deinit(void);

// Callback function which scheduler calls periodically
void task_controller_callback(void *arg);

int task_controller_set_frequency(double freq);
int task_controller_set_amplitude(double amplitude);

#endif // TASK_CONTROLLER_H
