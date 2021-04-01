#include "drv/hardware_targets.h"
#include "usr/user_config.h"

#if USER_CONFIG_HARDWARE_TARGET == HW_TARGET_AMDC_REV_C

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

void gpio_init(void);

void gpio_pin_read(uint8_t port, uint8_t pin, uint8_t *value);
void gpio_pin_write(uint8_t port, uint8_t pin, uint8_t value);

void gpio_port_read(uint8_t port, uint16_t *value);
void gpio_port_write(uint8_t port, uint16_t value);

void gpio_set_dir(uint8_t group, uint8_t dir);

#endif // GPIO_H

#endif // USER_CONFIG_HARDWARE_TARGET
