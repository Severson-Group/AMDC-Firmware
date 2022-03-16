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

#include "drv/bsp.h"
#include "sys/cmd/cmd_counter.h"
#include "sys/commands.h"
#include "sys/defines.h"
#include "sys/icc_tx.h"
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

#include "xil_exception.h"
#include "xil_io.h"
#include "xil_mmu.h"
#include "xpseudo_asm.h"

#include "xparameters.h"

// REV E has 4 GPIO ports, each with their own eddy current sensor driver IP block:
#if (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E) && (XPAR_AMDC_EDDY_CURRENT_SENSOR_NUM_INSTANCES != 4)
#error "ERROR: Vivado hardware target is REV D, but usr/user_config.h target is REV E!"
#endif

// REV D has 2 GPIO ports, but only a single eddy current sensor driver IP block:
#if (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D) && (XPAR_AMDC_EDDY_CURRENT_SENSOR_NUM_INSTANCES != 1)
#error "ERROR: Vivado hardware target is REV E, but usr/user_config.h target is REV D!"
#endif

int main()
{
    // Required system initialization
    init_platform();

    // Disable cache on OCM
    // S=b1 TEX=b100 AP=b11, Domain=b1111, C=b0, B=b0
    Xil_SetTlbAttributes(0xFFFF0000, 0x14de2);

    // User BSP library initialization
    bsp_init();

    // Initialize system tasks
    serial_init();
    commands_init();
    icc_tx_init();
#if USER_CONFIG_ENABLE_LOGGING == 1
    log_init();
#endif

#if USER_CONFIG_ENABLE_INJECTION == 1
    injection_init();
#endif

#if USER_CONFIG_ENABLE_MOTHERBOARD_SUPPORT == 1
    // Initialize motherboard driver and register command
    motherboard_init();
#endif

    // Register the "cnt" command
    cmd_counter_register();

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
