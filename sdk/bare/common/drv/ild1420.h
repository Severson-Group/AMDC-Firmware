#ifndef ILD1420_H
#define ILD1420_H

#include <stdint.h>

typedef enum{
	SENSOR1 = 0,
	SENSOR2 = 1,
	SENSOR3 = 2
} ild1420_device_t;

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
ild1420_packet_t ild1420_get_latest_packet(ild1420_device_t);

#endif // ILD1420_H
