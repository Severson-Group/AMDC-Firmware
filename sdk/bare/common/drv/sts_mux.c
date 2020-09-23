#include "drv/hardware_targets.h"
#include "usr/user_config.h"

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D

#include "drv/sts_mux.h"
#include "xil_io.h"
#include <stdint.h>
#include <stdio.h>

#define STS_MUX_BASE_ADDR (0x43C50000)

static int ports[] = {
    INVERTER_PORT1, INVERTER_PORT2, INVERTER_PORT3, INVERTER_PORT4,
    INVERTER_PORT5, INVERTER_PORT6, INVERTER_PORT7, INVERTER_PORT8,
};

void sts_mux_init(void)
{
    printf("STATUS LINES:\tInitializing...\n");

    // Set lines A, B, and C as outputs and D as input (CS, SCLK, MOSI, and MISO)
    sts_mux_set_output(STS_MUX_A, true);
    sts_mux_set_output(STS_MUX_B, false);
    sts_mux_set_output(STS_MUX_C, false);
    sts_mux_set_output(STS_MUX_D, false);

    for (int i = 0; i < 8; i++) {
        if (ports[i]) {
            // Map the port status lines to the corresponding device defined in the header file
            sts_mux_set_device(i, ports[i]);
        }
    }
}

void sts_mux_set_line(uint8_t port, sts_mux_device_t device, sts_mux_line_t line)
{
    Xil_Out32(STS_MUX_BASE_ADDR + ((port + (line * 8)) * sizeof(uint32_t)), device);
}

void sts_mux_set_output(sts_mux_line_t line, bool output)
{
    int dir = output ? 1 : 0;
    Xil_Out32(STS_MUX_BASE_ADDR + ((32 + line) * sizeof(uint32_t)), dir);
}

void sts_mux_set_device(uint8_t port, sts_mux_device_t device)
{
    sts_mux_set_line(port, device, STS_MUX_A);
    sts_mux_set_line(port, device, STS_MUX_B);
    sts_mux_set_line(port, device, STS_MUX_C);
    sts_mux_set_line(port, device, STS_MUX_D);
}

#endif // USER_CONFIG_HARDWARE_TARGET
