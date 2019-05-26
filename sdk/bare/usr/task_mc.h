#ifndef TASK_MC_H
#define TASK_MC_H

#include <stdint.h>
#include "../sys/defines.h"

#define TASK_MC_UPDATES_PER_SEC		(1000)
#define TASK_MC_INTERVAL_USEC		(USEC_IN_SEC / TASK_MC_UPDATES_PER_SEC)

#define MC_BANDWIDTH				(20) // Hz

void task_mc_init(void);
void task_mc_deinit(void);
void task_mc_callback(void *arg);

uint8_t task_mc_is_inited(void);

void task_mc_set_omega_star(double my_omega_star);

#endif // TASK_MC_H
