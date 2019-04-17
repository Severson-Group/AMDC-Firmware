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
#include "sinetable.h"

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


//int main()
//{
//	init_platform();
//	bsp_init();
//
//	pwm_set_switching_freq(25000.0);
//
////	pwm_set_duty(0, 0.5);
////	pwm_set_duty(1, 0.5);
////	pwm_set_duty(2, 0.5);
////	pwm_set_duty(23, 0.5);
////
////	while (1);
//
//	int idx = 0;
//	while (1) {
//		// Spin for a while
//		for (volatile int i = 0; i < 1280; i++);
//
//		int idx1 = idx;
//		int idx2 = idx - (SINETABLE_LENGTH / 3);
//		int idx3 = idx + (SINETABLE_LENGTH / 3);
//
//		if (idx1 < 0) idx1 += SINETABLE_LENGTH;
//		if (idx2 < 0) idx2 += SINETABLE_LENGTH;
//		if (idx3 < 0) idx3 += SINETABLE_LENGTH;
//
//		if (idx1 >= SINETABLE_LENGTH) idx1 -= SINETABLE_LENGTH;
//		if (idx2 >= SINETABLE_LENGTH) idx2 -= SINETABLE_LENGTH;
//		if (idx3 >= SINETABLE_LENGTH) idx3 -= SINETABLE_LENGTH;
//
//
//		// Do this at 60Hz:
//		pwm_set_duty(0, sinetable[idx1] / 255.0);
//		pwm_set_duty(1, sinetable[idx2] / 255.0);
//		pwm_set_duty(2, sinetable[idx3] / 255.0);
//
//		idx++;
//		if (idx >= SINETABLE_LENGTH) {
//			idx = 0;
//		}
//	}
//
//	cleanup_platform();
//	return 0;
//}

//int main()
//{
//	init();
//
//	// Turn on all PWM outputs to 0% duty cycle
//	int inv;
//	for (inv = 0; inv < 24; inv++) {
//		pwm_set_duty(inv, 0);
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
//		float current = 3.0 * (valuef[2] / 5.0);
//
//		// Print ANLG1 value
//		printf("ANLG%d:\t%f\t", 3, valuef[2]);
//		printf("CURRENT:\t%f\n", current);
//
//
//		// Blink the RGB LED on and off
//		if (counter++ > 100) {
//			counter = 0;
//			io_led_color_t color = { r, 0, 0 };
//			io_led_set(color);
//			r = 1 - r;
//		}
//
//
//		uint8_t duty = 127 + ((valuef[0] / 10.0) * 128);
//		pwm_set_duty(2, duty);
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
