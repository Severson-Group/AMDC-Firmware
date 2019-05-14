#ifndef TASK_CC_H
#define TASK_CC_H

#include "defines.h"
#include "scheduler.h"

#define TASK_CC_UPDATES_PER_SEC		(10000)
#define TASK_CC_INTERVAL_USEC		(USEC_IN_SEC / TASK_CC_UPDATES_PER_SEC)

#define CC_BANDWIDTH				(200) // Hz

#define CC_BUS_VOLTAGE				(30.0) // V

#define ADC_TO_AMPS					(0.5) // amps = (raw ADC voltage value) * (this value)

#define CC_PHASE_A_ADC				(ANLG_CHNL1)
#define CC_PHASE_B_ADC				(ANLG_CHNL2)
#define CC_PHASE_C_ADC				(ANLG_CHNL3)

#define CC_PHASE_A_PWM_LEG_IDX		(0)
#define CC_PHASE_B_PWM_LEG_IDX		(1)
#define CC_PHASE_C_PWM_LEG_IDX		(2)

void task_cc_init(void);
void task_cc_callback(void);

void task_cc_set_Id_star(double my_Id_star);
void task_cc_set_Iq_star(double my_Iq_star);

#endif // TASK_CC_H
