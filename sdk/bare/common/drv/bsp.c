#include "drv/hardware_targets.h"
#include "usr/user_config.h"

#if (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_C) || (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D)
// Ensure a valid hardware target is specified
// NOTE: this firmware only supports REV C hardware onward
#else
#error "ERROR: Hardware target not specified correctly"
// If you have this error, please define USER_CONFIG_HARDWARE_TARGET in your usr/user_config.h file!
#endif

#include "drv/analog.h"
#include "drv/bsp.h"
#include "drv/cpu_timer.h"
#include "drv/dac.h"
#include "drv/encoder.h"
#include "drv/fpga_timer.h"
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
    if (uart_init() != SUCCESS) {
        printf("ERROR: UART init failed!\n");
        while (1) {
        };
    }

    // Busy loop to wait for UART to warm up... :)
    volatile int i;
    for (i = 0; i < 10e6; i++) {
        asm("nop");
    }

    // Print welcome message to user
    printf("Advanced Motor Drive Controller\n");

    switch (USER_CONFIG_HARDWARE_TARGET) {
    case AMDC_REV_C:
        printf("Hardware Target: REV C\n");
        break;

    case AMDC_REV_D:
        printf("Hardware Target: REV D\n");
        break;

    default:
        printf("ERROR: Unknown AMDC hardware revision\n");
        while (1) {
        }
    }

    printf("(C) 2020 Severson Research Group\n");
    printf("--------------------------------\n");
    printf("\n");

    printf("BSP:\tInitializing...\n");

    encoder_init();
    analog_init(ANALOG_BASE_ADDR);
    pwm_init();

    fpga_timer_init();
    cpu_timer_init();

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
