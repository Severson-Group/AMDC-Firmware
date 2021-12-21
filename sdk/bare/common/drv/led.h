#ifndef LED_H
#define LED_H

#include <stdint.h>

typedef struct led_color_t {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} led_color_t;

// clang-format off
static const led_color_t LED_COLOR_RED     = { 020, 000, 000 };
static const led_color_t LED_COLOR_GREEN   = { 000, 020, 000 };
static const led_color_t LED_COLOR_BLUE    = { 000, 000, 020 };
static const led_color_t LED_COLOR_YELLOW  = { 020, 020, 000 };
static const led_color_t LED_COLOR_CYAN    = { 000, 020, 020 };
static const led_color_t LED_COLOR_MAGENTA = { 020, 000, 020 };
static const led_color_t LED_COLOR_WHITE   = { 020, 020, 020 };
static const led_color_t LED_COLOR_BLACK   = { 000, 000, 000 };
// clang-format on

typedef enum {
    LED0 = 0,
    LED1,
    LED2,
    LED3,
    NUM_LEDS,
} led_t;

void led_init(void);

void led_set_color(led_t idx, led_color_t color);
void led_set_color_bytes(led_t idx, uint8_t r, uint8_t g, uint8_t b);
void led_set_raw(led_t idx, uint32_t color);

led_color_t led_get_color(led_t idx);
uint32_t led_get_raw(led_t idx);

#endif // LED_H
