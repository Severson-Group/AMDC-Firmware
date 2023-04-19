#ifndef GPIO_DIRECT_H
#define GPIO_DIRECT_H

#include <stdint.h>
#include "xparameters.h"
#include "usr/user_config.h"


#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D

#define GPIO1_DIRECT_BASE_ADDR ()
#define GPIO2_DIRECT_BASE_ADDR ()

#endif // ifdef HARDWARE_TARGET  REV_D


#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E

#define GPIO1_DIRECT_BASE_ADDR (XPAR_HIER_GPIO_0_AMDC_GPIO_DIRECT_0_S00_AXI_BASEADDR)
#define GPIO2_DIRECT_BASE_ADDR (XPAR_HIER_GPIO_1_AMDC_GPIO_DIRECT_0_S00_AXI_BASEADDR)
#define GPIO3_DIRECT_BASE_ADDR (XPAR_HIER_GPIO_2_AMDC_GPIO_DIRECT_0_S00_AXI_BASEADDR)
#define GPIO4_DIRECT_BASE_ADDR (XPAR_HIER_GPIO_3_AMDC_GPIO_DIRECT_0_S00_AXI_BASEADDR)

#endif // ifdef HARDWARE_TARGET  REV_E

// These port and pin enumerations MUST BE the specified numbers below

typedef enum {
    GPIO_DIRECT_PORT1=0,
	GPIO_DIRECT_PORT2=1,
#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E
	GPIO_DIRECT_PORT3=2,
	GPIO_DIRECT_PORT4=3
#endif
} gpio_direct_port_t;

typedef enum {
    GPIO_DIRECT_PIN1=0,
	GPIO_DIRECT_PIN2=1,
#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E
	GPIO_DIRECT_PIN3=2
#endif
} gpio_direct_pin_t;

typedef enum {
    GPIO_DIRECT_LOW=0,
	GPIO_DIRECT_HIGH=1
} gpio_direct_level_t;


void gpio_direct_init();
gpio_direct_level_t gpio_direct_read(gpio_direct_port_t port, gpio_direct_pin_t pin);
void gpio_direct_write(gpio_direct_port_t port, gpio_direct_pin_t pin, gpio_direct_level_t level);
void gpio_direct_toggle(gpio_direct_port_t port, gpio_direct_pin_t pin);


#endif // GPIO_DIRECT_H
