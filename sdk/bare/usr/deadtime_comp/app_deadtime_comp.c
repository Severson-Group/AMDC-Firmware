#include "app_deadtime_comp.h"
#include "cmd/cmd_dtc.h"

void app_deadtime_comp_init(void)
{
	cmd_dtc_register();
}
