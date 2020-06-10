#ifdef APP_BLINK

#include "usr/blink/task_blink.h"
#include "drv/io.h"
#include "sys/scheduler.h"
#include <stdint.h>

// Hold LED state (0: off, 1: red, 2: green, 3: blue)
static uint8_t led_state = 0;

// Scheduler TCB which holds task "context"
static task_control_block_t tcb;

void task_blink_init(void)
{
    // Fill TCB with parameters
    scheduler_tcb_init(&tcb, task_blink_callback, NULL, "blink", TASK_BLINK_INTERVAL_USEC);

    // Register task with scheduler
    scheduler_tcb_register(&tcb);
}

void task_blink_callback(void *arg)
{
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
}

#endif // APP_BLINK
