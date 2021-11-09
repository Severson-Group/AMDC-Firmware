#include "icc_rx.h"
#include "icc.h"
#include "socket_manager.h"

void icc_rx_process(void)
{
    // Check if CPU1 has written data
    if (ICC_CPU1to0__GET_CPU1_HasWrittenData) {
        // Clear CPU1's HasWrittenData flag
        ICC_CPU1to0__CLR_CPU1_HasWrittenData;

        // Read one byte from ICC
        uint32_t d = ICC_CPU1to0__GET_DATA;
        socket_manager_broadcast_ascii_cmd_byte(d);

        // Tell CPU1 that we just read data
        ICC_CPU1to0__SET_CPU0_WaitingForData;
    }
}
