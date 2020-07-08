#ifndef TASK_CC_H
#define TASK_CC_H

#include "drv/analog.h"
#include "sys/defines.h"
#include "sys/scheduler.h"
#include <stdint.h>

#define TASK_CC_UPDATES_PER_SEC (20000)
#define TASK_CC_INTERVAL_USEC   (USEC_IN_SEC / TASK_CC_UPDATES_PER_SEC)

// Current = GAIN * ADC_Voltage + Offset

#define ADC_TO_AMPS_PHASE_A_GAIN (1.0056083)
#define ADC_TO_AMPS_PHASE_B_GAIN (1.0088819)
#define ADC_TO_AMPS_PHASE_C_GAIN (1.0100499)

#define ADC_TO_AMPS_PHASE_A_OFFSET (-0.0477295)
#define ADC_TO_AMPS_PHASE_B_OFFSET (-0.048415325)
#define ADC_TO_AMPS_PHASE_C_OFFSET (-0.01765254)

#define CC_PHASE_A_ADC (ANLG_CHNL1)
#define CC_PHASE_B_ADC (ANLG_CHNL2)
#define CC_PHASE_C_ADC (ANLG_CHNL3)

#define CC_PHASE_A_PWM_LEG_IDX (0)
#define CC_PHASE_B_PWM_LEG_IDX (1)
#define CC_PHASE_C_PWM_LEG_IDX (2)

void task_cc_init(void);
void task_cc_deinit(void);
void task_cc_callback(void *arg);

uint8_t task_cc_is_inited(void);

double task_cc_get_theta_e_enc(void);

void task_cc_clear(void);
void task_cc_set_dq_offset(int32_t offset);
void task_cc_set_bw(double bw);

void task_cc_set_Iq_star(double value);
void task_cc_set_Id_star(double value);

void task_cc_set_theta_e_src(uint8_t use_encoder);
void task_cc_set_omega_e_src(uint8_t use_encoder);

#endif // TASK_CC_H
