#ifndef TASK_DAC_TEST_H
#define TASK_DAC_TEST_H

#include "../../sys/defines.h"

#define TASK_DAC_TEST_UPDATES_PER_SEC		(10000)
#define TASK_DAC_TEST_INTERVAL_USEC			(USEC_IN_SEC / TASK_DAC_TEST_UPDATES_PER_SEC)


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

void task_dac_test_init(void);
void task_dac_test_callback(void *arg);

#endif // TASK_DAC_TEST_H
