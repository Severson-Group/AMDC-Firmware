#include "task_dac_test.h"
#include "../src/scheduler.h"
#include "../src/defines.h"
#include "../bsp/bsp.h"
#include <math.h>

#define SINUSOID_FREQ_HZ	(100.0)
#define SINUSOID_FREQ_RADS	(SINUSOID_FREQ_HZ * PI2)
#define SINUSOID_DELTA		(SINUSOID_FREQ_RADS / TASK_DAC_TEST_UPDATES_PER_SEC)

static double theta = 0.0;

static task_control_block_t tcb;

void task_dac_test_init(void)
{
	printf("DACT:\tInitializing DAC test task...\n");

	scheduler_tcb_init(&tcb, task_dac_test_callback, TASK_DAC_TEST_INTERVAL_USEC);
	scheduler_tcb_register(&tcb);
}

void task_dac_test_callback(void)
{
	// Update theta
	theta += SINUSOID_DELTA;
	while (theta > PI2) {
		theta -= PI2;
	}

	double value = cos(theta);
	uint16_t dac_value = (DAC_FULL_SCALE/2) + (DAC_FULL_SCALE/2) * value;

	// Assign to all DAC channels
	for (int i = 0; i < 8; i++) {
		dac_set_voltage(i, dac_value);
	}
}
