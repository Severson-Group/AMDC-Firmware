#include "drv/led.h"
#include "xil_io.h"
#include <stdint.h>

#define LED_BASE_ADDR (0x43C30000)

// Color Data Bit Format
// =====================
//
// NOTE: see datasheet for bit ordering:
//
// 24-bit data format
//
// MSB -- G8R8B8 -- LSB
//
// Bits 23-16 : green
// Bits 15-8  : red
// Bits 7-0   : blue

void led_init(void)
{
    // Turn off all LEDs to start
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
        led_set_raw(i, 0);
    }
}

void led_set_color(led_t idx, led_color_t color)
{
    uint32_t raw = 0;
    raw |= ((uint32_t) color.g & 0x000000FF) << 16;
    raw |= ((uint32_t) color.r & 0x000000FF) << 8;
    raw |= ((uint32_t) color.b & 0x000000FF) << 0;

    led_set_raw(idx, raw);
}

void led_set_color_bytes(led_t idx, uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t raw = 0;
    raw |= ((uint32_t) g & 0x000000FF) << 16;
    raw |= ((uint32_t) r & 0x000000FF) << 8;
    raw |= ((uint32_t) b & 0x000000FF) << 0;

    led_set_raw(idx, raw);
}

void led_set_raw(led_t idx, uint32_t color)
{
    // NOTE: Writing color values to the device registers will
    // automatically trigger the transmission of color data
    // out to the LEDs.
    //
    // The user does not have to manually trigger the transmission.

    Xil_Out32(LED_BASE_ADDR + (idx * sizeof(uint32_t)), color);
}

led_color_t led_get_color(led_t idx)
{
    uint32_t raw = led_get_raw(idx);

    led_color_t ret;
    ret.g = (raw & 0x00FF0000) >> 16;
    ret.r = (raw & 0x0000FF00) >> 8;
    ret.b = (raw & 0x000000FF) >> 0;

    return ret;
}

uint32_t led_get_raw(led_t idx)
{
    return Xil_In32(LED_BASE_ADDR + (idx * sizeof(uint32_t)));
}
