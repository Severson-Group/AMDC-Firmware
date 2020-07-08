#ifndef TASK_CONTROL_H
#define TASK_CONTROL_H

#include "sys/defines.h"
#include <stdint.h>
#include <stdlib.h>

#define TASK_CONTROL_UPDATES_PER_SEC (10000)
#define TASK_CONTROL_INTERVAL_USEC   (USEC_IN_SEC / TASK_CONTROL_UPDATES_PER_SEC)

void task_control_callback(void *arg);

void task_control_init(int cc_idx);
void task_control_deinit(int cc_idx);

void task_control_vdc_set(int ctrl_idx, double vdc);
void task_control_pwm(int ctrl_idx, uint8_t phase, uint8_t pwm_chnl);
void task_control_adc(int ctrl_idx, uint8_t phase, uint8_t adc_chnl, double adc_gain, double adc_offset);
void task_control_tune(int ctrl_idx, double Rs_hat, double Ld_hat, double Lq_hat, double bandwidth_hz);
void task_control_set(int ctrl_idx, double Id_star, double Iq_star, double omega_e);

#endif // TASK_CONTROL_H
