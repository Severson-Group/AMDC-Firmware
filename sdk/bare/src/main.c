/*
 * Basic AMDC Application
 *
 * Nathan Petersen, 12/12/2018
 *
 * -----------
 *
 * This is a basic application to test the AMDC board
 *
 * It uses UART of 115200 baud
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

