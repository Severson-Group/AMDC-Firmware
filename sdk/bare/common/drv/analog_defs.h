#ifndef ANALOG_DEFS_H
#define ANALOG_DEFS_H

#include "drv/hardware_targets.h"
#include "usr/user_config.h"

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_C

// ===============
// Number of slave registers in this IP
// ===============
#define ANALOG_DEFS_NUM_SLAVE_REGS (17)

// ===============
// Offsets for the relevant slave registers (in bytes).
// Separated by 4 bytes since each register is 32 bits.
// ===============
#define ANALOG_DEFS_OFFSET_CHANNEL1  (0 * 4)
#define ANALOG_DEFS_OFFSET_CHANNEL2  (1 * 4)
#define ANALOG_DEFS_OFFSET_CHANNEL3  (2 * 4)
#define ANALOG_DEFS_OFFSET_CHANNEL4  (3 * 4)
#define ANALOG_DEFS_OFFSET_CHANNEL5  (4 * 4)
#define ANALOG_DEFS_OFFSET_CHANNEL6  (5 * 4)
#define ANALOG_DEFS_OFFSET_CHANNEL7  (6 * 4)
#define ANALOG_DEFS_OFFSET_CHANNEL8  (7 * 4)
#define ANALOG_DEFS_OFFSET_CHANNEL9  (8 * 4)
#define ANALOG_DEFS_OFFSET_CHANNEL10 (9 * 4)
#define ANALOG_DEFS_OFFSET_CHANNEL11 (10 * 4)
#define ANALOG_DEFS_OFFSET_CHANNEL12 (11 * 4)
#define ANALOG_DEFS_OFFSET_CHANNEL13 (12 * 4)
#define ANALOG_DEFS_OFFSET_CHANNEL14 (13 * 4)
#define ANALOG_DEFS_OFFSET_CHANNEL15 (14 * 4)
#define ANALOG_DEFS_OFFSET_CHANNEL16 (15 * 4)
#define ANALOG_DEFS_OFFSET_CONTROL   (16 * 4)

#endif // USER_CONFIG_HARDWARE_TARGET

#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_D

// ===============
// Number of slave registers in this IP
// ===============
#define ANALOG_DEFS_NUM_SLAVE_REGS (10)

// ===============
// Offsets for the relevant slave registers (in bytes).
// Separated by 4 bytes since each register is 32 bits.
// ===============
#define ANALOG_DEFS_OFFSET_CHANNEL1 (0 * 4)
#define ANALOG_DEFS_OFFSET_CHANNEL2 (1 * 4)
#define ANALOG_DEFS_OFFSET_CHANNEL3 (2 * 4)
#define ANALOG_DEFS_OFFSET_CHANNEL4 (3 * 4)
#define ANALOG_DEFS_OFFSET_CHANNEL5 (4 * 4)
#define ANALOG_DEFS_OFFSET_CHANNEL6 (5 * 4)
#define ANALOG_DEFS_OFFSET_CHANNEL7 (6 * 4)
#define ANALOG_DEFS_OFFSET_CHANNEL8 (7 * 4)
#define ANALOG_DEFS_OFFSET_CONTROL  (8 * 4)
#define ANALOG_DEFS_OFFSET_UNUSED1  (9 * 4)

#endif // USER_CONFIG_HARDWARE_TARGET

// ===============
// CONTROL register
// ===============
#define ANALOG_DEFS_CONTROL_CLKDIV_WIDTH (2) // in bits
#define ANALOG_DEFS_CONTROL_CLKDIV_SHIFT (0) // up from reg[0]
#define ANALOG_DEFS_CONTROL_CLKDIV_MASK  (0x3)

#define ANALOG_DEFS_CONTROL_PWM_SYNC_HIGH_WIDTH (1) // in bits
#define ANALOG_DEFS_CONTROL_PWM_SYNC_HIGH_SHIFT (2) // up from reg[0]
#define ANALOG_DEFS_CONTROL_PWM_SYNC_HIGH_MASK  (0x1)

#define ANALOG_DEFS_CONTROL_PWM_SYNC_LOW_WIDTH (1) // in bits
#define ANALOG_DEFS_CONTROL_PWM_SYNC_LOW_SHIFT (3) // up from reg[0]
#define ANALOG_DEFS_CONTROL_PWM_SYNC_LOW_MASK  (0x1)

#endif // ANALOG_DEFS_H
