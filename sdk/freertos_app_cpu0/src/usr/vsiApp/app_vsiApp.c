
#ifdef APP_VSI

#include "usr/vsiApp/app_vsiApp.h"
#include "usr/vsiApp/cmd/cmd_vsiApp.h"
#include "usr/vsiApp/task_vsiApp.h"
#include "drv/timing_manager.h"
#include "drv/gp3io_mux.h"

void app_vsiApp_init(void)
{
	// Configure GP3IO mux for the AMDS
	// GP3IO_MUX_#_BASE_ADDR means AMDC GPIO Port #
	// GP3IO_MUX_DEVICE1 is AMDS driver IP block
	// GP3IO_MUX_DEVICE2 is Eddy Current Sensor IP block
	gp3io_mux_set_device(GP3IO_MUX_2_BASE_ADDR, GP3IO_MUX_DEVICE1);
	// Enable data sampling for AMDS on GPIO Port 2
	timing_manager_enable_sensor(AMDS_2);
	// Enable ADC (timing manager)
	timing_manager_enable_sensor(ADC);
	// Register "vsi" command with system
	cmd_vsiApp_register();
}

#endif // APP_VSI
