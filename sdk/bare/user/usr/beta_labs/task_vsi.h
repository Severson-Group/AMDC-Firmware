#ifndef TASK_VSI_H
#define TASK_VSI_H

#include "sys/scheduler.h"
#include <stdint.h>

#define TASK_VSI_UPDATES_PER_SEC (10000)
#define TASK_VSI_INTERVAL_USEC   (USEC_IN_SEC / TASK_VSI_UPDATES_PER_SEC)

uint8_t task_vsi_is_inited(void);
void task_vsi_init(void);
void task_vsi_deinit(void);
void task_vsi_callback(void *arg);

void task_vsi_set_legs(int phAIdx, int phBIdx, int phCIdx);
void task_vsi_set(int vPercent, int freq, double ramptime);

#endif // TASK_VSI_H
