#ifndef TASK_DTC_H
#define TASK_DTC_H

#include "drv/analog.h"
#include "sys/scheduler.h"
#include <stdint.h>

#define TASK_DTC_UPDATES_PER_SEC (10000)
#define TASK_DTC_INTERVAL_USEC   (USEC_IN_SEC / TASK_DTC_UPDATES_PER_SEC)

#define DTC_BANDWIDTH (200.0) // Hz

// Current = GAIN * ADC_Voltage + Offset

#define ADC_TO_AMPS_PHASE_A_GAIN (1.0100499)
#define ADC_TO_AMPS_PHASE_B_GAIN (1.0088819)
#define ADC_TO_AMPS_PHASE_C_GAIN (1.0056083)

#define ADC_TO_AMPS_PHASE_A_OFFSET (-0.01765254)
#define ADC_TO_AMPS_PHASE_B_OFFSET (-0.048415325)
#define ADC_TO_AMPS_PHASE_C_OFFSET (-0.0477295)

#define CC_PHASE_A_ADC (ANLG_CHNL1)
#define CC_PHASE_B_ADC (ANLG_CHNL2)
#define CC_PHASE_C_ADC (ANLG_CHNL3)

#define CC_PHASE_A_PWM_LEG_IDX (0)
#define CC_PHASE_B_PWM_LEG_IDX (1)
#define CC_PHASE_C_PWM_LEG_IDX (2)

uint8_t task_dtc_is_inited(void);

void task_dtc_init(void);
void task_dtc_deinit(void);
void task_dtc_callback(void *arg);

void task_dtc_set_I_star(double A, double Hz);
void task_dtc_clear(void);

#endif // TASK_DTC_H
