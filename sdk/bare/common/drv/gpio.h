#ifndef GPIO_H
#define GPIO_H

#include "usr/user_defines.h"

#if HARDWARE_REVISION == 3

#include <stdint.h>

void gpio_init(void);

void gpio_pin_read(uint8_t port, uint8_t pin, uint8_t *value);
void gpio_pin_write(uint8_t port, uint8_t pin, uint8_t value);

void gpio_port_read(uint8_t port, uint16_t *value);
void gpio_port_write(uint8_t port, uint16_t value);

void gpio_set_dir(uint8_t group, uint8_t dir);

#endif // HARDWARE_REVISION

#endif // GPIO_H
