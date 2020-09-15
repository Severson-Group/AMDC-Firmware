#include "drv/hardware_targets.h"
#include "usr/user_config.h"

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D

#include "drv/sts_mux.h"
#include "xil_io.h"
#include <stdint.h>
#include <stdio.h>

#define STS_MUX_BASE_ADDR (0x43C50000)

void sts_mux_init(void)
{
    printf("STATUS LINES:\tInitializing...\n");

    Xil_Out32(STS_MUX_BASE_ADDR + (35 * sizeof(uint32_t)), 0xF);
    /*
    if(INVERTER_PORT1) {
    	sts_mux_set_line(0,INVERTER_PORT1,STS_MUX_A);
    	sts_mux_set_line(0,INVERTER_PORT1,STS_MUX_B);
    	sts_mux_set_line(0,INVERTER_PORT1,STS_MUX_C);
    }

    if(INVERTER_PORT2)
    {
		sts_mux_set_line(1,INVERTER_PORT2,STS_MUX_A);
		sts_mux_set_line(1,INVERTER_PORT2,STS_MUX_B);
		sts_mux_set_line(1,INVERTER_PORT2,STS_MUX_C);
    }

    if(INVERTER_PORT3) {
    	sts_mux_set_line(2,INVERTER_PORT3,STS_MUX_A);
    	sts_mux_set_line(2,INVERTER_PORT3,STS_MUX_B);
    	sts_mux_set_line(2,INVERTER_PORT3,STS_MUX_C);
    }

    if(INVERTER_PORT4)
    {
		sts_mux_set_line(3,INVERTER_PORT4,STS_MUX_A);
		sts_mux_set_line(3,INVERTER_PORT4,STS_MUX_B);
		sts_mux_set_line(3,INVERTER_PORT4,STS_MUX_C);
    }
    */

}

void sts_mux_set_line(uint8_t port, uint8_t addr, sts_mux_line_t line)
{
	uint32_t reg;
	reg = Xil_In32(STS_MUX_BASE_ADDR + (line * sizeof(uint32_t)));
	reg &= ~(0x7<<port*3);
	reg |= ((addr<<(port*3)) | (0x1<<(port+24)));
	Xil_Out32(STS_MUX_BASE_ADDR + (line * sizeof(uint32_t)), reg);
}

#endif // USER_CONFIG_HARDWARE_TARGET
