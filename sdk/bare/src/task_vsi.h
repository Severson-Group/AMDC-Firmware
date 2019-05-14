#ifndef TEST_TASK_H
#define TEST_TASK_H

#include "defines.h"

#define TEST_TASK_UPDATES_PER_SEC		(10000)
#define TEST_TASK_INTERVAL_USEC			(USEC_IN_SEC / TEST_TASK_UPDATES_PER_SEC)

void test_task_init(void);
void test_task_callback(void);

#endif // TEST_TASK_H
