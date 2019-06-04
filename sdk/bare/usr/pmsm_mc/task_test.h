#ifndef TASK_TEST_H
#define TASK_TEST_H

#include "../../sys/defines.h"

#define TASK_TEST_UPDATES_PER_SEC		(120)
#define TASK_TEST_INTERVAL_USEC			(USEC_IN_SEC / TASK_TEST_UPDATES_PER_SEC)

void task_test_init(void);
void task_test_callback(void *arg);

#endif // TASK_TEST_H
