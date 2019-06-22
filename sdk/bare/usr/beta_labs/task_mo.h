#ifndef TASK_MO_H
#define TASK_MO_H

#include "../../sys/defines.h"

#define TASK_MO_UPDATES_PER_SEC			(10000)
#define TASK_MO_INTERVAL_USEC			(USEC_IN_SEC / TASK_MO_UPDATES_PER_SEC)

void task_mo_init(void);
void task_mo_callback(void *arg);

void task_mo_get_omega_e(double *omega_e);
void task_mo_get_omega_m(double *omega_m);

#endif // TASK_MO_H
