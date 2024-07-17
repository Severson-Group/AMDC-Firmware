#ifndef TASK_VSI_H
#define TASK_VSI_H

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"

#define TASK_VSI_UPDATES_PER_SEC (10000)
#define TASK_VSI_INTERVAL_TICKS  (pdMS_TO_TICKS(1000.0 / TASK_VSI_UPDATES_PER_SEC))

void task_vsi_init(void);

void task_vsi(void *arg);

#endif // TASK_VSI_H
