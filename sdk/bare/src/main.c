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
#include "test_task.h"

int main()
{
    init_platform();
    bsp_init();

    io_led_color_t color;
    color.r = 0;
    color.g = 0;
    color.b = 255;
    io_led_set(&color);

	// Initialize tasks...
	test_task_init();

	// Initialize scheduler (sets up h/w timer, interrupt)
	printf("Initializing scheduler...\n");
    scheduler_init();

    // Run scheduler => this takes over the system and never returns!
	printf("Running scheduler...\n");
    scheduler_run();

    cleanup_platform();
    return 0;
}

