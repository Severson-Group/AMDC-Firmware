#ifndef TASK_CC_H
#define TASK_CC_H

#include "sys/scheduler.h"
#include <stdint.h>
#include <stdlib.h>

#define TASK_CC_UPDATES_PER_SEC (10000)
#define TASK_CC_INTERVAL_USEC   (USEC_IN_SEC / TASK_CC_UPDATES_PER_SEC)

void task_cc_callback(void *arg);

void task_cc_init(int cc_idx);
void task_cc_deinit(int cc_idx);

void task_cc_vdc_set(int cc_idx, double vdc);
void task_cc_pwm(int cc_idx, uint8_t phase, uint8_t pwm_chnl);
void task_cc_adc(int cc_idx, uint8_t phase, uint8_t adc_chnl, double adc_gain, double adc_offset);
void task_cc_tune(int cc_idx, double Rs, double Ld, double Lq, double bw);
void task_cc_setmode(int cc_idx, char *argMode);
void task_cc_set_currents(int cc_idx, double Id_star, double Iq_star);
void task_cc_set_omega(int cc_idx, double omega_e);
void task_cc_set_theta(int cc_idx, double theta_e);

#endif // TASK_CC_H
