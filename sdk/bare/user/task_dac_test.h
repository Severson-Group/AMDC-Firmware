#ifndef TASK_DAC_TEST_H
#define TASK_DAC_TEST_H

#include "../src/defines.h"

#define TASK_DAC_TEST_UPDATES_PER_SEC		(10000)
#define TASK_DAC_TEST_INTERVAL_USEC			(USEC_IN_SEC / TASK_DAC_TEST_UPDATES_PER_SEC)

void task_dac_test_init(void);
void task_dac_test_callback(void);

#endif // TASK_DAC_TEST_H
