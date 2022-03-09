#include "drv/fpga_timer.h"
#include "sys/defines.h"
#include "usr/user_config.h"
#include "xparameters.h"
#include "xtmrctr.h"
#include <stdio.h>

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D
#define TMR_DEVICE_ID XPAR_CONTROL_TIMER_1_DEVICE_ID
#endif

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E
#define TMR_DEVICE_ID XPAR_HIER_TIMERS_CONTROL_TIMER_1_DEVICE_ID
#endif

static XTmrCtr timer;

void fpga_timer_init(void)
{
    int xStatus = XTmrCtr_Initialize(&timer, TMR_DEVICE_ID);
    if (xStatus != XST_SUCCESS) {
        HANG;
    }

    // Configure timer to count up and auto-restart when it hits max value
    XTmrCtr_SetOptions(&timer, 0, XTC_AUTO_RELOAD_OPTION);

    // Reset timer to value of 0
    XTmrCtr_SetResetValue(&timer, 0, 0);

    XTmrCtr_Start(&timer, 0);
}

uint32_t fpga_timer_now(void)
{
    return XTmrCtr_GetValue(&timer, 0);
}

double fpga_timer_ticks_to_usec(uint32_t ticks)
{
    // FPGA clock is 200MHz on AMDC
    double usec = (double) ticks / 200;
    return usec;
}

double fpga_timer_ticks_to_sec(uint32_t ticks)
{
    // FPGA clock is 200MHz on AMDC
    double sec = (double) ticks / 200e6;
    return sec;
}
