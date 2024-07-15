#include "drv/watchdog.h"
#include "xparameters.h"
#include "xscuwdt.h"

// Private watchdog defines
#define WDT_DEVICE_ID  (XPAR_SCUWDT_0_DEVICE_ID)
#define INTC_DEVICE_ID (XPAR_SCUGIC_SINGLE_DEVICE_ID)
#define WDT_IRPT_INTR  (XPAR_SCUWDT_INTR)
#define WDT_LOAD_VALUE (0xFFFFFFFF) // seems to be about 10 seconds

XScuWdt WdtInstance;

void watchdog_init(void)
{
    int Status;
    XScuWdt_Config *ConfigPtr;

    // Initialize the SCU Private Wdt driver so that it is ready to use.
    ConfigPtr = XScuWdt_LookupConfig(WDT_DEVICE_ID);
    Status = XScuWdt_CfgInitialize(&WdtInstance, ConfigPtr, ConfigPtr->BaseAddr);
    if (Status != XST_SUCCESS) {
        // Hang if error...
        while (1) {
        }
    }

    // Put the watchdog timer in watchdog mode.
    XScuWdt_SetWdMode(&WdtInstance);

    // Load the watchdog counter register.
    XScuWdt_LoadWdt(&WdtInstance, WDT_LOAD_VALUE);

    // Start the ScuWdt device.
    XScuWdt_Start(&WdtInstance);
}

void watchdog_reset(void)
{
    XScuWdt_RestartWdt(&WdtInstance);
}
