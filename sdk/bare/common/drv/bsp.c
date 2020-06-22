#include "drv/bsp.h"
#include "drv/analog.h"
#include "drv/dac.h"
#include "drv/encoder.h"
#include "drv/gpio.h"
#include "drv/io.h"
#include "drv/pwm.h"
#include "drv/timer.h"
#include "drv/uart.h"
#include "drv/watchdog.h"
#include "sys/cmd/cmd_hw.h"
#include "sys/defines.h"
#include "usr/user_defines.h"
#include <stdio.h>

void bsp_init(void)
{
    printf("BSP:\tInitializing...\n");

    int err;

    err = uart_init();
    if (err != CMD_SUCCESS) {
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
