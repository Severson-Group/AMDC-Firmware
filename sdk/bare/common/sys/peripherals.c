#include "sys/peripherals.h"
#include "drv/hardware_targets.h"
#include "usr/user_config.h"

#if (USER_CONFIG_HARDWARE_TARGET == HW_TARGET_AMDC_REV_C) || (USER_CONFIG_HARDWARE_TARGET == HW_TARGET_AMDC_REV_D)
// Ensure a valid hardware target is specified
// NOTE: this firmware only supports REV C hardware onward
#else
#error "ERROR: Hardware target not specified correctly"
// If you have this error, please define USER_CONFIG_HARDWARE_TARGET in your usr/user_config.h file!
#endif

#include "drv/analog.h"
#include "drv/cpu_timer.h"
#include "drv/dac.h"
#include "drv/encoder.h"
#include "drv/fpga_timer.h"
#include "drv/pwm.h"
#include "drv/timer.h"
#include "drv/uart.h"
#include "drv/watchdog.h"
#include "sys/cmd/cmd_hw.h"
#include "sys/errors.h"
#include <stdio.h>

#if USER_CONFIG_HARDWARE_TARGET == HW_TARGET_AMDC_REV_C
#include "drv/gpio.h"
#include "drv/io.h"
#endif

#if USER_CONFIG_HARDWARE_TARGET == HW_TARGET_AMDC_REV_D
#include "drv/dac.h"
#include "drv/eddy_current_sensor.h"
#include "drv/gpio_mux.h"
#include "drv/led.h"
#include "drv/sts_mux.h"
#endif

error_t peripherals_init(void)
{
    if (uart_init() != ERROR_OK) {
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
    case HW_TARGET_AMDC_REV_C:
        printf("Hardware Target: REV C\n");
        break;

    case HW_TARGET_AMDC_REV_D:
        printf("Hardware Target: REV D\n");
        break;

    default:
        printf("ERROR: Unknown AMDC hardware revision\n");
        while (1) {
        }
    }

    printf("(C) 2021 Severson Research Group\n");
    printf("--------------------------------\n");
    printf("\n");

    printf("BSP:\tInitializing...\n");

    // Initialize common motor drive peripherals
    encoder_init();
    analog_init(ANALOG_BASE_ADDR);
    pwm_init();

    // Initialize timer modules
    fpga_timer_init();
    cpu_timer_init();

#if USER_CONFIG_HARDWARE_TARGET == HW_TARGET_AMDC_REV_D
    // Initialize peripherals specific to REV D hardware
    led_init();
    sts_mux_init();
    gpio_mux_init();
    dac_init();
    eddy_current_sensor_init();
#endif

#if USER_CONFIG_HARDWARE_TARGET == HW_TARGET_AMDC_REV_C
    // Initialize peripherals specific to REV C hardware
    io_init();
    gpio_init();
#endif

#if USER_CONFIG_ENABLE_WATCHDOG == 1
    watchdog_init();
#endif

    // Register the 'hw' command with the system
    cmd_hw_register();

    return ERROR_OK;
}
