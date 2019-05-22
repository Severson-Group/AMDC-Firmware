#ifndef TASK_MC_H
#define TASK_MC_H

#include "../src/defines.h"

#define TASK_MC_UPDATES_PER_SEC		(1000)
#define TASK_MC_INTERVAL_USEC		(USEC_IN_SEC / TASK_MC_UPDATES_PER_SEC)

#define MC_BANDWIDTH				(50) // Hz

void task_mc_init(void);
void task_mc_callback(void);

void task_mc_set_omega_star(double my_omega_star);

#endif // TASK_MC_H
