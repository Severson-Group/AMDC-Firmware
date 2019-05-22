#include "bsp.h"
#include <stdio.h>

void bsp_init(void)
{
	printf("BSP:\tInitializing...\n");

	uart_init();

	encoder_init();
	analog_init();
	pwm_init();
	io_init();
	gpio_init();
}
