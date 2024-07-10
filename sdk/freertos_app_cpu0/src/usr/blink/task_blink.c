#ifdef APP_BLINK

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
/* other includes */
#include "usr/blink/task_blink.h"
#include "drv/led.h"
#include "sys/defines.h"
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
static TaskHandle_t tcb;

int task_blink_init(void)
{
    // Fill TCB with parameters
	xTaskCreate(task_blink, (const char *) "blink", configMINIMAL_STACK_SIZE,
				NULL, tskIDLE_PRIORITY, &tcb);
    return SUCCESS;
}

int task_blink_deinit(void)
{
    // Turn off all LEDs
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
        led_set_color(i, LED_COLOR_BLACK);
    }

    // Reset state
    led_pos = 0;
    led_color_idx = 0;

    // Unregister task with scheduler
    return SUCCESS;
}

void task_blink(void *arg)
{
	for (;;) {
		vTaskDelay(TASK_BLINK_INTERVAL_TICKS);
		for (uint8_t i = 0; i < NUM_LEDS; i++) {
			led_set_color(i, led_pos == i ? led_colors[led_color_idx] : LED_COLOR_BLACK);
		}

		if (++led_pos == NUM_LEDS) {
			led_pos = 0;

			if (++led_color_idx >= NUM_LED_COLORS) {
				led_color_idx = 0;
			}
		}
	}
}

void task_blink_stats_print(void)
{
//    task_stats_print(&tcb.stats); // no work in freertos
}

void task_blink_stats_reset(void)
{
//    task_stats_reset(&tcb.stats);
}

#endif // APP_BLINK
