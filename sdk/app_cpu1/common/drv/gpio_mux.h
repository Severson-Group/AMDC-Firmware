#include "usr/user_config.h"
#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D

#ifndef GPIO_MUX_H
#define GPIO_MUX_H

#include <stdbool.h>
#include <stdint.h>

#include "xparameters.h"

#define GPIO_MUX_BASE_ADDR (XPAR_AMDC_GPIO_MUX_0_S00_AXI_BASEADDR)

typedef enum {
    GPIO_MUX_UNUSED = 0,
    GPIO_MUX_DEVICE1 = 1,
    GPIO_MUX_DEVICE2 = 2,
    GPIO_MUX_DEVICE3 = 3,
    GPIO_MUX_DEVICE4 = 4,
    GPIO_MUX_DEVICE5 = 5,
    GPIO_MUX_DEVICE6 = 6,
    GPIO_MUX_DEVICE7 = 7,
    GPIO_MUX_DEVICE8 = 8
} gpio_mux_device_t;

#define GPIO_PORT1 GPIO_MUX_UNUSED
#define GPIO_PORT2 GPIO_MUX_UNUSED

#define GPIO_MUX_DEVICE_COUNT 8

void gpio_mux_init(void);

void gpio_mux_set_device(uint8_t, gpio_mux_device_t);

#endif // GPIO_MUX_H

#endif // USER_CONFIG_HARDWARE_TARGET
