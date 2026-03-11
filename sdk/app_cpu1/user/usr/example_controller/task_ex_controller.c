
#ifdef APP_EXAMPLE_CONTROLLER

#include <usr/example_controller/task_ex_controller.h>
#include "sys/scheduler.h"
#include "sys/commands.h"
#include <math.h>
#include "simulink/exampleController.h"         /* Model's header file */
#include "simulink/rtwtypes.h"

// Scheduler TCB which holds task "context"
static task_control_block_t tcb;

int task_ex_controller_init(void)
{
    if (scheduler_tcb_is_registered(&tcb)) {
        return FAILURE;
    }

    /* Initialize model */
    exampleController_initialize();

    // Fill TCB with parameters
    scheduler_tcb_init(&tcb, task_ex_controller_callback,
                        NULL, "exctrl", TASK_EX_CONTROLLER_INTERVAL_USEC);

    // Register task with scheduler
    return scheduler_tcb_register(&tcb);
}

int task_ex_controller_deinit(void)
{
    return scheduler_tcb_unregister(&tcb);
}

double out = 0;

void task_ex_controller_callback(void *arg)
{
	/* Attach rt_OneStep to a timer or interrupt service routine with
	* period 1.0E-6 seconds (the model's base sample time) here.
	*/

	/* Set model inputs here */
	exampleController_U.Input = (real_T) 5.0;

	/* Step the model */
	exampleController_step();


	/* Get model outputs here */
	out = (double) exampleController_Y.Output;
}

#endif // APP_EXAMPLE_CONTROLLER
