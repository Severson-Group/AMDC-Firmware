#ifdef APP_PCBTEST

#include "usr/pcbtest/task_test.h"
#include "drv/hardware_targets.h"
#include "sys/scheduler.h"
#include "usr/user_config.h"
#include <stdint.h>

// Scheduler TCB which holds task "context"
static task_control_block_t tcb;

void task_test_init(void)
{
    if (scheduler_tcb_is_registered(&tcb)) {
        return;
    }

    // Fill TCB with parameters
    scheduler_tcb_init(&tcb, task_test_callback, NULL, "test", TASK_TEST_INTERVAL_USEC);

    // Register task with scheduler
    scheduler_tcb_register(&tcb);
}

void task_test_deinit(void)
{
    if (!scheduler_tcb_is_registered(&tcb)) {
        return;
    }

    // Register task with scheduler
    scheduler_tcb_unregister(&tcb);
}

void task_test_callback(void *arg)
{

}

#endif // APP_PCBTEST
