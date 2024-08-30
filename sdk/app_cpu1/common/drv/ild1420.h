#ifndef ILD1420_H
#define ILD1420_H

#include <stdint.h>

#include "usr/user_config.h"
#include "xparameters.h"

typedef enum {
    // Keep starting at 0
    ILD1420_SENSOR1 = 0,
    ILD1420_SENSOR2 = 1,
    ILD1420_SENSOR3 = 2,
    ILD1420_SENSOR4 = 3,
#if (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E) || (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_F)
    ILD1420_SENSOR5 = 4,
    ILD1420_SENSOR6 = 5,
    ILD1420_SENSOR7 = 6,
    ILD1420_SENSOR8 = 7,
#endif
    // Keep at end
    ILD1420_NUM_SENSORS
} ild1420_sensor_t;

// IP base addresses per mapping in Vivado
static const uint32_t addr_mapping[ILD1420_NUM_SENSORS] = {
#if (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D)
    XPAR_HIER_ILD1420_0_AMDC_ILD1420_0_S00_AXI_BASEADDR,
    XPAR_HIER_ILD1420_0_AMDC_ILD1420_1_S00_AXI_BASEADDR,
    XPAR_HIER_ILD1420_1_AMDC_ILD1420_0_S00_AXI_BASEADDR,
    XPAR_HIER_ILD1420_1_AMDC_ILD1420_1_S00_AXI_BASEADDR
#elif (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E) || (USER_CONFIG_HARDWARE_TARGET == AMDC_REV_F)
    XPAR_HIER_GPIO_0_HIER_ILD1420_0_AMDC_ILD1420_0_S00_AXI_BASEADDR,
    XPAR_HIER_GPIO_0_HIER_ILD1420_0_AMDC_ILD1420_1_S00_AXI_BASEADDR,
    XPAR_HIER_GPIO_1_HIER_ILD1420_0_AMDC_ILD1420_0_S00_AXI_BASEADDR,
    XPAR_HIER_GPIO_1_HIER_ILD1420_0_AMDC_ILD1420_1_S00_AXI_BASEADDR,
    XPAR_HIER_GPIO_2_HIER_ILD1420_0_AMDC_ILD1420_0_S00_AXI_BASEADDR,
    XPAR_HIER_GPIO_2_HIER_ILD1420_0_AMDC_ILD1420_1_S00_AXI_BASEADDR,
    XPAR_HIER_GPIO_3_HIER_ILD1420_0_AMDC_ILD1420_0_S00_AXI_BASEADDR,
    XPAR_HIER_GPIO_3_HIER_ILD1420_0_AMDC_ILD1420_1_S00_AXI_BASEADDR
#endif
};

typedef struct ild1420_packet_t {
    // 16-bit raw data from the sensor
    uint16_t distance;

    // 2-bit raw error code from sensor
    uint8_t error;

    // 1 if data was received in the last 300us, 0 if stale
    uint8_t fresh;
} ild1420_packet_t;

// User calls this function to receive the latest
// packet from the ILD1420 sensor. The FPGA is constantly
// listening for data and parsing it. The last valid
// packet is returned, which may be OLD! Use the "fresh"
// bit to determine recency.
//
// NOTE: the C code does NOT have to initialize this driver,
// because the FPGA takes care of aligning its acquisitions
// to the UART data stream.
ild1420_packet_t ild1420_get_latest_packet(ild1420_sensor_t sensor);

#endif // ILD1420_H
