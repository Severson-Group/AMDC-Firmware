#include "drv/gpio_direct.h"
#include "usr/user_config.h"
#include "xil_io.h"
#include <stdint.h>

/* GPIO DIRECT REGISTERS

AMDC REV E:

Ports are 1-4
Pins are 1-3 (both IN and OUT)

Each GPIO port has two registers, an IN and an OUT.

Reg 0 (BASE_ADDR) is the IN register, and Reg 1 (BASE_ADDR + 4) is the OUT register.

For each register, the 3 least-significant bits are pin 1, pin 2, and pin 3.



AMDC REV D:

Ports are 1-2
Pins are 1-2 (both IN and OUT)

Each GPIO port has two registers, an IN and an OUT.

Reg 0 (BASE_ADDR) is the IN register, and Reg 1 (BASE_ADDR + 4) is the OUT register.

For each register, the 2 least-significant bits are pin 1 and pin 2.



Note that for each GPIO port, the IN and OUT pins are different electric connections on the GPIO port.
i.e. Port 1 has both pin 1 IN and pin 1 OUT.

*/

void gpio_direct_init()
{
    // Set all WRITE pins to LOW
    gpio_direct_write(GPIO_DIRECT_PORT1, GPIO_DIRECT_PIN1, GPIO_DIRECT_LOW);
    gpio_direct_write(GPIO_DIRECT_PORT1, GPIO_DIRECT_PIN2, GPIO_DIRECT_LOW);
    gpio_direct_write(GPIO_DIRECT_PORT2, GPIO_DIRECT_PIN1, GPIO_DIRECT_LOW);
    gpio_direct_write(GPIO_DIRECT_PORT2, GPIO_DIRECT_PIN2, GPIO_DIRECT_LOW);
#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E
    gpio_direct_write(GPIO_DIRECT_PORT1, GPIO_DIRECT_PIN3, GPIO_DIRECT_LOW);
    gpio_direct_write(GPIO_DIRECT_PORT2, GPIO_DIRECT_PIN3, GPIO_DIRECT_LOW);
    gpio_direct_write(GPIO_DIRECT_PORT3, GPIO_DIRECT_PIN1, GPIO_DIRECT_LOW);
    gpio_direct_write(GPIO_DIRECT_PORT3, GPIO_DIRECT_PIN2, GPIO_DIRECT_LOW);
    gpio_direct_write(GPIO_DIRECT_PORT3, GPIO_DIRECT_PIN3, GPIO_DIRECT_LOW);
    gpio_direct_write(GPIO_DIRECT_PORT4, GPIO_DIRECT_PIN1, GPIO_DIRECT_LOW);
    gpio_direct_write(GPIO_DIRECT_PORT4, GPIO_DIRECT_PIN2, GPIO_DIRECT_LOW);
    gpio_direct_write(GPIO_DIRECT_PORT4, GPIO_DIRECT_PIN3, GPIO_DIRECT_LOW);
#endif
}

gpio_direct_level_t gpio_direct_read(gpio_direct_port_t port, gpio_direct_pin_t pin)
{
    uint32_t base_addr = 0;

    switch (port) {
    case GPIO_DIRECT_PORT1:
        base_addr = GPIO1_DIRECT_BASE_ADDR;
        break;
    case GPIO_DIRECT_PORT2:
        base_addr = GPIO2_DIRECT_BASE_ADDR;
        break;
#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E
    case GPIO_DIRECT_PORT3:
        base_addr = GPIO3_DIRECT_BASE_ADDR;
        break;
    case GPIO_DIRECT_PORT4:
        base_addr = GPIO4_DIRECT_BASE_ADDR;
        break;
#endif
    }

    if (Xil_In32(base_addr) & (1 << pin))
        return GPIO_DIRECT_HIGH;
    else
        return GPIO_DIRECT_LOW;
}

void gpio_direct_write(gpio_direct_port_t port, gpio_direct_pin_t pin, gpio_direct_level_t level)
{
    // Get current register value using Xil_In,
    // then set the correct bit if write level is HIGH or clear the correct bit if write level is LOW
    uint32_t current = 0;
    uint32_t base_addr = 0;

    switch (port) {
    case GPIO_DIRECT_PORT1:
        base_addr = GPIO1_DIRECT_BASE_ADDR;
        break;
    case GPIO_DIRECT_PORT2:
        base_addr = GPIO2_DIRECT_BASE_ADDR;
        break;
#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E
    case GPIO_DIRECT_PORT3:
        base_addr = GPIO3_DIRECT_BASE_ADDR;
        break;
    case GPIO_DIRECT_PORT4:
        base_addr = GPIO4_DIRECT_BASE_ADDR;
        break;
#endif
    }

    // Read the WRITE register, change only desired bit, write back out below
    // The desired port's WRITE register is the second register in the IP block
    // Therefore it is at the port's base_addr + sizeof(uint32_t)
    current = Xil_In32(base_addr + sizeof(uint32_t));

    if (level) { 
        // SET
        Xil_Out32(base_addr + sizeof(uint32_t), current | (1 << pin));
    } else { 
        // CLEAR
        Xil_Out32(base_addr + sizeof(uint32_t), current & ~(1 << pin));
    }
}

void gpio_direct_toggle(gpio_direct_port_t port, gpio_direct_pin_t pin)
{
    uint32_t base_addr = 0;

    switch (port) {
    case GPIO_DIRECT_PORT1:
        base_addr = GPIO1_DIRECT_BASE_ADDR;
        break;
    case GPIO_DIRECT_PORT2:
        base_addr = GPIO2_DIRECT_BASE_ADDR;
        break;
#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E
    case GPIO_DIRECT_PORT3:
        base_addr = GPIO3_DIRECT_BASE_ADDR;
        break;
    case GPIO_DIRECT_PORT4:
        base_addr = GPIO4_DIRECT_BASE_ADDR;
        break;
#endif
    }

    // Get current register value using Xil_In, then toggle the desired bit
    Xil_Out32(base_addr + sizeof(uint32_t), Xil_In32(base_addr + sizeof(uint32_t)) ^ (1 << pin));
}
