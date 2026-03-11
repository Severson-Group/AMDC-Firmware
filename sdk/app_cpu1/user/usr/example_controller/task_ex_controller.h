#ifndef TASK_EXAMPLE_CONTROLLER_H
#define TASK_EXAMPLE_CONTROLLER_H

#include "sys/scheduler.h"

#define TASK_EX_CONTROLLER_UPDATES_PER_SEC (1000000)
#define TASK_EX_CONTROLLER_INTERVAL_USEC   (USEC_IN_SEC / TASK_EX_CONTROLLER_UPDATES_PER_SEC)

int task_ex_controller_init(void);
int task_ex_controller_deinit(void);

void task_ex_controller_callback(void *arg);

#endif // TASK_EXAMPLE_CONTROLLER_H
