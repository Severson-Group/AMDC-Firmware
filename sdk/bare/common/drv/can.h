#ifndef CAN_H
#define CAN_H

#include <stdint.h>
#include "xcanps.h"

// Setter methods, useful for configuring the CAN peripheral
int can_setmode(uint32_t value);
int can_setbaud(int rate);
int can_set_btr(int jump_width, int first_time, int second_time);
int can_set_peripheral(int device_id);

// Initialize the CAN peripheral
int can_init(int);

// Send and get CAN packets
int can_send(uint8_t* packet, int num_bytes);
int can_print();

// Useful debugging functionality
void can_print_mode();
void can_print_peripheral();

// Sanity check that hardware working
int can_loopback_test();

#endif // CAN_H
