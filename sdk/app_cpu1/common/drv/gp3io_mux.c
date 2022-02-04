#include "usr/user_config.h"
#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E

#include "drv/gp3io_mux.h"
#include "xil_io.h"
#include <stdint.h>
#include <stdio.h>

void gp3io_mux_init(void)
{
    printf("GP3IO LINES:\tInitializing...\n");

    // Default all GP3IO muxes to unused, i.e. disconnected
    gp3io_mux_set_device(GP3IO_MUX_1_BASE_ADDR, GP3IO_MUX_UNUSED);
    gp3io_mux_set_device(GP3IO_MUX_2_BASE_ADDR, GP3IO_MUX_UNUSED);
    gp3io_mux_set_device(GP3IO_MUX_3_BASE_ADDR, GP3IO_MUX_UNUSED);
    gp3io_mux_set_device(GP3IO_MUX_4_BASE_ADDR, GP3IO_MUX_UNUSED);
}

void gp3io_mux_set_device(uint32_t base_addr, gp3io_mux_device_t device)
{
    Xil_Out32(base_addr, device);
}

#endif // USER_CONFIG_HARDWARE_TARGET
