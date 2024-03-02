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

#define CPU0_ID (XSCUGIC_SPI_CPU0_MASK << 0)
#define CPU1_ID (XSCUGIC_SPI_CPU0_MASK << 1)

#define INTR_GIC_DEVICE_ID          XPAR_PS7_SCUGIC_0_DEVICE_ID     // good

#define INTC_0TO1_SEND_INTERRUPT_ID 0U
#define INTC_1TO0_RCVE_INTERRUPT_ID 1U
#define INTC_1TO0_SEND_INTERRUPT_ID 2U
#define INTC_0TO1_RCVE_INTERRUPT_ID 3U


int intr_init();

/* We only need to define the handlers in the appropriate core
 */
#if XPAR_CPU_ID == 0
void CPU0WakeTxHandler();
void CPU0WakeRxHandler();
#elif XPAR_CPU_ID == 1
void CPU1WakeTxHandler();
void CPU1WakeRxHandler();
#endif

#endif /* INTR_H */
