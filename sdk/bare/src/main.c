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
#include "task_cc.h"
#include "task_mc.h"
#include "defines.h"
#include "debug.h"

int main()
{
	// Required system initialization
	init_platform();

	// User BSP library initialization
	bsp_init();

	// User tasks initialization
	task_mc_init();
	task_cc_init();

	// Command RPM to motion control task
	task_mc_set_omega_star(PI2 * 2); // 2 RPM

	// Initialize scheduler (sets up h/w timer, interrupt)
	scheduler_init();

	// Run scheduler => this takes over the system and never returns!
	scheduler_run();

	cleanup_platform();
	return 0;
}

