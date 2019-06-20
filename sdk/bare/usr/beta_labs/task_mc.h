#ifndef TASK_MC_H
#define TASK_MC_H

#include "../../sys/defines.h"
#include <stdint.h>

#define TASK_MC_UPDATES_PER_SEC			(10000)
#define TASK_MC_INTERVAL_USEC			(USEC_IN_SEC / TASK_MC_UPDATES_PER_SEC)

uint8_t task_mc_is_inited(void);
void task_mc_init(void);
void task_mc_deinit(void);
void task_mc_callback(void *arg);

#endif // TASK_MC_H