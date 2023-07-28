#ifndef TASK_CONTROLLER_H
#define TASK_CONTROLLER_H

#include "sys/scheduler.h"

#define TASK_CONTROLLER_UPDATES_PER_SEC (10000)
#define TASK_CONTROLLER_INTERVAL_USEC   (USEC_IN_SEC / TASK_CONTROLLER_UPDATES_PER_SEC)

int task_controller_init(void);
int task_controller_deinit(void);

void task_controller_callback(void *arg);

int task_controller_set_frequency(double freq);
int task_controller_set_amplitude(double amplitude);

void task_controller_stats_print(void);
void task_controller_stats_reset(void);

#endif // TASK_CONTROLLER_H
