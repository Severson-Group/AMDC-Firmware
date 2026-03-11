
#ifdef APP_EXAMPLE_CONTROLLER

#include "usr/controller/app_controller.h"
#include "usr/controller/cmd/cmd_ctrl.h"
#include "drv/timing_manager.h"
#include "drv/gp3io_mux.h"
#include <usr/example_controller/task_ex_controller.h>

void app_ex_controller_init(void)
{
	// register commands
    cmd_ctrl_register();

    // Initialize controller with system
    task_ex_controller_init();
}

#endif // APP_EXAMPLE_CONTROLLER
