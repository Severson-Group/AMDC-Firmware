#ifndef TASK_CC_H
#define TASK_CC_H

#include "../src/defines.h"
#include "../src/scheduler.h"

#define TASK_CC_UPDATES_PER_SEC		(10000)
#define TASK_CC_INTERVAL_USEC		(USEC_IN_SEC / TASK_CC_UPDATES_PER_SEC)

#define CC_BANDWIDTH				(200) // Hz

#define CC_BUS_VOLTAGE				(5.0) // V

// Current = GAIN * ADC_Voltage + Offset

#define ADC_TO_AMPS_PHASE_A_GAIN	(0.50343077)
#define ADC_TO_AMPS_PHASE_B_GAIN	(0.50375471)
#define ADC_TO_AMPS_PHASE_C_GAIN	(0.50425522)

#define ADC_TO_AMPS_PHASE_A_OFFSET	(0.022818905)
#define ADC_TO_AMPS_PHASE_B_OFFSET	(0.025518978)
#define ADC_TO_AMPS_PHASE_C_OFFSET	(0.010428336)


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
