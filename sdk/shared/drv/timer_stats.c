#include "drv/timer_stats.h"
#include "xparameters.h"
#include "xscugic.h"
#include "xscutimer.h"

/* timer.h also uses TIMER_0_DEVICE_ID, meaning that it probably can't coexist with this timer. This should be changed (or they should be merged) */
#define TIMER_DEVICE_ID XPAR_XSCUTIMER_0_DEVICE_ID
#define TIMER_LOAD_VALUE(period_usec) ((period_usec * 333.333) - 1)

XScuTimer xTimerStats;

// Cortex A9 Scu Private Timer Instance

static void fatalError(char *str)
{
    xil_printf("TIMER ERROR: %s\n", str);
    while (1) {
    }
}

void vInitialiseTimerForRunTimeStats() {
	int Status;
	XScuTimer_Config *ConfigPtr;

	// Initialize the Scu Private Timer driver
	ConfigPtr = XScuTimer_LookupConfig(TIMER_DEVICE_ID);

	// This is where the virtual address would be used, this example uses physical address
	Status = XScuTimer_CfgInitialize(&xTimerStats, ConfigPtr, ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS) {
		fatalError("Error 1");
		return;
	}

	// Perform a self-test to ensure that the hardware was built correctly
	Status = XScuTimer_SelfTest(&xTimerStats);
	if (Status != XST_SUCCESS) {
		fatalError("Error 2");
		return;
	}

	// Enable Auto reload mode
	XScuTimer_EnableAutoReload(&xTimerStats);

	// Load the timer counter register
	XScuTimer_LoadTimer(&xTimerStats, TIMER_LOAD_VALUE(TIMER_STATS_USEC));

	// Start the timer counter
	XScuTimer_Start(&xTimerStats);
}
