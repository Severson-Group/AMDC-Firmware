#ifdef APP_PARAMS

#include "app_params.h"
#include "cmd/cmd_cc.h"
#include "cmd/cmd_inv.h"

void app_params_init(void)
{
	cmd_cc_register();
	cmd_inv_register();
}

#endif // APP_PARAMS
