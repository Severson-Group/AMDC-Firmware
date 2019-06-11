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

	dac_set_output(0, value1, -1.5, 1.5);
	dac_set_output(1, value2, -1.5, 1.5);
	dac_set_output(2, value3, -1.5, 1.5);
}
