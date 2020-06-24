#include "usr/user_defines.h"

#if (HARDWARE_REVISION == 3) || \
	(HARDWARE_REVISION == 4)
	// Ensure a valid hardware revision is specified
	// NOTE: this firmware only supports REV C hardware onward
#else
	#error "ERROR: Hardware revision not specified correctly"
#endif

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
#include <stdio.h>

#if HARDWARE_REVISION == 4
#include "drv/led.h"
#endif

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

#if HARDWARE_REVISION == 4
    led_init();
#endif

#if HARDWARE_REVISION == 3
    io_init();
    gpio_init();
#endif

    // The DAC driver is current not supported on any hardware
    // dac_init();

#ifdef ENABLE_WATCHDOG
    watchdog_init();
#endif

    cmd_hw_register();
}
