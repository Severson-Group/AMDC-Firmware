/*
 * Copyright (C) 2010 - 2019 Xilinx, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 */
/*
 * platform_zynq.c
 *
 * Zynq platform specific functions.
 *
 * 02/29/2012: UART initialization is removed. Timer initializations are
 * removed. All unnecessary include files and hash defines are removed.
 * 03/01/2013: Timer initialization is added back. Support for SI #692601 is
 * added in the timer callback. The SI #692601 refers to the following issue.
 *
 * The EmacPs has a HW bug on the Rx path for heavy Rx traffic.
 * Under heavy Rx traffic because of the HW bug there are times when the Rx path
 * becomes unresponsive. The workaround for it is to check for the Rx path for
 * traffic (by reading the stats registers regularly). If the stats register
 * does not increment for sometime (proving no Rx traffic), the function resets
 * the Rx data path.
 *
 * </pre>
 */

#include "lwip/tcp.h"
#include "netif/xadapter.h"
#include "platform.h"
#include "platform_config.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xparameters_ps.h" /* defines XPAR values */
#include "xscugic.h"
#include "xscutimer.h"

#define INTC_DEVICE_ID      XPAR_SCUGIC_SINGLE_DEVICE_ID
#define TIMER_DEVICE_ID     XPAR_SCUTIMER_DEVICE_ID
#define INTC_BASE_ADDR      XPAR_SCUGIC_0_CPU_BASEADDR
#define INTC_DIST_BASE_ADDR XPAR_SCUGIC_0_DIST_BASEADDR
#define TIMER_IRPT_INTR     XPAR_SCUTIMER_INTR

#define RESET_RX_CNTR_LIMIT 400

void tcp_fasttmr(void);
void tcp_slowtmr(void);

static XScuTimer TimerInstance;

static int ResetRxCntr = 0;

extern struct netif *my_netif;

volatile int TcpFastTmrFlag = 0;
volatile int TcpSlowTmrFlag = 0;

void timer_callback(XScuTimer *TimerInstance)
{
    static int DetectEthLinkStatus = 0;

    // We need to call tcp_fasttmr & tcp_slowtmr at intervals specified
    // by lwIP. It is not important that the timing is absolutely accurate.
    static int odd = 1;

    DetectEthLinkStatus++;
    TcpFastTmrFlag = 1;

    odd = !odd;
    ResetRxCntr++;

    if (odd) {
        TcpSlowTmrFlag = 1;
    }

    /* For providing an SW alternative for the SI #692601. Under heavy
     * Rx traffic if at some point the Rx path becomes unresponsive, the
     * following API call will ensures a SW reset of the Rx path. The
     * API xemacpsif_resetrx_on_no_rxdata is called every 100 milliseconds.
     * This ensures that if the above HW bug is hit, in the worst case,
     * the Rx path cannot become unresponsive for more than 100
     * milliseconds.
     */
    if (ResetRxCntr >= RESET_RX_CNTR_LIMIT) {
        xemacpsif_resetrx_on_no_rxdata(my_netif);
        ResetRxCntr = 0;
    }

    // For detecting Ethernet phy link status periodically
    if (DetectEthLinkStatus == ETH_LINK_DETECT_INTERVAL) {
        eth_link_detect(my_netif);
        DetectEthLinkStatus = 0;
    }

    XScuTimer_ClearInterruptStatus(TimerInstance);
}

void platform_setup_timer(void)
{
    int Status = XST_SUCCESS;
    XScuTimer_Config *ConfigPtr;
    int TimerLoadValue = 0;

    ConfigPtr = XScuTimer_LookupConfig(TIMER_DEVICE_ID);
    Status = XScuTimer_CfgInitialize(&TimerInstance, ConfigPtr, ConfigPtr->BaseAddr);
    if (Status != XST_SUCCESS) {
        xil_printf("In %s: Scutimer Cfg initialization failed...\r\n", __func__);
        return;
    }

    Status = XScuTimer_SelfTest(&TimerInstance);
    if (Status != XST_SUCCESS) {
        xil_printf("In %s: Scutimer Self test failed...\r\n", __func__);
        return;
    }

    XScuTimer_EnableAutoReload(&TimerInstance);

    // Set for 250 milli seconds timeout
    TimerLoadValue = XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ / 8;

    XScuTimer_LoadTimer(&TimerInstance, TimerLoadValue);
    return;
}

void platform_setup_interrupts(void)
{
    Xil_ExceptionInit();

    XScuGic_DeviceInitialize(INTC_DEVICE_ID);

    // Connect the interrupt controller interrupt handler to the hardware
    // interrupt handling logic in the processor.
    Xil_ExceptionRegisterHandler(
        XIL_EXCEPTION_ID_IRQ_INT, (Xil_ExceptionHandler) XScuGic_DeviceInterruptHandler, (void *) INTC_DEVICE_ID);

    // Connect the device driver handler that will be called when an
    // interrupt for the device occurs, the handler defined above performs
    // the specific interrupt processing for the device.
    XScuGic_RegisterHandler(
        INTC_BASE_ADDR, TIMER_IRPT_INTR, (Xil_ExceptionHandler) timer_callback, (void *) &TimerInstance);

    // Enable the interrupt for scu timer
    XScuGic_EnableIntr(INTC_DIST_BASE_ADDR, TIMER_IRPT_INTR);

    return;
}

void platform_enable_interrupts(void)
{
    // Enable non-critical exceptions
    Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);
    XScuTimer_EnableInterrupt(&TimerInstance);
    XScuTimer_Start(&TimerInstance);

    return;
}

void init_platform(void)
{
    platform_setup_timer();
    platform_setup_interrupts();

    return;
}

void cleanup_platform(void)
{
    Xil_ICacheDisable();
    Xil_DCacheDisable();

    return;
}
