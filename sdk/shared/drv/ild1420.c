#include "drv/ild1420.h"
#include "xil_io.h"
#include <stdint.h>

// IP base addresses per mapping in Vivado
static const uint32_t addr_mapping[ILD1420_NUM_SENSORS] = {
    0x43CA0000,
    0x43CB0000,
    0x43CC0000,
    0x43CD0000,
};

ild1420_packet_t ild1420_get_latest_packet(ild1420_sensor_t sensor)
{
    // Read the raw output of the first slave register in the FPGA
    uint32_t reg = Xil_In32(addr_mapping[sensor]);

    // Parse the raw output into our data
    ild1420_packet_t ret;
    ret.distance = reg & 0x0000FFFF;
    ret.error = (reg >> 16) & 0x3;
    ret.fresh = (reg >> 18) & 0x1;

    return ret;
}
