#ifndef CAN_H
#define CAN_H

#include <stdint.h>
#include "xcanps.h"

int can_init(void);

int can_setmode(uint32_t value);
int can_setbaud(int rate);
int can_set_btr(int jump_width, int first_time, int second_time);

int can_send(uint8_t* packet, int num_bytes);
int can_print();
int can_loopback_test();


#endif // CAN_H
