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
#include "sys/icc.h"
#include "sys/intr.h"

/* Begin User Includes */
#include "drv/led.h"
/* End User Includes */

#define TIMER_ID              1
#define DELAY_10_SECONDS      10000UL
#define DELAY_1_SECOND        1000UL
#define TIMER_CHECK_THRESHOLD 9
/*-----------------------------------------------------------*/

/* The Tx and Rx tasks as described at the top of this file. */
static void prvTxTask(void *pvParameters);
static void prvRxTask(void *pvParameters);
static void prvBlinkyTask(void *pvParameters);
static void vTimerCallback(TimerHandle_t pxTimer);
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

static TaskHandle_t xTxTaskHandle;
static TaskHandle_t xRxTaskHandle;
static TaskHandle_t xBlinkyTaskHandle;
static QueueHandle_t xQueue = NULL;
static TimerHandle_t xTimer = NULL;

char HWstring[32] = "CPU0 - Hello World";
long RxtaskCntr = 0;

uint8_t message_status = 0;
// 0 - sending messages
// 1 - complete, success
// 2 - complete, failure

int main(void)
{
    // Both CPUs: Disable cache on OCM
    // S=b1 TEX=b100 AP=b11, Domain=b1111, C=b0, B=b0
    Xil_SetTlbAttributes(0xFFFF0000, 0x14de2);

#if 1
    // CPU0 ONLY:
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

    intr_init();
    icc_init();
    vPortInstallFreeRTOSVectorTable();

    ///////////////////////////
    // BEGIN USER CODE HERE //
    /////////////////////////

    led_init();

    const TickType_t x10seconds = pdMS_TO_TICKS(DELAY_10_SECONDS);

    /* Create the three tasks */
    xTaskCreate(prvTxTask,                /* The function that implements the task. */
                (const char *) "CPU0_Tx", /* Text name for the task, provided to assist debugging only. */
                configMINIMAL_STACK_SIZE, /* The stack allocated to the task. */
                NULL,                     /* The task parameter is not used, so set to NULL. */
                tskIDLE_PRIORITY,
                &xTxTaskHandle);

    xTaskCreate(prvRxTask,                /* The function that implements the task. */
                (const char *) "CPU0_Rx", /* Text name for the task, provided to assist debugging only. */
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 1,
                &xRxTaskHandle);

    // Create additional blinky task - CPU0 only
    xTaskCreate(prvBlinkyTask,
                (const char *) "CPU0_Blinky",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY,
                &xBlinkyTaskHandle);

    /* Create the queue used by the tasks.  The Rx task has a higher priority
    than the Tx task, so will preempt the Tx task and remove values from the
    queue as soon as the Tx task writes to the queue - therefore the queue can
    never have more than one item in it. */
    xQueue = xQueueCreate(1, sizeof(HWstring));

    /* Check the queue was created. */
    configASSERT(xQueue);

    /* Create a timer with a timer expiry of 10 seconds. The timer would expire
     after 10 seconds and the timer call back would get called. In the timer call back
     checks are done to ensure that the tasks have been running properly till then.
     The tasks are deleted in the timer call back and a message is printed to convey that
     the example has run successfully.
     The timer expiry is set to 10 seconds and the timer set to not auto reload. */
    xTimer = xTimerCreate((const char *) "CPU0_Timer", x10seconds, pdFALSE, (void *) TIMER_ID, vTimerCallback);

    /* Check the timer was created. */
    configASSERT(xTimer);

    /* start the timer with a block time of 0 ticks. This means as soon
       as the schedule starts the timer will start running and will expire after
       10 seconds */
    xTimerStart(xTimer, 0);

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
    }
}

/*-----------------------------------------------------------*/
static void prvTxTask(void *pvParameters)
{
    const TickType_t x1second = pdMS_TO_TICKS(DELAY_1_SECOND);

    for (;;) {
        if (message_status > 0) {
            // Cannot delete tasks created using heap_1 implementation, so instead we suspend immediately if done
            vTaskSuspend(NULL);
        } else {
            /* Delay for 1 second. */
            vTaskDelay(x1second);

            /* Send the next value on the queue.  The queue should always be
            empty at this point so a block time of 0 is used. */
            //  xQueueSend(xQueue,   /* The queue being written to. */
            //             HWstring, /* The address of the data being sent. */
            //             0UL);     /* The block time. */

            //xil_printf("DEBUG: CPU0 about to attempt send\r\n");
            xil_printf("A\r\n");

            // Send a message to the other core
            size_t bytes_sent = xMessageBufferSend(xCPU0to1MessageBufferHandle, HWstring, sizeof(HWstring), 0UL);

            //xil_printf("DEBUG: CPU0 sent %d bytes to ICC buffer\r\n", bytes_sent);
            xil_printf("B\r\n");

            if (bytes_sent == 0) {
                //xil_printf("ERROR: CPU0 failed to write to ICC buffer\r\n");
                xil_printf("C\r\n");
            }
        }
    }
}

/*-----------------------------------------------------------*/
static void prvRxTask(void *pvParameters)
{
    char Rcvdstring[32] = "";

    for (;;) {
        if (message_status > 0) {
            // Cannot delete tasks created using heap_1 implementation, so instead we suspend immediately if done
            vTaskSuspend(NULL);
        } else {

            //  /* Block to wait for data arriving on the queue. */
            //  xQueueReceive(xQueue,         /* The queue being read. */
            //                Rcvdstring,     /* Data is read into this address. */
            //                portMAX_DELAY); /* Wait without a timeout for data. */

            //xil_printf("DEBUG: CPU0 about to attempt rcv\r\n");
        	xil_printf("D\r\n");

            size_t bytes_rcvd = xMessageBufferReceive(xCPU1to0MessageBufferHandle, Rcvdstring, 32, portMAX_DELAY);

            //xil_printf("DEBUG: CPU0 rcvd %d bytes from ICC buffer\r\n", bytes_rcvd);
            xil_printf("E\r\n");

            if (bytes_rcvd == 0) {
                //xil_printf("CPU0 failed to receive from ICC buffer\r\n");
            	xil_printf("F\r\n");
            } else {
                /* Print the received data. */
                //xil_printf("CPU0 - Rx task received string from CPU1 Tx: %s\r\n", Rcvdstring);
            	xil_printf("G\r\n");
                RxtaskCntr++;
            }
        }
    }
}

/*-----------------------------------------------------------*/
static void prvBlinkyTask(void *pvParameters)
{
    const TickType_t x250ms = pdMS_TO_TICKS(DELAY_1_SECOND / 4);
    uint8_t led_offset = 0;

    for (;;) {
        if (message_status == 0) {
            // If not complete, cycle yellow every 250ms
            vTaskDelay(x250ms);

            led_set_color(0 + led_offset, LED_COLOR_YELLOW);
            led_set_color(1 + led_offset, LED_COLOR_BLACK);
            led_set_color(2 + led_offset, LED_COLOR_BLACK);
            led_set_color(3 + led_offset, LED_COLOR_BLACK);

            led_offset = (led_offset + 1) % 4;
        } else if (message_status == 1) {
            // If complete, flash all green every 250ms
            vTaskDelay(x250ms);

            led_set_color(LED0, LED_COLOR_BLACK);
            led_set_color(LED1, LED_COLOR_BLACK);
            led_set_color(LED2, LED_COLOR_BLACK);
            led_set_color(LED3, LED_COLOR_BLACK);

            vTaskDelay(x250ms);

            led_set_color(LED0, LED_COLOR_GREEN);
            led_set_color(LED1, LED_COLOR_GREEN);
            led_set_color(LED2, LED_COLOR_GREEN);
            led_set_color(LED3, LED_COLOR_GREEN);
        } else {
            // message_status must be 2, meaning failure
            // flash all red every 250ms
            vTaskDelay(x250ms);

            led_set_color(LED0, LED_COLOR_BLACK);
            led_set_color(LED1, LED_COLOR_BLACK);
            led_set_color(LED2, LED_COLOR_BLACK);
            led_set_color(LED3, LED_COLOR_BLACK);

            vTaskDelay(x250ms);

            led_set_color(LED0, LED_COLOR_RED);
            led_set_color(LED1, LED_COLOR_RED);
            led_set_color(LED2, LED_COLOR_RED);
            led_set_color(LED3, LED_COLOR_RED);
        }
    }
}

/*-----------------------------------------------------------*/
static void vTimerCallback(TimerHandle_t pxTimer)
{
    long lTimerId;
    configASSERT(pxTimer);

    lTimerId = (long) pvTimerGetTimerID(pxTimer);

    if (lTimerId != TIMER_ID) {
        //xil_printf("CPU0 - FreeRTOS Hello World Example FAILED");
    	xil_printf("H\r\n");
    }

    /* If the RxtaskCntr is updated every time the Rx task is called. The
     Rx task is called every time the Tx task sends a message. The Tx task
     sends a message every 1 second.
     The timer expires after 10 seconds. We expect the RxtaskCntr to at least
     have a value of 9 (TIMER_CHECK_THRESHOLD) when the timer expires. */
    if (RxtaskCntr >= TIMER_CHECK_THRESHOLD) {
        message_status = 1;
        //xil_printf("CPU0 - FreeRTOS Hello World Example PASSED\r\n");
        xil_printf("I\r\n");
    } else {
        message_status = 2;
        //xil_printf("CPU0 - FreeRTOS Hello World Example FAILED\r\n");
        xil_printf("J\r\n");
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
