#ifndef TASK_CONTROLLER_H
#define TASK_CONTROLLER_H

#include "sys/scheduler.h"

#define TASK_CONTROLLER_UPDATES_PER_SEC (25000)
#define TASK_CONTROLLER_INTERVAL_USEC   (USEC_IN_SEC / TASK_CONTROLLER_UPDATES_PER_SEC)

int task_controller_init(void);
int task_controller_deinit(void);

void task_controller_callback(void *arg);

int task_controller_set_frequency(double freq);
int task_controller_set_amplitude(double amplitude);

void task_controller_get_raw(void);
int task_controller_get_data(int channel);

// Read sensor card data
int task_controller_get_fbc_data(double *out_current1, double *out_voltage1,
						  double *out_current2, double *out_voltage2,
						  double *out_current3, double *out_voltage3);

#endif // TASK_CONTROLLER_H
