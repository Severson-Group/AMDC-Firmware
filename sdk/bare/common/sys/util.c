#include "sys/util.h"
#include "xil_exception.h"
#include "xpseudo_asm.h"
#include <stdint.h>

uint32_t util_critical_section_enter(void)
{
    uint32_t currmask;

    // Get current mask so user can return it to this later
    currmask = mfcpsr();

    // Disable all exceptions
    mtcpsr(XIL_EXCEPTION_ALL);

    return currmask;
}

void util_critical_section_exit(uint32_t primask)
{
    // Restore the mask... Hopefully the user passed back in
    // the returned mask from the `enter` function!
    mtcpsr(primask);
}
