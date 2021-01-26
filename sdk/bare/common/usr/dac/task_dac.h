#ifndef TASK_DAC_H
#define TASK_DAC_H

#include "sys/scheduler.h"

// Frequency that this task is called (in Hz)
//
// Must be less than or equal to scheduler updates per second
// This value is defined in sys/scheduler.h and defaults to 10kHz.
// Note that it can be overridden via usr/user_config.h
#define TASK_DAC_UPDATES_PER_SEC (10000)

// Microseconds interval between when task is called
//
// This is what scheduler actually uses to run task,
// but is generated via define above
#define TASK_DAC_INTERVAL_USEC (USEC_IN_SEC / TASK_DAC_UPDATES_PER_SEC)

// The minimum phase step between each time the task is called
// The default output of the any channel updated in the callback will
// always be 1Hz regardless of the task updates per second.
#define TASK_DAC_MIN_PHASE (2 * PI) / (TASK_DAC_UPDATES_PER_SEC)

// Called in app init function to set up task (or via command)
int task_dac_init(void);
int task_dac_deinit(void);
void task_dac_trigger(void);
void task_dac_frequency(double);
void task_dac_broadcast(double);
void task_dac_set_voltage(uint8_t, double);
void task_dac_set_reg(uint8_t, uint32_t);

// Callback function which scheduler calls periodically
void task_dac_callback(void *arg);

#endif // TASK_BLINK_H
