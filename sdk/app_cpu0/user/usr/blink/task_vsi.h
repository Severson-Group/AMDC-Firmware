#ifndef TASK_VSI_H
#define TASK_VSI_H

#include "sys/scheduler.h"

#define TASK_VSI_UPDATES_PER_SEC (10000)
#define TASK_VSI_INTERVAL_USEC   (USEC_IN_SEC / TASK_VSI_UPDATES_PER_SEC)

void task_vsi_init(void);

void task_vsi_callback(void *arg);

#endif // TASK_VSI_H
