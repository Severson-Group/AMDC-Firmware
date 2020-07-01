#include "drv/hardware_targets.h"
#include "usr/user_config.h"

#if (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_C) || (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D)
// Ensure a valid hardware target is specified
// NOTE: this firmware only supports REV C hardware onward
#else
#error "ERROR: Hardware target not specified correctly"
// If you have this error, please define USER_CONFIG_HARDWARE_TARGET in your user_defines.h file!
#endif

#include "drv/analog.h"
#include "drv/bsp.h"
#include "drv/dac.h"
#include "drv/encoder.h"
#include "drv/pwm.h"
#include "drv/timer.h"
#include "drv/uart.h"
#include "drv/watchdog.h"
#include "sys/cmd/cmd_hw.h"
#include "sys/defines.h"
#include <stdio.h>

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_C
#include "drv/gpio.h"
#include "drv/io.h"
#endif

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D
#include "drv/led.h"
#endif

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

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D
    led_init();
#endif

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_C
    io_init();
    gpio_init();
#endif

    // The DAC driver is current not supported on any hardware
    // dac_init();

#if USER_CONFIG_ENABLE_WATCHDOG == 1
    watchdog_init();
#endif

    cmd_hw_register();
}
