#ifndef TASK_MC_H
#define TASK_MC_H

#include "sys/scheduler.h"
#include <stdint.h>

#define TASK_MC_UPDATES_PER_SEC (4000)
#define TASK_MC_INTERVAL_USEC   (USEC_IN_SEC / TASK_MC_UPDATES_PER_SEC)

uint8_t task_mc_is_inited(void);
void task_mc_init(void);
void task_mc_deinit(void);
void task_mc_callback(void *arg);

void task_mc_set_omega_m_star(double omega_m);
void task_mc_set_cff_enabled(uint32_t enabled);
void task_mc_set_omega_m_src(uint8_t use_encoder);

#endif // TASK_MC_H
