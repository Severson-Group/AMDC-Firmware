#ifndef TASK_CC_H
#define TASK_CC_H

#include <stdint.h>
#include "../../drv/analog.h"
#include "../../sys/defines.h"
#include "../../sys/scheduler.h"

#define TASK_CC_UPDATES_PER_SEC		(10000)
#define TASK_CC_INTERVAL_USEC		(USEC_IN_SEC / TASK_CC_UPDATES_PER_SEC)

#define CC_BANDWIDTH				(200) // Hz

#define CC_BUS_VOLTAGE				(20.0) // V

// Current = GAIN * ADC_Voltage + Offset

#define ADC_TO_AMPS_PHASE_A_GAIN	(1.0100499)
#define ADC_TO_AMPS_PHASE_B_GAIN	(1.0088819)
#define ADC_TO_AMPS_PHASE_C_GAIN	(1.0056083)

#define ADC_TO_AMPS_PHASE_A_OFFSET	(-0.01765254)
#define ADC_TO_AMPS_PHASE_B_OFFSET	(-0.048415325)
#define ADC_TO_AMPS_PHASE_C_OFFSET	(-0.0477295)


#define CC_PHASE_A_ADC				(ANLG_CHNL1)
#define CC_PHASE_B_ADC				(ANLG_CHNL2)
#define CC_PHASE_C_ADC				(ANLG_CHNL3)

#define CC_PHASE_A_PWM_LEG_IDX		(0)
#define CC_PHASE_B_PWM_LEG_IDX		(1)
#define CC_PHASE_C_PWM_LEG_IDX		(2)

void task_cc_init(void);
void task_cc_deinit(void);
void task_cc_callback(void *arg);

uint8_t task_cc_is_inited(void);

void task_cc_set_Id_star(double my_Id_star);
void task_cc_set_Iq_star(double my_Iq_star);
void task_cc_set_dq_offset(int32_t offset);

#endif // TASK_CC_H
