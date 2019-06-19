#include "injection.h"
#include "scheduler.h"
#include "cmd/cmd_inj.h"

static task_control_block_t tcb;

void injection_init(void)
{
	scheduler_tcb_init(&tcb, injection_callback, NULL, "inj", INJECTION_INTERVAL_USEC);
	scheduler_tcb_register(&tcb);

	// Register command
	cmd_inj_register();
}

void injection_callback(void *arg)
{

}
