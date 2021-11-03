#ifndef ANALOG_DEFS_H
#define ANALOG_DEFS_H

#include "drv/hardware_targets.h"
#include "usr/user_config.h"

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
