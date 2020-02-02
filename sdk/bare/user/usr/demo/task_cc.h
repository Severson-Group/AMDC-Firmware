#ifndef TASK_CC_H
#define TASK_CC_H

#include "sys/defines.h"
#include <stdint.h>

#define TASK_CC_UPDATES_PER_SEC (10000)
#define TASK_CC_INTERVAL_USEC (USEC_IN_SEC / TASK_CC_UPDATES_PER_SEC)

void task_cc_init(void);
void task_cc_callback(void *arg);

void task_cc_init(void);
void task_cc_deinit(void);
void task_cc_vdc_set(double vdc);
void task_cc_pwm(uint8_t phase, uint8_t pwm_chnl);
void task_cc_adc(uint8_t phase, uint8_t adc_chnl, double adc_gain, double adc_offset);
void task_cc_tune(double Rs, double Ld, double Lq, double bw);
void task_cc_set(double Id_star, double Iq_star, double omega_e);

#endif // TASK_CC_H
