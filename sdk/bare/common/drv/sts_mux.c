#include "drv/hardware_targets.h"
#include "usr/user_config.h"

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D

#include "drv/sts_mux.h"
#include "xil_io.h"
#include <stdint.h>
#include <stdio.h>

#define STS_MUX_BASE_ADDR (0x43C50000)

static int ports[] = {
		INVERTER_PORT1,
		INVERTER_PORT2,
		INVERTER_PORT3,
		INVERTER_PORT4,
		INVERTER_PORT5,
		INVERTER_PORT6,
		INVERTER_PORT7,
		INVERTER_PORT8,
};

void sts_mux_init(void)
{
    printf("STATUS LINES:\tInitializing...\n");

    Xil_Out32(STS_MUX_BASE_ADDR + (35 * sizeof(uint32_t)), 0xF);

    for(int i = 0; i < 8; i++) {
    	if(ports[i]) {
    		sts_mux_set_line(i,ports[i],STS_MUX_A);
    		sts_mux_set_line(i,ports[i],STS_MUX_B);
    		sts_mux_set_line(i,ports[i],STS_MUX_C);
    		sts_mux_set_line(i,ports[i],STS_MUX_D);
    	}
    }
}

void sts_mux_set_line(uint8_t port, uint8_t addr, sts_mux_line_t line)
{
	Xil_Out32(STS_MUX_BASE_ADDR + ((port+(line*8)) * sizeof(uint32_t)), addr);
}

#endif // USER_CONFIG_HARDWARE_TARGET
