#include "app_pmsm_mc.h"
#include "task_test.h"
#include "cmd/cmd_cc.h"
#include "cmd/cmd_mc.h"

void app_pmsm_mc_init(void)
{
	task_test_init();

	cmd_cc_register();
	cmd_mc_register();
}
