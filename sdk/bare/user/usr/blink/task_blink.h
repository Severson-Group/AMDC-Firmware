#ifndef TASK_BLINK_H
#define TASK_BLINK_H

#include "sys/defines.h"

// Frequency that this task is called (in Hz)
//
// Must be less than or equal to scheduler updates per second
// This value is defined in sys/scheduler.h and defaults to 10kHz.
// Note that it can be overridden via usr/user_defines.h
#define TASK_BLINK_UPDATES_PER_SEC (5)

// Microseconds interval between when task is called
//
// This is what scheduler actually uses to run task,
// but is generated via define above
#define TASK_BLINK_INTERVAL_USEC (USEC_IN_SEC / TASK_BLINK_UPDATES_PER_SEC)

// Called in app init function to set up task (or via command)
void task_blink_init(void);

// Callback function which scheduler calls periodically
void task_blink_callback(void *arg);

#endif // TASK_BLINK_H
