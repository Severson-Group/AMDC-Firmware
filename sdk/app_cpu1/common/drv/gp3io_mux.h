#include "usr/user_config.h"
#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E

#ifndef GP3IO_MUX_H
#define GP3IO_MUX_H

#include "xparameters.h"
#include <stdbool.h>
#include <stdint.h>

#define GP3IO_MUX_1_BASE_ADDR (XPAR_HIER_GPIO_0_AMDC_GP3IO_MUX_0_S00_AXI_BASEADDR)
#define GP3IO_MUX_2_BASE_ADDR (XPAR_HIER_GPIO_1_AMDC_GP3IO_MUX_0_S00_AXI_BASEADDR)
#define GP3IO_MUX_3_BASE_ADDR (XPAR_HIER_GPIO_2_AMDC_GP3IO_MUX_0_S00_AXI_BASEADDR)
#define GP3IO_MUX_4_BASE_ADDR (XPAR_HIER_GPIO_3_AMDC_GP3IO_MUX_0_S00_AXI_BASEADDR)

typedef enum {
    GP3IO_MUX_UNUSED = 0,
    GP3IO_MUX_DEVICE1 = 1,
    GP3IO_MUX_DEVICE2 = 2,
    GP3IO_MUX_DEVICE3 = 3,
    GP3IO_MUX_DEVICE4 = 4
} gp3io_mux_device_t;

void gp3io_mux_init(void);
void gp3io_mux_set_device(uint32_t base_addr, gp3io_mux_device_t device);

#endif // GP3IO_MUX_H

#endif // USER_CONFIG_HARDWARE_TARGET
