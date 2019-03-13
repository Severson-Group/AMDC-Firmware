#include "bsp.h"

void bsp_init(void)
{
    encoder_init();
    analog_init();
    pwm_init();
    io_init();
    gpio_init();
}
