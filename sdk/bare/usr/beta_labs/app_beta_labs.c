#ifdef APP_BETA_LABS

#include "app_beta_labs.h"
#include "task_mo.h"
#include "cmd/cmd_inv.h"
#include "cmd/cmd_dtc.h"
#include "cmd/cmd_cc.h"
#include "cmd/cmd_mc.h"
#include "cmd/cmd_bemfo.h"

void app_beta_labs_init(void)
{
	task_mo_init();

	cmd_inv_register();
	cmd_dtc_register();
	cmd_cc_register();
	cmd_mc_register();
	cmd_bemfo_register();
}

#endif // APP_BETA_LABS
