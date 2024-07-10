/*
    Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
    Copyright (C) 2012 - 2018 Xilinx, Inc. All Rights Reserved.

    Permission is hereby granted, free of charge, to any person obtaining a copy of
    this software and associated documentation files (the "Software"), to deal in
    the Software without restriction, including without limitation the rights to
    use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
    the Software, and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software. If you wish to use our Amazon
    FreeRTOS name, please do so in a fair use way that does not cause confusion.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
    FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
    COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
    IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
    CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

    http://www.FreeRTOS.org
    http://aws.amazon.com/freertos


    1 tab == 4 spaces!
*/

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"
/* Xilinx includes. */
#include "platform.h"
#include "xil_cache.h"
#include "xil_io.h"
#include "xil_mmu.h"
#include "xil_printf.h"
#include "xparameters.h"
/* Firmware includes */
//#include "sys/icc.h"
#include "sys/intr.h"

/* Begin User Includes */
#include "drv/led.h"
#include "drv/uart.h"
#include "sys/serial.h"
#include "sys/commands.h"
#include "sys/cmd/cmd_counter.h"
#include "sys/cmd/cmd_hw.h"
#include "usr/user_apps.h"
/* End User Includes */

#define DELAY_1_SECOND        1000UL
#define INTC_HANDLER          XScuGic_InterruptHandler
/*-----------------------------------------------------------*/

/* This project has configSUPPORT_STATIC_ALLOCATION set to 1 (for Inter-Core Communication) so
 * the following application callback function must be provided to supply the RAM that will
 * get used for the Idle task data structures and stack.
 */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize);

/* This project has configSUPPORT_STATIC_ALLOCATION set to 1 (for Inter-Core Communication)
 * and configUSE_TIMERS set to 1 so the following application callback function must be
 * provided to supply the RAM that will get used for the Timer task data structures and stack.
 */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize);

/*
 * The Xilinx projects use a BSP that do not allow the start up code to be
 * altered easily. Therefore the vector table used by FreeRTOS is defined in
 * FreeRTOS_asm_vectors.S, which is part of this project. Switch to use the
 * FreeRTOS vector table.
 */
extern void vPortInstallFreeRTOSVectorTable(void);

/*-----------------------------------------------------------*/

/* Parameters for the queue used by the Tx and Rx tasks, as described at the top of this file. */

#define QUEUE_LENGTH 10
#define ITEM_SIZE    sizeof(uint32_t)

int main(void)
{
    // Both CPUs: Disable cache on OCM
    // S=b1 TEX=b100 AP=b11, Domain=b1111, C=b0, B=b0
    Xil_SetTlbAttributes(0xFFFF0000, 0x14de2);

    Xil_ExceptionInit();
    intr_init();
    icc_init();
    vPortInstallFreeRTOSVectorTable();

    ///////////////////////////
    // BEGIN USER CODE HERE //
    /////////////////////////

    /* initialise hardware */

    led_init();
    uart_init();
    pwm_init();
    serial_init();
    commands_init();

    /* command sets */
    cmd_counter_register();
    cmd_hw_register();

    /* user apps */
    user_apps_init();

    xil_printf("CPU0 - freertos game!\r\n");
//    xil_printf("1 millisecond is %d ticks\n", pdMS_TO_TICKS(1));
//    xil_printf("0.1 milliseconds is %d ticks\n", pdMS_TO_TICKS(0.1));
//
//    xil_printf("10 ticks is %d milliseconds\n", pdTICKS_TO_MS(10));
//    xil_printf("1 ticks is %d microseconds\n", (int) (pdTICKS_TO_MS(1) * 1000));

    /////////////////////////
    // END USER CODE HERE //
    ///////////////////////

    /* Start the tasks and timer running. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for (;;) {
    	xil_printf("IF YOU'RE READING THIS THEN A TERRIBLE ERROR HAS OCCURRED!\n");
    }
}

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
 * used by the Idle task. */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
    /* If the buffers to be provided to the Idle task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
     * state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
 * application must provide an implementation of vApplicationGetTimerTaskMemory()
 * to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize)
{
    /* If the buffers to be provided to the Timer task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
     * task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

/*******************************************************************************/
/*              S E T U P   I N T E R R U P T   S Y S T E M                    */
/*******************************************************************************/
int SetupInterruptSystem(XScuGic *IntcInstancePtr) {
	int Result;

	XScuGic_Config *IntcConfig;

	// Initialize the interrupt controller driver so that it is ready to use.

	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);//ELS -- this is misleading! there is only one XScuGic device (0)
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Result = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
			IntcConfig->CpuBaseAddress);
	if (Result != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Initialize the exception table and register the interrupt
	// controller handler with the exception table

	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler) INTC_HANDLER, IntcInstancePtr);

	// Enable non-critical exceptions

	Xil_ExceptionEnable();

	return XST_SUCCESS;
}
