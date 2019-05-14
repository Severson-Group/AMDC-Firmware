#include "debug.h"
#include "scheduler.h"
#include <stdio.h>

void debug_init(void)
{
	printf("DB:\tInitializing debug task...\n");
	scheduler_register_task(debug_callback, DEBUG_INTERVAL_USEC);
}


void debug_callback(void)
{
	printf("a");
}
