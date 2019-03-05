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
#include "platform.h"
#include "xil_printf.h"

#include "encoder.h"
#include "analog.h"
#include "inverter.h"
#include "io.h"
#include "gpio.h"

#include "sinetable.h"

void init(void)
{
    init_platform();

    encoder_init();
    analog_init();
    inverter_init();
    io_init();
    gpio_init();
}

void cleanup(void)
{
    cleanup_platform();
}

int main()
{
	init();

	// Turn on all PWM outputs to 0% duty cycle
	int inv;
	for (inv = 0; inv < 24; inv++) {
		inverter_set_duty(inv, 0);
	}


	int idx = 0;
	while (1) {
		// Spin for a while
		for (volatile int i = 0; i < 1280; i++);

		// Do this at 60Hz:
		inverter_set_duty(2, sinetable[idx]);

		idx++;
		if (idx >= SINETABLE_LENGTH) {
			idx = 0;
		}
	}

	cleanup();
	return 0;
}

//int main()
//{
//	init();
//
//	// Turn on all PWM outputs to 0% duty cycle
//	int inv;
//	for (inv = 0; inv < 24; inv++) {
//		inverter_set_duty(inv, 0);
//	}
//
//	static uint8_t r = 1;
//	static int counter = 0;
//
//	while (1) {
//		// Read in all 16 analog channel values
//		float valuef[16];
//
//		for (int i = 0; i < 16; i++) {
//			valuef[i] = 0;
//
//			// Average 16 reads
//			for (int j = 0; j < 16; j++) {
//				float tmp;
//				analog_getf(i+1, &tmp);
//				valuef[i] += tmp;
//			}
//
//			valuef[i] /= 16;
//		}
//
//		// Print ANLG1 value
//		printf("ANLG%d:\t%f\n", 1, valuef[0]);
//
//
//		if (counter++ > 100) {
//			counter = 0;
//			io_led_color_t color = { r, 0, 0 };
//			io_led_set(color);
//			r = 1 - r;
//		}
//
//
//		uint8_t duty = 127 + ((valuef[0] / 10.0) * 128);
//		inverter_set_duty(2, duty);
//	}
//
//    cleanup();
//    return 0;
//}



//    printf("Hello World\n\r");
//
//    // Test encoder
//    int32_t steps;
//    encoder_steps(&steps);
//    printf("Steps: %d\n", (int)steps);
//
//
//    // Test inverter
//    //inverter_set_duty_ratio(0, 0, 127);
//
//
//    // Test I/O
//    io_led_color_t c = {0, 255, 0};
//    io_led_set(c);
//    uint8_t sw1, sw2;
//    io_switch_get(&sw1, &sw2);
//    uint8_t btn1;
//    io_button_get(&btn1);
//    printf("I/O status: sw1: %d, sw2: %d, btn1: %d\n", sw1, sw2, btn1);
//
//
//    // Test GPIO
//    uint8_t gpio_value;
//    gpio_pin_read(0, 1, &gpio_value);
//    printf("GPIO value: %d\n", gpio_value);

//	while (1) {
//		uint8_t btn1;
//		uint8_t sw1;
//		uint8_t sw2;
//
//		io_button_get(&btn1);
//		io_switch_get(&sw1, &sw2);
//
//		io_led_color_t color = {1,1,1};
//
//		if (btn1) color.r = 0;
//		if (sw1)  color.g = 0;
//		if (sw2)  color.b = 0;
//
//		io_led_set(color);
//	}
