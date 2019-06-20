#ifdef APP_BETA_LABS

#include "task_mc.h"

#include "../../sys/scheduler.h"

static task_control_block_t tcb;

uint8_t task_mc_is_inited(void)
{
	return scheduler_tcb_is_registered(&tcb);
}

void task_mc_init(void)
{
	scheduler_tcb_init(&tcb, task_mc_callback, NULL, "mc", TASK_MC_INTERVAL_USEC);
	scheduler_tcb_register(&tcb);
}

void task_mc_deinit(void)
{
	scheduler_tcb_unregister(&tcb);
}

void task_mc_callback(void *arg) {

}

#endif // APP_BETA_LABS
