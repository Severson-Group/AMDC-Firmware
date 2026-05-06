#ifdef APP_CONTROLLER

#include "usr/controller/app_controller.h"
#include "usr/controller/cmd/cmd_ctrl.h"
#include "drv/timing_manager.h"
#include "drv/gp3io_mux.h"
#include "drv/amds.h"
#include "drv/pwm.h"
#include "xil_io.h"

void app_controller_init(void)
{
	// Configure GP3IO mux for the AMDS
	// GP3IO_MUX_#_BASE_ADDR means AMDC GPIO Port #
	// GP3IO_MUX_DEVICE1 is AMDS driver IP block
	// GP3IO_MUX_DEVICE2 is Eddy Current Sensor IP block
	gp3io_mux_set_device(GP3IO_MUX_1_BASE_ADDR, GP3IO_MUX_DEVICE1);

	// Enable data sampling for ADC
	timing_manager_enable_sensor(ADC);

	// Enable data sampling for AMDS on GPIO Port #
	timing_manager_enable_sensor(AMDS_1);

	uint32_t enable = 0x00111111;
	amds_set_enabled(0, enable);  // enable 3 FBC channels

	pwm_disable();
	pwm_set_switching_freq(50000);
	pwm_set_deadtime_ns(150);

	// register commands
    cmd_ctrl_register();
}

#endif // APP_CONTROLLER
