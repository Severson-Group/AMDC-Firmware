#include "usr/user_config.h"

#if (USER_CONFIG_ENABLE_MOTHERBOARD_SUPPORT == 1)

#include "drv/motherboard.h"
#include "drv/motherboard_defs.h"
#include "sys/cmd/cmd_mb.h"
#include "sys/commands.h"
#include "sys/defines.h"
#include "sys/util.h"
#include <stdbool.h>
#include <stdint.h>

void motherboard_init(void)
{
    cmd_mb_register();

#if USER_CONFIG_ENABLE_MOTHERBOARD_AUTO_TX == 1
    bool auto_tx = true;
#else
    bool auto_tx = false
#endif

    motherboard_set_adc_sampling(MOTHERBOARD_1_BASE_ADDR, auto_tx);
#if USER_CONFIG_HARDWARE_TARGET == AMDC_REV_E
    motherboard_set_adc_sampling(MOTHERBOARD_2_BASE_ADDR, auto_tx);
    motherboard_set_adc_sampling(MOTHERBOARD_3_BASE_ADDR, auto_tx);
    motherboard_set_adc_sampling(MOTHERBOARD_4_BASE_ADDR, auto_tx);
#endif
}

void motherboard_set_adc_sampling(uint32_t base_addr, bool enable)
{
    // Create base address for IP
    volatile uint32_t *m_motherboard = (volatile uint32_t *) base_addr;

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
    // Create base address for IP
    volatile uint32_t *m_motherboard = (volatile uint32_t *) base_addr;

    // Read register from FPGA
    uint32_t reg = m_motherboard[MOTHERBOARD_DEFS_OFFSET_CONTROL / 4];

    // Toggle the bit
    BITFLIP(reg, MOTHERBOARD_DEFS_CONTROL_SYNC_TX_SHIFT);

    // Write register to FPGA
    m_motherboard[MOTHERBOARD_DEFS_OFFSET_CONTROL / 4] = reg;
}

int motherboard_get_data(uint32_t base_addr, mb_channel_e channel, int32_t *out)
{
    // Create base address for IP
    volatile uint32_t *m_motherboard = (volatile uint32_t *) base_addr;

    if (!motherboard_is_valid_channel(channel)) {
        return FAILURE;
    } else {
        *out = (int32_t) m_motherboard[channel];
        return SUCCESS;
    }
}

void motherboard_print_samples(uint32_t base_addr)
{
    // Create base address for IP
    volatile uint32_t *m_motherboard = (volatile uint32_t *) base_addr;

    for (int i = 0; i < 8; i++) {
        uint32_t val = m_motherboard[i];
        cmd_resp_printf("%i: %04X\r\n", i, val);
    }
}

void motherboard_print_counters(uint32_t base_addr)
{
    // Create base address for IP
    volatile uint32_t *m_motherboard = (volatile uint32_t *) base_addr;

    cmd_resp_printf("V: %08X\r\n", m_motherboard[MOTHERBOARD_DEFS_OFFSET_COUNT_VALID / 4]);
    cmd_resp_printf("C: %08X\r\n", m_motherboard[MOTHERBOARD_DEFS_OFFSET_COUNT_CORRUPT / 4]);
    cmd_resp_printf("T: %08X\r\n", m_motherboard[MOTHERBOARD_DEFS_OFFSET_COUNT_TIMEOUT / 4]);
}

#endif // USER_CONFIG_ENABLE_MOTHERBOARD_SUPPORT
