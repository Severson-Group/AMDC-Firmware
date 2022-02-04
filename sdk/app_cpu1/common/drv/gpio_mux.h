#include "usr/user_config.h"
#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D

#ifndef GPIO_MUX_H
#define GPIO_MUX_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    GPIO_MUX_UNUSED = 0,
    GPIO_MUX_DEVICE1 = 1,
    GPIO_MUX_DEVICE2 = 2,
    GPIO_MUX_DEVICE3 = 3,
    GPIO_MUX_DEVICE4 = 4
} gpio_mux_device_t;

#define GPIO_PORT1 GPIO_MUX_UNUSED
#define GPIO_PORT2 GPIO_MUX_UNUSED

void gpio_mux_init(void);

void gpio_mux_set_device(uint8_t, gpio_mux_device_t);

#endif // GPIO_MUX_H

#endif // USER_CONFIG_HARDWARE_TARGET
