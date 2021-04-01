#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include "sys/errors.h"
#include "xparameters.h"
#include <stdbool.h>
#include <stdint.h>

static const uint32_t PERIPHERAL_MOTHERBOARD_NUM_INSTANCES = XPAR_AMDC_MOTHERBOARD_NUM_INSTANCES;
static inline bool peripheral_motherboard_is_valid_id(int id)
{
    return (id >= 0 && id < PERIPHERAL_MOTHERBOARD_NUM_INSTANCES);
}

static inline uint32_t peripheral_motherboard_id_to_base_addr(int id)
{
    switch (id) {
    case XPAR_HIER_AMDS_IP_AMDC_MOTHERBOARD_0_DEVICE_ID:
        return XPAR_HIER_AMDS_IP_AMDC_MOTHERBOARD_0_S00_AXI_BASEADDR;
    case XPAR_HIER_AMDS_IP_AMDC_MOTHERBOARD_1_DEVICE_ID:
        return XPAR_HIER_AMDS_IP_AMDC_MOTHERBOARD_1_S00_AXI_BASEADDR;
    default:
        return 0;
    }
}

static inline bool peripheral_eddy_current_sensor_is_valid_id(int id)
{
    return (id >= 0 && id < XPAR_AMDC_EDDY_CURRENT_SENSOR_NUM_INSTANCES);
}

static inline uint32_t peripheral_eddy_current_sensor_id_to_base_addr(int id)
{
    switch (id) {
    case XPAR_HIER_EDDY_CURRENT_SENSOR_IP_AMDC_EDDY_CURRENT_SE_0_DEVICE_ID:
        return XPAR_HIER_EDDY_CURRENT_SENSOR_IP_AMDC_EDDY_CURRENT_SE_0_S00_AXI_BASEADDR;
    case XPAR_HIER_EDDY_CURRENT_SENSOR_IP_AMDC_EDDY_CURRENT_SE_1_DEVICE_ID:
        return XPAR_HIER_EDDY_CURRENT_SENSOR_IP_AMDC_EDDY_CURRENT_SE_1_S00_AXI_BASEADDR;
    default:
        return 0;
    }
}

error_t peripherals_init(void);

#endif
