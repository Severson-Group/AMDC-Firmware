
#ifdef APP_VSI

#include "usr/vsiApp/app_vsiApp.h"
#include "usr/vsiApp/cmd/cmd_vsiApp.h"
#include "usr/vsiApp/task_vsiApp.h"

void app_vsiApp_init(void)
{
	// Register "vsi" command with system
	cmd_vsiApp_register();
}

#endif // APP_VSI
