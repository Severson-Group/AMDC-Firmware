#ifndef CAN_H
#define CAN_H

#include "xcanps.h"
#include <stdint.h>

/*
 * The Baud Rate Prescaler Register (BRPR) and Bit Timing Register (BTR)
 * are setup such that CAN baud rate equals 500 Kbps, assuming that the
 * the CAN clock is 200 MHz. The user needs to modify these values based on
 * the desired baud rate and the CAN clock frequency. For more information
 * see the CAN 2.0A, CAN 2.0B, ISO 11898-1 specifications.
 */

/*
 * Timing parameters to be set in the Bit Timing Register (BTR).
 * These values are for a 500 Kbps baudrate assuming the CAN input clock frequency
 * is 24 MHz.
 *
 * The value of BTR register after using these defaults is 0x1C. According to the Zynq-7000
 * Reference Manual, the actual value is one more than the value written to the register.
 * Thus, we write a value of 12 and 1 for the first and second time segment, respectively,
 * to have an actual value of 13 and 2.
 */
#define DEFAULT_BTR_SYNCJUMPWIDTH      3
#define DEFAULT_BTR_SECOND_TIMESEGMENT 1
#define DEFAULT_BTR_FIRST_TIMESEGMENT  12

/*
 * The Baud rate Prescalar value in the Baud Rate Prescaler Register (BRPR)
 * needs to be set based on the input clock  frequency to the CAN core and
 * the desired CAN baud rate.
 * This value is for a 500 Kbps baudrate assuming the CAN input clock frequency
 * is 24 MHz.
 *
 * The value of the BRPR after using this default is 0x02. According to the Zync-7000
 * Reference Manual, the actual value is one more than the value written to the register.
 * Thus, we write a value of 2 to the register, but the actual value is 3, which it should be.
 */
#define DEFAULT_BAUD_PRESCALAR 2

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
