#ifdef APP_BLINK

#include "usr/blink/task_blink.h"
#include "sys/scheduler.h"
#include "usr/user_config.h"
#include <stdint.h>

#if USER_CONFIG_HARDWARE_TARGET == 4
#include "drv/led.h"
#endif // USER_CONFIG_HARDWARE_TARGET

#if USER_CONFIG_HARDWARE_TARGET == 3
#include "drv/io.h"
#endif // USER_CONFIG_HARDWARE_TARGET

#if USER_CONFIG_HARDWARE_TARGET == 3
// Hold LED state (0: off, 1: red, 2: green, 3: blue)
static uint8_t led_state = 0;
#endif // USER_CONFIG_HARDWARE_TARGET

#if USER_CONFIG_HARDWARE_TARGET == 4
// Hold LED animation state
static uint8_t led_pos = 0;
static uint8_t led_color_idx = 0;
#define NUM_LED_COLORS (7)
static led_color_t led_colors[NUM_LED_COLORS] = {
    LED_COLOR_RED,     //
    LED_COLOR_GREEN,   //
    LED_COLOR_BLUE,    //
    LED_COLOR_YELLOW,  //
    LED_COLOR_CYAN,    //
    LED_COLOR_MAGENTA, //
    LED_COLOR_WHITE,   //
};
#endif // USER_CONFIG_HARDWARE_TARGET

// Scheduler TCB which holds task "context"
static task_control_block_t tcb;

void task_blink_init(void)
{
    // Fill TCB with parameters
    scheduler_tcb_init(&tcb, task_blink_callback, NULL, "blink", TASK_BLINK_INTERVAL_USEC);

    // Register task with scheduler
    scheduler_tcb_register(&tcb);
}

void task_blink_deinit(void)
{
    if (!scheduler_tcb_is_registered(&tcb)) {
        return;
    }

    // Register task with scheduler
    scheduler_tcb_unregister(&tcb);

#if USER_CONFIG_HARDWARE_TARGET == 4
    // Turn off all LEDs
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
        led_set_color(i, LED_COLOR_BLACK);
    }

    // Reset state
    led_pos = 0;
    led_color_idx = 0;
#endif // USER_CONFIG_HARDWARE_TARGET
}

void task_blink_callback(void *arg)
{
#if USER_CONFIG_HARDWARE_TARGET == 3
    // Set LED output via I/O driver
    io_led_color_t color = { 0 };
    color.r = led_state == 1 ? 1 : 0;
    color.g = led_state == 2 ? 1 : 0;
    color.b = led_state == 3 ? 1 : 0;
    io_led_set(&color);

    // Update LED state for next time task is called
    if (++led_state >= 4) {
        led_state = 0;
    }
#endif // USER_CONFIG_HARDWARE_TARGET

#if USER_CONFIG_HARDWARE_TARGET == 4
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
        led_set_color(i, led_pos == i ? led_colors[led_color_idx] : LED_COLOR_BLACK);
    }

    if (++led_pos == NUM_LEDS) {
        led_pos = 0;

        if (++led_color_idx >= NUM_LED_COLORS) {
            led_color_idx = 0;
        }
    }
#endif // USER_CONFIG_HARDWARE_TARGET
}

#endif // APP_BLINK
