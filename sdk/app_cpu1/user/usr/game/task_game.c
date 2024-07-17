#ifdef APP_GAME

#include "usr/game/task_game.h"
#include "drv/led.h"
#include "sys/scheduler.h"
#include <stdint.h>
#include <stdlib.h>

/* from: https://www.quora.com/Can-we-get-a-random-number-without-using-rand-in-C-C-and-Python */
int randCustom() { 
    static int next = 3251 ; // Anything you like here - but not 
                            // 0000, 0100, 2500, 3792, 7600, 
                            // 0540, 2916, 5030 or 3009. 
    next = ((next * next) / 100 ) % 10000; 
    return next ; 
} 

int randomInt(int lowerBound, int upperBound) { // random integer between lower and upper bound (inclusive)
    return randCustom() % (upperBound + 1 - lowerBound) + lowerBound;  
}

int gameScore = 0;

void incrementGameScore() {
    gameScore++;
}

void decrementGameScore() {
    gameScore--;
}

uint8_t game_led_on = 1;

uint8_t getGameLedOn() {
    return game_led_on;
}

void setGameLedOn(uint8_t setTo) {
	if (!game_started()) {
		start_game();
	}
    led_set_color(game_led_on - 1, LED_COLOR_BLACK);
    led_set_color(setTo - 1, LED_COLOR_GREEN);
    game_led_on = setTo;
}

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

int task_game_init(void)
{
    // Turn off all LEDs
    for (uint8_t i = 1; i < NUM_LEDS; i++) {
        led_set_color(i, LED_COLOR_BLACK);
    }
    // Turn on first LED to RED
    led_set_color(0, LED_COLOR_RED);
    if (scheduler_tcb_is_registered(&tcb)) {
        return FAILURE;
    }

    // Fill TCB with parameters
    scheduler_tcb_init(&tcb, task_game_callback, NULL, "game", TASK_GAME_INTERVAL_USEC);

    // Register task with scheduler
    return scheduler_tcb_register(&tcb);
}

int task_game_deinit(void)
{
    // Turn off all LEDs
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
        led_set_color(i, LED_COLOR_BLACK);
    }

    // Reset state
    led_pos = 0;
    led_color_idx = 0;

    // Unregister task with scheduler
    return scheduler_tcb_unregister(&tcb);
}

int callbackIter = -2;
int secondsLeft = 30;

void start_game() {
	callbackIter = 0;
	secondsLeft = 30;
}

void reset_game() {
	// Turn off all LEDs
	for (uint8_t i = 1; i < NUM_LEDS; i++) {
		led_set_color(i, LED_COLOR_BLACK);
	}
	// Turn on first LED to RED
	led_set_color(0, LED_COLOR_RED);
	callbackIter = -2;
	gameScore = 0;
	game_led_on = 1;
}

int game_started() {
	if (callbackIter == -2) {
		return 0;
	}
	return 1;
}

void task_game_callback(void *arg)
{
    if (callbackIter >= 0) {
    	// printf("Callback called %d\n", callbackIter);
        callbackIter++;
        if (callbackIter % 5 == 0) {
        	printf("%d seconds left\n", secondsLeft);
        	secondsLeft--;
		}
        if (callbackIter > 150) {
            game_led_on = 0;
            for (uint8_t i = 0; i < NUM_LEDS; i++) {
                led_set_color(i, LED_COLOR_BLACK);
            }
            callbackIter = -1;
            printf("Time's up! Score: %d\n", gameScore);
        }
    }
    // for (uint8_t i = 0; i < NUM_LEDS; i++) {
    //     led_set_color(i, led_pos == i ? led_colors[led_color_idx] : LED_COLOR_BLACK);
    // }

    // if (++led_pos == NUM_LEDS) {
    //     led_pos = 0;

    //     if (++led_color_idx >= NUM_LED_COLORS) {
    //         led_color_idx = 0;
    //     }
    // }
}

void task_game_stats_print(void)
{
    task_stats_print(&tcb.stats);
}

void task_game_stats_reset(void)
{
    task_stats_reset(&tcb.stats);
}

#endif // APP_GAME
