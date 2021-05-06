#ifndef ICC_H
#define ICC_H

#include <stdbool.h>
#include <stdint.h>

#define ICC_CACHE_BASE_ADDR 	0x3000000
#define ICC_CACHE_SIZE		0x100000

#define ICC_TABLE_SIZE		0x16

uint8_t* ICC_F;

uint32_t* ICC_LED_1;
uint32_t* ICC_LED_2;
uint32_t* ICC_LED_3;
uint32_t* ICC_LED_4;
uint32_t* ICC_x_pos;
uint32_t* ICC_y_pos;
uint32_t* ICC_z_pos;
uint8_t* ICC_a;
uint8_t* ICC_b;
uint8_t* ICC_c;

void icc_init(void);

#endif // ICC_H
