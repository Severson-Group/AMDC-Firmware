#ifndef IO_H
#define IO_H

#include <stdint.h>

typedef struct io_led_color_t {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} io_led_color_t;

void io_init(void);

void io_led_set_c(uint8_t r, uint8_t g, uint8_t b, io_led_color_t *color);
void io_led_set(io_led_color_t *color);

void io_switch_get(uint8_t *sw1, uint8_t *sw2);
void io_button_get(uint8_t *btn1);

#endif // IO_H
