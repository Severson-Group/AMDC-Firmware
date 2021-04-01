#include "drv/hardware_targets.h"
#include "usr/user_config.h"

#if (USER_CONFIG_HARDWARE_TARGET == HW_TARGET_AMDC_REV_D) && (USER_CONFIG_ENABLE_MOTHERBOARD_SUPPORT == 1)

#include "drv/motherboard.h"
#include "drv/motherboard_defs.h"
#include "sys/cmd/cmd_mb.h"
#include "sys/debug.h"
#include "sys/errors.h"
#include "sys/util.h"
#include <stdbool.h>
#include <stdint.h>

static volatile uint32_t *cast_base_addr(uint32_t base_addr) {
	volatile uint32_t *m_motherboard = (volatile uint32_t *) base_addr;
	return m_motherboard;
}

void motherboard_init(uint32_t base_addr)
{
    cmd_mb_register();

#if USER_CONFIG_ENABLE_MOTHERBOARD_AUTO_TX == 1
    motherboard_set_adc_sampling(base_addr, true);
#else
    motherboard_set_adc_sampling(base_addr, false);
#endif
}

void motherboard_set_adc_sampling(uint32_t base_addr, bool enable)
{
    // Cast base address for IP
	volatile uint32_t *m_motherboard = cast_base_addr(base_addr);

    // Read register from FPGA
    uint32_t reg = m_motherboard[MOTHERBOARD_DEFS_OFFSET_CONTROL / 4];

    // Clear bit
    BITCLEAR(reg, MOTHERBOARD_DEFS_CONTROL_SYNC_ADC_EN_SHIFT);

    // Set new bits based on user enable flag
    if (enable) {
        BITSET(reg, MOTHERBOARD_DEFS_CONTROL_SYNC_ADC_EN_SHIFT);
    }

    // Write register to FPGA
    m_motherboard[MOTHERBOARD_DEFS_OFFSET_CONTROL / 4] = reg;
}

void motherboard_request_new_data(uint32_t base_addr)
{
    // Cast base address for IP
	volatile uint32_t *m_motherboard = cast_base_addr(base_addr);

    // Read register from FPGA
    uint32_t reg = m_motherboard[MOTHERBOARD_DEFS_OFFSET_CONTROL / 4];

    // Toggle the bit
    BITFLIP(reg, MOTHERBOARD_DEFS_CONTROL_SYNC_TX_SHIFT);

    // Write register to FPGA
    m_motherboard[MOTHERBOARD_DEFS_OFFSET_CONTROL / 4] = reg;
}

error_t motherboard_get_data(uint32_t base_addr, mb_channel_e channel, int32_t *out)
{
    // Cast base address for IP
	volatile uint32_t *m_motherboard = cast_base_addr(base_addr);

    if (!motherboard_is_valid_channel(channel)) {
        return ERROR_GENERIC;
    } else {
        *out = (int32_t) m_motherboard[channel];
        return ERROR_OK;
    }
}

void motherboard_print_samples(uint32_t base_addr)
{
    // Cast base address for IP
	volatile uint32_t *m_motherboard = cast_base_addr(base_addr);

    for (int i = 0; i < 8; i++) {
        uint32_t val = m_motherboard[i];
        debug_printf("%i: %04X\r\n", i, val);
    }
}

void motherboard_print_counters(uint32_t base_addr)
{
    // Cast base address for IP
	volatile uint32_t *m_motherboard = cast_base_addr(base_addr);

    debug_printf("V: %08X\r\n", m_motherboard[MOTHERBOARD_DEFS_OFFSET_COUNT_VALID / 4]);
    debug_printf("C: %08X\r\n", m_motherboard[MOTHERBOARD_DEFS_OFFSET_COUNT_CORRUPT / 4]);
    debug_printf("T: %08X\r\n", m_motherboard[MOTHERBOARD_DEFS_OFFSET_COUNT_TIMEOUT / 4]);
}

#endif // USER_CONFIG_HARDWARE_TARGET
