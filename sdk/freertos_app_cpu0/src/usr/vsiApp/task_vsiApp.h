#ifndef TASK_VSIAPP_H
#define TASK_VSIAPP_H

#define TASK_VSIAPP_UPDATES_PER_SEC (10000)
#define TASK_VSIAPP_INTERVAL_TICKS (pdMS_TO_TICKS(1000.0 / TASK_VSIAPP_UPDATES_PER_SEC))

int task_vsiApp_init(void);
int task_vsiApp_deinit(void);

void task_vsiApp_callback(void *arg);

int task_vsiApp_amplitude(double amplitude);
int task_vsiApp_frequency(double frequency);
int task_vsiApp_clear_inj(void);

void task_vsiApp_stats_print(void);
void task_vsiApp_stats_reset(void);

#endif // TASK_VSIAPP_H
