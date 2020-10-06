#ifndef MOTHERBOARD_DEFS_H
#define MOTHERBOARD_DEFS_H

// ===============
// Number of slave registers in this IP
// ===============
#define MOTHERBOARD_DEFS_NUM_SLAVE_REGS (16)

// ===============
// Offsets for the relevant slave registers (in bytes).
// Separated by 4 bytes since each register is 32 bits.
// ===============
#define MOTHERBOARD_DEFS_OFFSET_ADC_CH1       (0 * 4)
#define MOTHERBOARD_DEFS_OFFSET_ADC_CH2       (1 * 4)
#define MOTHERBOARD_DEFS_OFFSET_ADC_CH3       (2 * 4)
#define MOTHERBOARD_DEFS_OFFSET_ADC_CH4       (3 * 4)
#define MOTHERBOARD_DEFS_OFFSET_ADC_CH5       (4 * 4)
#define MOTHERBOARD_DEFS_OFFSET_ADC_CH6       (5 * 4)
#define MOTHERBOARD_DEFS_OFFSET_ADC_CH7       (6 * 4)
#define MOTHERBOARD_DEFS_OFFSET_ADC_CH8       (7 * 4)
#define MOTHERBOARD_DEFS_OFFSET_CONTROL       (8 * 4)
#define MOTHERBOARD_DEFS_OFFSET_STATUS        (9 * 4)
#define MOTHERBOARD_DEFS_OFFSET_COUNT_VALID   (10 * 4)
#define MOTHERBOARD_DEFS_OFFSET_COUNT_CORRUPT (11 * 4)
#define MOTHERBOARD_DEFS_OFFSET_COUNT_TIMEOUT (12 * 4)
#define MOTHERBOARD_DEFS_OFFSET_UNUSED5       (13 * 4)
#define MOTHERBOARD_DEFS_OFFSET_UNUSED6       (14 * 4)
#define MOTHERBOARD_DEFS_OFFSET_UNUSED7       (15 * 4)

// ===============
// CONTROL register
// ===============
#define MOTHERBOARD_DEFS_CONTROL_SYNC_ADC_EN_WIDTH (1) // in bits
#define MOTHERBOARD_DEFS_CONTROL_SYNC_ADC_EN_SHIFT (0) // up from reg[0]
#define MOTHERBOARD_DEFS_CONTROL_SYNC_ADC_EN_MASK  (0x1)

#define MOTHERBOARD_DEFS_CONTROL_SYNC_TX_WIDTH (1) // in bits
#define MOTHERBOARD_DEFS_CONTROL_SYNC_TX_SHIFT (1) // up from reg[0]
#define MOTHERBOARD_DEFS_CONTROL_SYNC_TX_MASK  (0x1)

#endif // MOTHERBOARD_DEFS_H
