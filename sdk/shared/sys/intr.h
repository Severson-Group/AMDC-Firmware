#ifndef INTR_H
#define INTR_H

#include "FreeRTOS.h"
#include "shared_memory.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xscugic.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

///////////////////////////////////////////////////////
// THIS IS A SHARED FILE, SO IT IS ALWAYS
// IN SYNC IN BOTH CPU0 AND CPU1
//
// If you need to differentiate something between
// CPUs, use "#if XPAR_CPU_ID == ?"
///////////////////////////////////////////////////////

#define INTR_GIC_DEVICE_ID          XPAR_PS7_SCUGIC_0_DEVICE_ID     // good

int intr_init();

// We only need to define the handlers in the appropriate core
#if XPAR_CPU_ID == 0
void CPU0UnblockRxHandler();
void CPU0UnblockTxHandler();
#elif XPAR_CPU_ID == 1
void CPU1UnblockRxHandler();
void CPU1UnblockTxHandler();
#endif

#endif /* INTR_H */
