#include "bsp.h"
#include "analog.h"
#include "encoder.h"
#include "gpio.h"
#include "io.h"
#include "pwm.h"
#include "timer.h"
#include "dac.h"
#include "uart.h"
#include "watchdog.h"
#include "../sys/cmd/cmd_hw.h"
#include "../sys/defines.h"
#include "../usr/user_defines.h"
#include <stdio.h>

void bsp_init(void)
{
    printf("BSP:\tInitializing...\n");

    int err;

    err = uart_init();
    if (err != SUCCESS) {
        HANG;
    }

    encoder_init();
    analog_init();
    pwm_init();
    io_init();
    gpio_init();
    dac_init();

#ifdef ENABLE_WATCHDOG
    watchdog_init();
#endif

    cmd_hw_register();
}
