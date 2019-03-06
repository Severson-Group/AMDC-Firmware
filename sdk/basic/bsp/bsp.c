#include "bsp.h"

void bsp_init(void)
{
    encoder_init();
    analog_init();
    inverter_init();
    io_init();
    gpio_init();
}
