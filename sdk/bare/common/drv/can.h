#ifndef CAN_H
#define CAN_H

#include "xcanps.h"
#include <stdint.h>


/*
 * The Baud Rate Prescaler Register (BRPR) and Bit Timing Register (BTR)
 * are setup such that CAN baud rate equals 40Kbps, assuming that the
 * the CAN clock is 24MHz. The user needs to modify these values based on
 * the desired baud rate and the CAN clock frequency. For more information
 * see the CAN 2.0A, CAN 2.0B, ISO 11898-1 specifications.
 */

/*
 * Timing parameters to be set in the Bit Timing Register (BTR).
 * These values are for a 40 Kbps baudrate assuming the CAN input clock
 frequency
 * is 24 MHz.
 */
#define DEFAULT_BTR_SYNCJUMPWIDTH      3
#define DEFAULT_BTR_SECOND_TIMESEGMENT 2
#define DEFAULT_BTR_FIRST_TIMESEGMENT  15

/*
 * The Baud rate Prescalar value in the Baud Rate Prescaler Register (BRPR)
 * needs to be set based on the input clock  frequency to the CAN core and
 * the desired CAN baud rate.
 * This value is for a 40 Kbps baudrate assuming the CAN input clock frequency
 * is 24 MHz.
 */
#define DEFAULT_BAUD_PRESCALAR 29

// Different CAN modes
typedef enum {
    CAN_CONFIG = XCANPS_MODE_CONFIG,
    CAN_LOOPBACK = XCANPS_MODE_LOOPBACK,
    CAN_NORMAL = XCANPS_MODE_NORMAL,
    CAN_SLEEP = XCANPS_MODE_SLEEP,
    CAN_SNOOP = XCANPS_MODE_SNOOP,
} can_mode_t;

// Struct representing an entire CAN Packet
typedef struct can_packets_t {
    int message_id;
    int num_bytes;
    uint8_t buffer[8];
} can_packet_t;

// Setter methods, useful for configuring the CAN peripheral
int can_setmode(can_mode_t MODE);
int can_setbaud(int rate);
int can_set_btr(uint8_t sjw, uint8_t ts1, uint8_t ts2);
int can_set_peripheral(int device_id);

// Initialize the CAN peripheral
int can_init(int device_id);

// Send and get CAN packets
int can_send(can_packet_t *packet);
int can_rcv(can_packet_t *packet);

// Useful debugging functionality
void can_print_mode();
void can_print_peripheral();

// Sanity check that hardware working
int can_loopback_test();

#endif // CAN_H
