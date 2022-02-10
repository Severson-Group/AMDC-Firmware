#include "usr/user_config.h"
#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D

#include "drv/gpio_mux.h"
#include "xil_io.h"
#include <stdint.h>
#include <stdio.h>

#define GPIO_MUX_BASE_ADDR (0x43C70000)

static int ports[] = { GPIO_PORT1, GPIO_PORT2 };

void gpio_mux_init(void)
{
    printf("GPIO LINES:\tInitializing...\n");

    for (int i = 0; i < 2; i++) {
        if (ports[i]) {
            // Map the port status lines to the corresponding device defined in the header file
            gpio_mux_set_device(i, ports[i]);
        }
    }
}

void gpio_mux_set_device(uint8_t port, gpio_mux_device_t device)
{
    Xil_Out32(GPIO_MUX_BASE_ADDR + (port * sizeof(uint32_t)), device);
}

#endif // USER_CONFIG_HARDWARE_TARGET
