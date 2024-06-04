# IP: `amdc_inverters_1.0`

This IP core drives the inverter power stack.

## Features

- 24 Configurable duty ratios (3 for each of the 8 power stack ports)
- Configurable deadtime
- Three configurable options for the time at which the duty ratios take effect

## IP Interface

This IP is accessed via the AXI4-Lite register-based interface from the DSP.

## Register Map

| Offset | Name | R/W | Description |
| -- | -- | -- | -- |
| 0x00 | DUTY_RATIO_00 | R/W | Configuration Register |
| 0x04 | DUTY_RATIO_01 | R/W | Configuration Register |
| 0x08 | DUTY_RATIO_02 | R/W | Configuration Register |
| 0x0C | DUTY_RATIO_03 | R/W | Configuration Register |
| 0x10 | DUTY_RATIO_04 | R/W | Configuration Register |
| 0x14 | DUTY_RATIO_05 | R/W | Configuration Register |
| 0x18 | DUTY_RATIO_06 | R/W | Configuration Register |
| 0x1C | DUTY_RATIO_07 | R/W | Configuration Register |
| 0x20 | DUTY_RATIO_08 | R/W | Configuration Register |
| 0x24 | DUTY_RATIO_09 | R/W | Configuration Register |
| 0x28 | DUTY_RATIO_10 | R/W | Configuration Register |
| 0x2C | DUTY_RATIO_11 | R/W | Configuration Register |
| 0x30 | DUTY_RATIO_12 | R/W | Configuration Register |
| 0x34 | DUTY_RATIO_13 | R/W | Configuration Register |
| 0x38 | DUTY_RATIO_14 | R/W | Configuration Register |
| 0x3C | DUTY_RATIO_15 | R/W | Configuration Register |
| 0x40 | DUTY_RATIO_16 | R/W | Configuration Register |
| 0x44 | DUTY_RATIO_17 | R/W | Configuration Register |
| 0x48 | DUTY_RATIO_18 | R/W | Configuration Register |
| 0x4C | DUTY_RATIO_19 | R/W | Configuration Register |
| 0x50 | DUTY_RATIO_20 | R/W | Configuration Register |
| 0x54 | DUTY_RATIO_21 | R/W | Configuration Register |
| 0x58 | DUTY_RATIO_22 | R/W | Configuration Register |
| 0x5C | DUTY_RATIO_23 | R/W | Configuration Register |
| 0x60 | CARRIER_CLK_DIV | R/W | Configuration Register |
| 0x64 | CARRIER_MAX | R/W | Configuration Register |
| 0x68 | DEADTIME | R/W | Configuration Register |
| 0x6C | RESETS | R/W | Configuration Register |
| 0x70 | FLT_TEMP_OUT | R | Status Register |
| 0x74 | FLT_DESAT_OUT | R | Status Register |
| 0x78 | RDY_OUT | R | Status Register |
| 0x7C | CONFIGURATION | R/W | Configuration Register |
| 0x80 | PER_LEG_ENABLE | R/W | Configuration Register |
| 0x84 | PER_LEG_REVERSE | R/W | Configuration Register |

### DUTY_RATIO_XX

| Bits | Name | Description |
| -- | -- | -- |
| 15:0 | DUTY_RATIO | Duty ratio for a a single inverter leg, expressed as a 16-bit nummber. A value equal to the carrier max implies a duty ratio of 1. |

### CARRIER_CLK_DIV

| Bits | Name | Description |
| -- | -- | -- |
| 7:0 | DIVIDER | Clock divider passed to triangle carrier generation module |

### CARRIER_MAX

| Bits | Name | Description |
| -- | -- | -- |
| 15:0 | CARRIER_MAX | Max value of triangle carrier |

### DEADTIME

| Bits | Name | Description |
| -- | -- | -- |
| 15:0 | DEADTIME | Configures the amount of switching deadtime. This value is applied to all inverter legs. |

### RESETS

| Bits | Name | Description |
| -- | -- | -- |
| 7:0 | RST | Passed to `inv_status_mux` IP `sts_a_int` input |

### FLT_TEMP_OUT

| Bits | Name | Description |
| -- | -- | -- |
| 7:0 | FLT_TEMP | Status D line from `inv_status_mux` IP |

### FLT_DESAT_OUT

| Bits | Name | Description |
| -- | -- | -- |
| 7:0 | FLT_DESAT | Status C line from `inv_status_mux` IP |

### RDY_OUT

| Bits | Name | Description |
| -- | -- | -- |
| 7:0 | RDY | Status B line from `inv_status_mux` IP |

### CONFIGURATION

| Bits | Name | Description |
| -- | -- | -- |
| 0 | PWM_EN | If 1, PWM switching is enabled. If 0, PWM switching is disabled. |
| 5:4 | PWM_DUTY_LATCHING_MODE | Configures when the new duty ratio values are latched into their respective registers. 2'b00 will latch at the next timing manager trigger event. 2'b01 will latch at the next peak/valley of the PWM carrier. 2'b10 will latch the values in immediately at the next rising clock edge. The value written to this register is determined in the `user_config.h` file. |

### PER_LEG_ENABLE

| Bits | Name | Description |
| -- | -- | -- |
| 23:0 | ENABLE_LEG | Each bit of this register enables a single leg of an inverter in the IP. Each set of three bits corresponds to an inverter, [2:0] is inverter 1. The least significant bit in any group of three is leg one of that inverter, middle bit leg two, and most significant bit leg three. |

### PER_LEG_REVERSE

| Bits | Name | Description |
| -- | -- | -- |
| 23:0 | REVERSE_LEG | Each bit of this register reverses a single leg of an inverter in the IP, meaning the gate drive signals for the top and bottom switch are flipped. Each set of three bits corresponds to an inverter, [2:0] is inverter 1. The least significant bit in any group of three is leg one of that inverter, middle bit leg two, and most significant bit leg three. |

