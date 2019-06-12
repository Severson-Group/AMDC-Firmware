#ifdef APP_DAC_TEST

#include "task_dac_test.h"
#include "../../sys/scheduler.h"
#include "../../sys/defines.h"
#include "../../drv/dac.h"
#include "../../drv/analog.h"
#include <math.h>

#define SINUSOID_FREQ_HZ	(10.0)
#define SINUSOID_FREQ_RADS	(SINUSOID_FREQ_HZ * PI2)
#define SINUSOID_DELTA		(SINUSOID_FREQ_RADS / TASK_DAC_TEST_UPDATES_PER_SEC)

static double theta = 0.0;

static task_control_block_t tcb;

static void _get_Iabc(double *Iabc)
{
	// Read from ADCs
	float Iabc_f[3];
	analog_getf(CC_PHASE_A_ADC, &Iabc_f[0]);
	analog_getf(CC_PHASE_B_ADC, &Iabc_f[1]);
	analog_getf(CC_PHASE_C_ADC, &Iabc_f[2]);

	// Convert ADC values to raw currents
	Iabc[0] = ((double) Iabc_f[0] * ADC_TO_AMPS_PHASE_A_GAIN) + ADC_TO_AMPS_PHASE_A_OFFSET;
	Iabc[1] = ((double) Iabc_f[1] * ADC_TO_AMPS_PHASE_B_GAIN) + ADC_TO_AMPS_PHASE_B_OFFSET;
	Iabc[2] = ((double) Iabc_f[2] * ADC_TO_AMPS_PHASE_C_GAIN) + ADC_TO_AMPS_PHASE_C_OFFSET;
}

void task_dac_test_init(void)
{
	printf("DACT:\tInitializing DAC test task...\n");

	scheduler_tcb_init(&tcb, task_dac_test_callback, NULL, "dactest", TASK_DAC_TEST_INTERVAL_USEC);
	scheduler_tcb_register(&tcb);
}

void task_dac_test_callback(void *arg)
{
	double Iabc[3];
	_get_Iabc(Iabc);

	dac_set_output(0, Iabc[0], -5, 5);
	dac_set_output(1, Iabc[1], -5, 5);
	dac_set_output(2, Iabc[2], -5, 5);




//	// Update theta
//	theta += SINUSOID_DELTA;
//	while (theta > PI2) {
//		theta -= PI2;
//	}
//
//	double value1 = cos(theta - PI23);
//	double value2 = cos(theta);
//	double value3 = cos(theta + PI23);
//
//	dac_set_output(0, value1, -1.5, 1.5);
//	dac_set_output(1, value2, -1.5, 1.5);
//	dac_set_output(2, value3, -1.5, 1.5);




}

#endif // APP_DAC_TEST
