#ifndef TASK_CAN_H
#define TASK_CAN_H

#include "sys/scheduler.h"

// Frequency that this task is called (in Hz)
//
// Must be less than or equal to scheduler updates per second
// This value is defined in sys/scheduler.h and defaults to 10kHz.
// Note that it can be overridden via usr/user_config.h
#define TASK_CAN_UPDATES_PER_SEC (10000)

// Microseconds interval between when task is called
//
// This is what scheduler actually uses to run task,
// but is generated via define above
#define TASK_CAN_INTERVAL_USEC (USEC_IN_SEC / TASK_CAN_UPDATES_PER_SEC)

int task_can_init(void);

int task_can_deinit(void);

int task_can_loopback_test(void);

int task_can_sendmessage(uint8_t *, int);

int task_can_print();

int task_can_print_mode();

int task_can_print_peripheral();

int task_can_setmode(uint32_t);

int task_can_setbaud(int);

int task_can_set_btr(int, int, int);

int task_can_set_peripheral(int);

// Callback function which scheduler calls periodically
void task_can_callback(void *arg);

#endif // TASK_CAN_H
