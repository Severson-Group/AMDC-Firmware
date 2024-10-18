
#ifdef APP_VSI

#include "usr/vsiApp/app_vsiApp.h"
#include "usr/vsiApp/cmd/cmd_vsiApp.h"
#include "usr/vsiApp/task_vsiApp.h"
#include "drv/timing_manager.h"
#include "drv/gp3io_mux.h"

void app_vsiApp_init(void)
{
    // Register "vsi" command with system
	gp3io_mux_set_device(GP3IO_MUX_1_BASE_ADDR, GP3IO_MUX_DEVICE1);
	// Enable data sampling for AMDS on GPIO Port 1
	timing_manager_enable_sensor(AMDS_1);
    cmd_vsiApp_register();

    // task_vsiApp_init();
}

#endif // APP_VSI
