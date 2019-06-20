#ifdef APP_BETA_LABS

#include "app_beta_labs.h"
#include "cmd/cmd_inv.h"
#include "cmd/cmd_dtc.h"
#include "cmd/cmd_cc.h"
#include "cmd/cmd_mc.h"

void app_beta_labs_init(void)
{
	cmd_inv_register();
	cmd_dtc_register();
	cmd_cc_register();
	cmd_mc_register();
}

#endif // APP_BETA_LABS
