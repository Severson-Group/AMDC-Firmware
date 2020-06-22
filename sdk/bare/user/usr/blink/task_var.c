#ifdef APP_BLINK

#include "usr/blink/task_var.h"
#include "sys/scheduler.h"
#include <stdint.h>

// Scheduler TCB which holds task "context"
static task_control_block_t tcb;

double LOG_foo1 = 0;
double LOG_foo2 = 0;
double LOG_foo3 = 0;
double LOG_foo4 = 0;
double LOG_foo5 = 0;

void task_var_init(void)
{
    if (scheduler_tcb_is_registered(&tcb)) {
        return;
    }

    // Fill TCB with parameters
    scheduler_tcb_init(&tcb,
            task_var_callback, NULL,
            "var", TASK_VAR_INTERVAL_USEC);

    // Register task with scheduler
    scheduler_tcb_register(&tcb);
}

void task_var_callback(void *arg)
{
	LOG_foo1 += 0.0001;
	LOG_foo2 += LOG_foo1;
	LOG_foo3 = LOG_foo1 * LOG_foo2;
	LOG_foo4 = LOG_foo3 / LOG_foo1;
	LOG_foo5 = 3.14;
}

#endif // APP_BLINK
