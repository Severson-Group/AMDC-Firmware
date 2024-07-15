#ifdef APP_BETA_LABS

#include "usr/beta_labs/app_beta_labs.h"
#include "usr/beta_labs/cmd/cmd_bemfo.h"
#include "usr/beta_labs/cmd/cmd_cc.h"
#include "usr/beta_labs/cmd/cmd_dtc.h"
#include "usr/beta_labs/cmd/cmd_inv.h"
#include "usr/beta_labs/cmd/cmd_mc.h"
#include "usr/beta_labs/cmd/cmd_vsi.h"
#include "usr/beta_labs/task_mo.h"

void app_beta_labs_init(void)
{
    task_mo_init();

    cmd_inv_register();
    cmd_dtc_register();
    cmd_vsi_register();
    cmd_cc_register();
    cmd_mc_register();
    cmd_bemfo_register();
}

#endif // APP_BETA_LABS
