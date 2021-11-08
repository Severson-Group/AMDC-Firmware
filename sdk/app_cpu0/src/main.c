#include "xparameters.h"
#include "netif/xadapter.h"
#include "platform.h"
#include "platform_config.h"
#include "xil_printf.h"
#include "xstatus.h"
#include "xil_cache.h"
#include "xil_mmu.h"
#include "xil_io.h"
#include <stdio.h>
#include "lwip_glue.h"

/* defined by each RAW mode application */
int start_application();
int transfer_data();
void tcp_fasttmr(void);
void tcp_slowtmr(void);

extern volatile int TcpFastTmrFlag;
extern volatile int TcpSlowTmrFlag;

int main()
{
	init_platform();

    // Disable cache on OCM
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

    // Set up LwIP stuff
	print("Setting up LwIP... ");
    if (setup_lwip() == XST_SUCCESS) {
		print("Success!\r\n");
    } else {
		print("Failure\r\n");
    	print("Hanging program...\r\n");
    	while(1);
    }

	print("Setting up listening sockets... ");
	if (start_tcpip() == XST_SUCCESS) {
		print("Success!\r\n");
	} else {
		print("Failure\r\n");
    	print("Hanging program...\r\n");
    	while(1);
	}

	// Receive and process packets
	while (1) {
		xemacif_input(echo_netif);

		if (TcpFastTmrFlag) {
			tcp_fasttmr();
			TcpFastTmrFlag = 0;
		}
		if (TcpSlowTmrFlag) {
			tcp_slowtmr();
			TcpSlowTmrFlag = 0;
		}
	}

	// Never reached
	cleanup_platform();

	return 0;
}
