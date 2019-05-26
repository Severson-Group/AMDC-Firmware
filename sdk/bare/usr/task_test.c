#include "task_test.h"
#include "../sys/defines.h"
#include "../sys/scheduler.h"

#define MAX_VALUE	(100)
#define MIN_VALUE	(-100)

int32_t LOGGING_tri_i;
double LOGGING_tri_d;
static int dir = 1;

static task_control_block_t tcb;

void task_test_init(void)
{
	printf("TEST:\tInitializing test task...\n");

	scheduler_tcb_init(&tcb, task_test_callback, TASK_TEST_INTERVAL_USEC);
	scheduler_tcb_register(&tcb);

	LOGGING_tri_i = 0;
	LOGGING_tri_d = 0.0;
}

void task_test_callback(void)
{
	if (LOGGING_tri_i + dir > MAX_VALUE || LOGGING_tri_i + dir < MIN_VALUE) {
		dir *= -1;
	}

	LOGGING_tri_i += dir;

	LOGGING_tri_d = (double)LOGGING_tri_i / (double) MAX_VALUE;
}
