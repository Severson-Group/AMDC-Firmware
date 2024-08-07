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
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
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
#include "drv/pwm.h"
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

/* needed for FreeRTOS-Plus-TCP */

static UBaseType_t ulNextRand;

UBaseType_t uxRand(void) {
    const uint32_t ulMultiplier = 0x015a4e35UL, ulIncrement = 1UL;
    /* Utility function to generate a pseudo random number. */
    ulNextRand = (ulMultiplier * ulNextRand) + ulIncrement;
    return((int) (ulNextRand) & 0x7fffUL );
}

BaseType_t xApplicationGetRandomNumber(uint32_t *pulNumber) {
    *pulNumber = (uint32_t) uxRand();
    return pdTRUE;
}

uint32_t ulApplicationGetNextSequenceNumber(uint32_t ulSourceAddress, uint16_t usSourcePort, uint32_t ulDestinationAddress, uint16_t usDestinationPort) {
    return ( uint32_t ) uxRand();
}

void vApplicationIPNetworkEventHook(eIPCallbackEvent_t eNetworkEvent) {
    static BaseType_t xTasksAlreadyCreated = pdFALSE;
    /* If the network has just come up...*/
    if ((eNetworkEvent == eNetworkUp) && (xTasksAlreadyCreated == pdFALSE)) {
        /* Do nothing. Just a stub. */
        xTasksAlreadyCreated = pdTRUE;
    }
}

/* Default MAC address configuration.  The demo creates a virtual network
 * connection that uses this MAC address by accessing the raw Ethernet data
 * to and from a real network connection on the host PC.  See the
 * configNETWORK_INTERFACE_TO_USE definition for information on how to configure
 * the real network connection to use. */
const uint8_t ucMACAddress[6] =
{
    configMAC_ADDR0,
    configMAC_ADDR1,
    configMAC_ADDR2,
    configMAC_ADDR3,
    configMAC_ADDR4,
    configMAC_ADDR5
};

/* The default IP and MAC address used by the code. It is used as a place holder.
 */
static const uint8_t ucIPAddress[4] = {
    configIP_ADDR0,
    configIP_ADDR1,
    configIP_ADDR2,
    configIP_ADDR3
};
static const uint8_t ucNetMask[4] = {
    configNET_MASK0,
    configNET_MASK1,
    configNET_MASK2,
    configNET_MASK3
};
static const uint8_t ucGatewayAddress[4] = {
    configGATEWAY_ADDR0,
    configGATEWAY_ADDR1,
    configGATEWAY_ADDR2,
    configGATEWAY_ADDR3
};
static const uint8_t ucDNSServerAddress[4] = {
    configDNS_SERVER_ADDR0,
    configDNS_SERVER_ADDR1,
    configDNS_SERVER_ADDR2,
    configDNS_SERVER_ADDR3
};

extern void vStartSimpleTCPServerTasks( uint16_t usStackSize,
        UBaseType_t uxPriority );

uint8_t ucHeap[ configTOTAL_HEAP_SIZE ]; // the heap.

int main(void)
{
	/* initialise hardware */
	led_init();
	uart_init();
	pwm_init();
	serial_init();
	commands_init();
	init_platform();
    // Both CPUs: Disable cache on OCM
    // S=b1 TEX=b100 AP=b11, Domain=b1111, C=b0, B=b0
    Xil_SetTlbAttributes(0xFFFF0000, 0x14de2);

#if 1
    // This code is required to start CPU1 from CPU0 during boot.
    //
    // This only applies when booting from flash via the FSBL.
    // During development with JTAG loading, these low-level
    // calls in this #if block are not needed! However, we'll
    // keep them here since it doesn't affect performance...

    // Write starting base address for CPU1 PC.
    // It will look for this address upon waking up
    static const uintptr_t CPU1_START_ADDR = 0xFFFFFFF0;
    static const uint32_t CPU1_BASE_ADDR = 0x20080000;
    Xil_Out32(CPU1_START_ADDR, CPU1_BASE_ADDR);

    // Waits until write has finished
    // DMB = Data Memory Barrier
    dmb();

    // Wake up CPU1 by sending the SEV command
    // SEV = Set Event, which causes CPU1 to wake up and jump to CPU1_BASE_ADDR
    __asm__("sev");
#endif

    Xil_ExceptionInit();
    vPortInstallFreeRTOSVectorTable();

    ///////////////////////////
    // BEGIN USER CODE HERE //
    /////////////////////////

    FreeRTOS_printf(("FreeRTOS_IPInit\n"));
	FreeRTOS_IPInit(ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddress);

	commands_start_msg();

    /* command sets */
    cmd_counter_register();
    cmd_hw_register();

    /* user apps */
    user_apps_init();

    xil_printf("CPU0 - Hello, World!!!\r\n");
//    xil_printf("1 millisecond is %d ticks\n", pdMS_TO_TICKS(1));
//    xil_printf("0.1 milliseconds is %d ticks\n", pdMS_TO_TICKS(0.1));
//
//    xil_printf("10 ticks is %d milliseconds\n", pdTICKS_TO_MS(10));
//    xil_printf("1 ticks is %d microseconds\n", (int) (pdTICKS_TO_MS(1) * 1000));

    /////////////////////////
    // END USER CODE HERE //
    ///////////////////////

    vStartSimpleTCPServerTasks(1024, tskIDLE_PRIORITY);

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
