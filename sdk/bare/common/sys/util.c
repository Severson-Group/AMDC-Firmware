#include "sys/util.h"
#include "xil_exception.h"
#include <stdint.h>

uint32_t util_critical_section_enter(void)
{
    // This Xilinx driver only disables the IRQ exceptions which is fine for us.
    Xil_ExceptionDisable();

    // Return a garbage primask... This isn't used by us!
    return 0;
}

void util_critical_section_exit(uint32_t primask)
{
    // TODO(NP): We are ignoring the mask!!! Just use the Xilinx driver
    //           to return to the "normal" exception state.
    Xil_ExceptionEnable();
}
