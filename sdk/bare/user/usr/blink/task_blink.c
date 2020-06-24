#ifdef APP_BLINK

#include "usr/blink/task_blink.h"
#include "drv/led.h"
#include "sys/scheduler.h"
#include <math.h>
#include <stdint.h>

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

// Scheduler TCB which holds task "context"
static task_control_block_t tcb;

// Example logging variables for testing
int LOG_var1 = 0;
float LOG_var2 = 0;
double LOG_var3 = 0;

static double theta = 0;
static double dtheta = 0.01;

void task_blink_init(void)
{
    if (scheduler_tcb_is_registered(&tcb)) {
        return;
    }

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

    // Turn off all LEDs
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
        led_set_color(i, LED_COLOR_BLACK);
    }

    // Reset state
    led_pos = 0;
    led_color_idx = 0;
}

void task_blink_callback(void *arg)
{
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
        led_set_color(i, led_pos == i ? led_colors[led_color_idx] : LED_COLOR_BLACK);
    }

    if (++led_pos == NUM_LEDS) {
        led_pos = 0;

        if (++led_color_idx >= NUM_LED_COLORS) {
            led_color_idx = 0;
        }
    }

    // Update logging variables
    LOG_var1++;
    LOG_var2 = sinf(theta);
    LOG_var3 = cos(theta);
    theta += dtheta;
}

#endif // APP_BLINK
