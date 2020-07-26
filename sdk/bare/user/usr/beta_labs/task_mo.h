#ifndef TASK_MO_H
#define TASK_MO_H

#include "sys/scheduler.h"

#define TASK_MO_UPDATES_PER_SEC (10000)
#define TASK_MO_INTERVAL_USEC   (USEC_IN_SEC / TASK_MO_UPDATES_PER_SEC)

void task_mo_init(void);
void task_mo_callback(void *arg);

double task_mo_get_omega_e(void);
double task_mo_get_omega_m(void);

#endif // TASK_MO_H
