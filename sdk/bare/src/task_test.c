#include "task_test.h"
#include "scheduler.h"
#include "defines.h"

#define MAX_VALUE	(100)
#define MIN_VALUE	(-100)

int32_t NPETERSEN_READ_ME;
static int dir = 1;

static task_control_block_t tcb;

void task_test_init(void)
{
	printf("TEST:\tInitializing test task...\n");

	scheduler_tcb_init(&tcb, task_test_callback, TASK_TEST_INTERVAL_USEC);
	scheduler_tcb_register(&tcb);

	NPETERSEN_READ_ME = 0;
}

void task_test_callback(void)
{
	if (NPETERSEN_READ_ME + dir > MAX_VALUE || NPETERSEN_READ_ME - dir < MIN_VALUE) {
		dir *= -1;
	}

	NPETERSEN_READ_ME += dir;
}
