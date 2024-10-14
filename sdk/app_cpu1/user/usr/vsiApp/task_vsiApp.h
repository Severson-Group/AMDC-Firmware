#ifndef TASK_VSIAPP_H
#define TASK_VSIAPP_H

#include "sys/scheduler.h"

#define TASK_VSI_UPDATES_PER_SEC (10000)
#define TASK_VSI_INTERVAL_USEC   (USEC_IN_SEC / TASK_VSI_UPDATES_PER_SEC)

int task_vsiApp_init(void);
int task_vsiApp_deinit(void);

void task_vsiApp_callback(void *arg);

int task_vsiApp_amplitude(double amplitude);
double task_vsiApp_get_amplitude();
int task_vsiApp_frequency(double frequency);
int task_vsiApp_RMS(double RMS);
int task_vsiApp_RMS_enable();
int task_vsiApp_RMS_disable();
double calculateRMS();

void task_vsiApp_stats_print(void);
void task_vsiApp_stats_reset(void);

#endif // TASK_VSIAPP_H
