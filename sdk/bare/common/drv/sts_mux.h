#include "drv/hardware_targets.h"
#include "usr/user_config.h"

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D

#ifndef STS_MUX_H
#define STS_MUX_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    STS_MUX_UNUSED = 0,
    STS_MUX_DEVICE1 = 1, // DAC expansion board by default
    STS_MUX_DEVICE2 = 2,
    STS_MUX_DEVICE3 = 3,
    STS_MUX_DEVICE4 = 4,
    STS_MUX_DEVICE5 = 5,
    STS_MUX_DEVICE6 = 6,
    STS_MUX_DEVICE7 = 7,
    STS_MUX_DEVICE8 = 8
} sts_mux_device_t;

typedef enum { STS_MUX_A, STS_MUX_B, STS_MUX_C, STS_MUX_D } sts_mux_line_t;


// These macros are used to map the connected devices in the FPGA IP to the desired port on the
// AMDC. All ports are re-mapped to the defined device in the init function. The
// STS_MUX_UNUSED enum maps the port to the default connections from the inverter IP

#define INVERTER_PORT1 STS_MUX_DEVICE1
#define INVERTER_PORT2 STS_MUX_UNUSED
#define INVERTER_PORT3 STS_MUX_UNUSED
#define INVERTER_PORT4 STS_MUX_UNUSED
#define INVERTER_PORT5 STS_MUX_UNUSED
#define INVERTER_PORT6 STS_MUX_UNUSED
#define INVERTER_PORT7 STS_MUX_UNUSED
#define INVERTER_PORT8 STS_MUX_UNUSED

void sts_mux_init(void);

void sts_mux_set_line(uint8_t, sts_mux_device_t, sts_mux_line_t);

void sts_mux_set_device(uint8_t, sts_mux_device_t);

void sts_mux_set_output(sts_mux_line_t, bool);

#endif // STS_MUX_H

#endif // USER_CONFIG_HARDWARE_TARGET
