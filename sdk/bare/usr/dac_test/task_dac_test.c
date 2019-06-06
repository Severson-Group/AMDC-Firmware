#include "task_dac_test.h"
#include "../../sys/scheduler.h"
#include "../../sys/defines.h"
#include "../../drv/dac.h"
#include <math.h>

#define SINUSOID_FREQ_HZ	(10.0)
#define SINUSOID_FREQ_RADS	(SINUSOID_FREQ_HZ * PI2)
#define SINUSOID_DELTA		(SINUSOID_FREQ_RADS / TASK_DAC_TEST_UPDATES_PER_SEC)

static double theta = 0.0;

static task_control_block_t tcb;

void task_dac_test_init(void)
{
	printf("DACT:\tInitializing DAC test task...\n");

	scheduler_tcb_init(&tcb, task_dac_test_callback, NULL, "dactest", TASK_DAC_TEST_INTERVAL_USEC);
	scheduler_tcb_register(&tcb);
}

void task_dac_test_callback(void *arg)
{
	// Update theta
	theta += SINUSOID_DELTA;
	while (theta > PI2) {
		theta -= PI2;
	}

	double value1 = cos(theta - PI23);
	double value2 = cos(theta);
	double value3 = cos(theta + PI23);

	uint16_t dac_value1 = DAC_HALF_FULL_SCALE + DAC_HALF_FULL_SCALE * value1;
	uint16_t dac_value2 = DAC_HALF_FULL_SCALE + DAC_HALF_FULL_SCALE * value2;
	uint16_t dac_value3 = DAC_HALF_FULL_SCALE + DAC_HALF_FULL_SCALE * value3;

	dac_set_voltage_raw(0, dac_value1);
	dac_set_voltage_raw(1, dac_value2);
	dac_set_voltage_raw(2, dac_value3);
}
