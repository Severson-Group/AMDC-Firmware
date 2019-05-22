/*
 * Bare AMDC Application
 *
 * Nathan Petersen, 4/17/2019
 *
 * -----------
 *
 * This is a bare application which can be used as
 * a starting point for simple AMDC applications.
 *
 * NOTE: UART uses 115200 baud
 *
 */

#include <stdio.h>
#include "../bsp/bsp.h"
#include "platform.h"
#include "scheduler.h"
#include "defines.h"
#include "debug.h"
#include "commands.h"
#include "task_test.h"
#include "task_cc.h"
#include "task_mc.h"

int main()
{
	// Required system initialization
	init_platform();

	// User BSP library initialization
	bsp_init();

	// Initialize system tasks
	debug_init();
	commands_init();

	// User tasks initialization
	task_mc_init();
	task_cc_init();
	task_test_init();

	// Command zero speed to motion control task
	task_mc_set_omega_star(0); // rad/s

	// Initialize scheduler (sets up h/w timer, interrupt)
	scheduler_init();

	// Run scheduler => this takes over the system and never returns!
	scheduler_run();

	cleanup_platform();
	return 0;
}
