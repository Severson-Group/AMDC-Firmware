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

#define APP_PMSM_MC

#include <stdio.h>
#include "drv/bsp.h"
#include "sys/commands.h"
#include "sys/serial.h"
#include "sys/defines.h"
#include "sys/log.h"
#include "sys/platform.h"
#include "sys/scheduler.h"

#ifdef APP_PMSM_MC
#include "usr/pmsm_mc/app_pmsm_mc.h"
#endif

int main()
{
	// Required system initialization
	init_platform();

	// User BSP library initialization
	bsp_init();

	// Initialize system tasks
	serial_init();
	commands_init();
	log_init();

	// User tasks initialization
	task_dac_test_init();
#ifdef APP_PMSM_MC
	app_pmsm_mc_init();
#endif

	// Show start message to user, asking for cmds
	commands_start_msg();

	// Initialize scheduler (sets up h/w timer, interrupt, etc)
	scheduler_init();

	// Run scheduler => this takes over the system and never returns!
	scheduler_run();

	cleanup_platform();
	return 0;
}

