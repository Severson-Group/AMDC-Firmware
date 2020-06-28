#include "usr/user_defines.h"

#if USER_CONFIG_HARDWARE_TARGET == 3

#include "drv/gpio.h"
#include "xgpiops.h"
#include <stdio.h>

#define GPIO_DIR_1_MIO_PIN (40)
#define GPIO_DIR_2_MIO_PIN (49)
#define GPIO_DIR_3_MIO_PIN (46)
#define GPIO_DIR_4_MIO_PIN (48)

static XGpioPs Gpio;

void gpio_init(void)
{
    printf("GPIO:\tInitializing...\n");

    int Status;
    XGpioPs_Config *GPIOConfigPtr;

    // GPIO Initialization
    GPIOConfigPtr = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
    Status = XGpioPs_CfgInitialize(&Gpio, GPIOConfigPtr, GPIOConfigPtr->BaseAddr);
    if (Status != XST_SUCCESS) {
        // Just hang here if error...
        while (1) {
        }
    }

    // Set GPIO dir pins as outputs
    XGpioPs_SetDirectionPin(&Gpio, GPIO_DIR_1_MIO_PIN, 1);
    XGpioPs_SetDirectionPin(&Gpio, GPIO_DIR_2_MIO_PIN, 1);
    XGpioPs_SetDirectionPin(&Gpio, GPIO_DIR_3_MIO_PIN, 1);
    XGpioPs_SetDirectionPin(&Gpio, GPIO_DIR_4_MIO_PIN, 1);

    // Set LED output enabled
    XGpioPs_SetOutputEnablePin(&Gpio, GPIO_DIR_1_MIO_PIN, 1);
    XGpioPs_SetOutputEnablePin(&Gpio, GPIO_DIR_2_MIO_PIN, 1);
    XGpioPs_SetOutputEnablePin(&Gpio, GPIO_DIR_3_MIO_PIN, 1);
    XGpioPs_SetOutputEnablePin(&Gpio, GPIO_DIR_4_MIO_PIN, 1);

    // Start with GPIO dir set as outputs
    gpio_set_dir(1, 1);
    gpio_set_dir(2, 1);
    gpio_set_dir(3, 1);
    gpio_set_dir(4, 1);
}

void gpio_pin_read(uint8_t port, uint8_t pin, uint8_t *value)
{
}

void gpio_pin_write(uint8_t port, uint8_t pin, uint8_t value)
{
}

void gpio_port_read(uint8_t port, uint16_t *value)
{
}

void gpio_port_write(uint8_t port, uint16_t value)
{
}

void gpio_set_dir(uint8_t group, uint8_t dir)
{
    int pin;
    if (group == 1) {
        pin = GPIO_DIR_1_MIO_PIN;
    } else if (group == 2) {
        pin = GPIO_DIR_2_MIO_PIN;
    } else if (group == 3) {
        pin = GPIO_DIR_3_MIO_PIN;
    } else if (group == 4) {
        pin = GPIO_DIR_4_MIO_PIN;
    } else {
        // ERROR
    }

    XGpioPs_WritePin(&Gpio, pin, dir);
}

#endif // USER_CONFIG_HARDWARE_TARGET
