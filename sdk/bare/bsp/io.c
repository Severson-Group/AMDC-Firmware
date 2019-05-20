#include "../bsp/io.h"
#include <stdio.h>
#include "xgpiops.h"

#define IO_LED_R_MIO_PIN	(41)
#define IO_LED_G_MIO_PIN	(43)
#define IO_LED_B_MIO_PIN	(45)

#define IO_BTN1_MIO_PIN		(47)

#define IO_SW1_MIO_PIN		(42)
#define IO_SW2_MIO_PIN		(44)

static XGpioPs Gpio;

void io_init(void)
{
	printf("IO:\tInitializing...\n");

	int Status;
	XGpioPs_Config *GPIOConfigPtr;

	// GPIO Initialization
	GPIOConfigPtr = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
	Status = XGpioPs_CfgInitialize(&Gpio, GPIOConfigPtr, GPIOConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS) {
		// Just hang here if error...
		while (1);
	}

	// Set RGB LED direction as output
	XGpioPs_SetDirectionPin(&Gpio, IO_LED_R_MIO_PIN, 1);
	XGpioPs_SetDirectionPin(&Gpio, IO_LED_G_MIO_PIN, 1);
	XGpioPs_SetDirectionPin(&Gpio, IO_LED_B_MIO_PIN, 1);

	// Set switches and button as input
	XGpioPs_SetDirectionPin(&Gpio, IO_SW1_MIO_PIN, 0);
	XGpioPs_SetDirectionPin(&Gpio, IO_SW2_MIO_PIN, 0);
	XGpioPs_SetDirectionPin(&Gpio, IO_BTN1_MIO_PIN, 0);

	// Set LED output enabled
	XGpioPs_SetOutputEnablePin(&Gpio, IO_LED_R_MIO_PIN, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, IO_LED_G_MIO_PIN, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, IO_LED_B_MIO_PIN, 1);

	// Start with RGB LED off
	XGpioPs_WritePin(&Gpio, IO_LED_R_MIO_PIN, 0);
	XGpioPs_WritePin(&Gpio, IO_LED_G_MIO_PIN, 0);
	XGpioPs_WritePin(&Gpio, IO_LED_B_MIO_PIN, 0);
}

void io_led_set(io_led_color_t *color)
{
	io_led_set_c(1, 1, 1, color);
}

void io_led_set_c(uint8_t r, uint8_t g, uint8_t b, io_led_color_t *color)
{
	uint8_t rr = color->r > 0 ? 1 : 0;
	uint8_t gg = color->g > 0 ? 1 : 0;
	uint8_t bb = color->b > 0 ? 1 : 0;

	if (r) XGpioPs_WritePin(&Gpio, IO_LED_R_MIO_PIN, rr);
	if (g) XGpioPs_WritePin(&Gpio, IO_LED_G_MIO_PIN, gg);
	if (b) XGpioPs_WritePin(&Gpio, IO_LED_B_MIO_PIN, bb);
}

void io_switch_get(uint8_t *sw1, uint8_t *sw2)
{
	*sw1 = XGpioPs_ReadPin(&Gpio, IO_SW1_MIO_PIN);
	*sw2 = XGpioPs_ReadPin(&Gpio, IO_SW2_MIO_PIN);
}

void io_button_get(uint8_t *btn1)
{
	*btn1 = XGpioPs_ReadPin(&Gpio, IO_BTN1_MIO_PIN);
}
