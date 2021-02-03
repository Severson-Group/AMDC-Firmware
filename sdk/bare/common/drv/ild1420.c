#include "drv/ild1420.h"
#include "xil_io.h"
#include <stdint.h>

#define ILD1420_BASE_ADDR (0x43C80000)
#define OFFSET (0x10000)

// Sensor 1: (0x43C80000)
// Sensor 2: (0x43C90000)
// Sensor 3: (0x43CA0000)

ild1420_packet_t ild1420_get_latest_packet(ild1420_device_t sensor)
{
    // Read the raw output of the first slave register in the FPGA
    uint32_t reg = Xil_In32(ILD1420_BASE_ADDR + (OFFSET * sensor));

    // Parse the raw output into our data
    ild1420_packet_t ret;
    ret.distance = reg & 0x0000FFFF;
    ret.error = (reg >> 16) & 0x3;
    ret.fresh = (reg >> 18) & 0x1;

    return ret;
}
