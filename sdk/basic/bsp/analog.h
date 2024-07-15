#ifndef ANALOG_H
#define ANALOG_H

#include <stdint.h>

typedef enum {
	ANLG_CHNL1 = 1,
	ANLG_CHNL2,
	ANLG_CHNL3,
	ANLG_CHNL4,
	ANLG_CHNL5,
	ANLG_CHNL6,
	ANLG_CHNL7,
	ANLG_CHNL8,
	ANLG_CHNL9,
	ANLG_CHNL10,
	ANLG_CHNL11,
	ANLG_CHNL12,
	ANLG_CHNL13,
	ANLG_CHNL14,
	ANLG_CHNL15,
	ANLG_CHNL16
} analog_channel_e;

typedef enum {
	ANLG_CLKDIV2 = 0,
	ANLG_CLKDIV4,
	ANLG_CLKDIV8,
	ANLG_CLKDIV16
} analog_clkdiv_e;

#define ANALOG_NUM_CHANNELS				(16)
#define ANALOG_CALIBRATE_NUM_SAMPLES	(256)

void analog_init(void);

void analog_calibrate(uint32_t num);

void analog_set_clkdiv(analog_clkdiv_e div);
void analog_get_clkdiv(analog_clkdiv_e *div);

void analog_getf(analog_channel_e channel, float *value);
void analog_geti(analog_channel_e channel, int16_t *value);

// void analog_set_filter(analog_channel_e channel, ...);

#endif // ANALOG_H
