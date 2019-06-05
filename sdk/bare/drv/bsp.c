#include "bsp.h"
#include "analog.h"
#include "encoder.h"
#include "gpio.h"
#include "io.h"
#include "pwm.h"
#include "timer.h"
#include "dac.h"
#include "uart.h"
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
	dac_init();
}