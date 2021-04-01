// Bare AMDC Application
//
// Nathan Petersen, 5/22/2019
//
// -----------
//
// This is a bare application which can be used as
// a starting point for AMDC applications.
//
// It uses a custom "RTOS" -- a simple cooperative
// scheduler which manages device tasks. Users must
// ensure that their tasks do not consume too many
// cycles in a single iteration, else the scheduler
// time quantum will be overrun.
//
// NOTE: UART uses 115200 baud

#include "sys/peripherals.h"
#include "sys/commands.h"
#include "sys/defines.h"
#include "sys/injection.h"
#include "sys/log.h"
#include "sys/platform.h"
#include "sys/scheduler.h"
#include "sys/serial.h"
#include "usr/user_apps.h"
#include "usr/user_config.h"
#include <stdio.h>

#if USER_CONFIG_ENABLE_MOTHERBOARD_SUPPORT == 1
#include "drv/motherboard.h"
#endif

int main()
{
    // Required system initialization
    init_platform();

    // AMDC peripherals initialization
    peripherals_init();

    // Initialize system tasks
    serial_init();
    commands_init();
#if USER_CONFIG_ENABLE_LOGGING == 1
    log_init();
#endif

#if USER_CONFIG_ENABLE_INJECTION == 1
    injection_init();
#endif

#if USER_CONFIG_ENABLE_MOTHERBOARD_SUPPORT == 1
    // Initialize motherboard driver and register command
    motherboard_init(MOTHERBOARD_BASE_ADDR);
#endif

    // Initialize user applications
    user_apps_init();

    // Show start message to user, asking for cmds
    commands_start_msg();

    // Initialize scheduler (sets up h/w timer, interrupt, etc)
    scheduler_init();

    // Run scheduler => this takes over the system and never returns!
    scheduler_run();

    cleanup_platform();
    return 0;
}
