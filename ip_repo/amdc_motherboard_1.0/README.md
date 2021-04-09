# IP: `amdc_motherboard_1.0`

This IP core is a driver for the [AMDS platform mainboard](https://github.com/Severson-Group/AMDS/tree/develop/Mainboard).

## Features

- Uses one full GPIO port on AMDC (REV D hardware has two ports total)
- Synchronous triggering of daughtercard ADC conversions with AMDC PWM carrier
- Synchronous transmission of sample data from motherboard to AMDC

This IP core has no constraints on the frequency of input and output signals (i.e. PWM sync and data request rate). This means that the user can attempt to run the motherboard interface as fast as they want. It will break at some limit, but this will be due to the motherboard firmware itself, not this driver.

Reasonable PWM switching frequency and data request rates should work fine (e.g. 100kHz PWM switching and 10kHz data request rate).

## IP Interface

The IP is accessed via the AXI4-Lite register-based interface from the DSP. This interface can read the raw ADC data and update the control registers.

## Configuration

This IP has no build-time configuration parameters. It expects a 200 MHz clock input from the AXI interconnect.

## Register Map

| Offset | Name | R/W | Description |
| -- | -- | -- | -- |
| 0x00 | MB_ADC_CH1 | R | Raw ADC channel 1 data register |
| 0x04 | MB_ADC_CH2 | R | Raw ADC channel 2 data register |
| 0x08 | MB_ADC_CH3 | R | Raw ADC channel 3 data register |
| 0x0C | MB_ADC_CH4 | R |Raw ADC channel 4 data register |
| 0x10 | MB_ADC_CH5 | R | Raw ADC channel 5 data register |
| 0x14 | MB_ADC_CH6 | R | Raw ADC channel 6 data register |
| 0x18 | MB_ADC_CH7 | R | Raw ADC channel 7 data register |
| 0x1C | MB_ADC_CH8 | R | Raw ADC channel 8 data register |
| 0x20 | MB_CONTROL | RW | Configuration register |
| 0x24 | MB_STATUS  | R  | Status register |
| 0x28 | MB_COUNT_VALID | R | Number of valid data recieved over UART |
| 0x2C | MB_COUNT_CORRUPT | R | Number of corrupt data recieved over UART |
| 0x30 | MB_COUNT_TIMEOUT | R | Number of timeout data recieved over UART |
| 0x34 | MB_UNUSED5 | RW | Unused register |
| 0x38 | MB_UNUSED6 | RW | Unused register |
| 0x3C | MB_UNUSED7 | RW | Unused register |

### Register: `MB_ADC_CH*`

| Bits | Name | Description |
| -- | -- | -- |
| 31:0 | DATA | Sign-extended, 2's complement, raw data from ADC on motherboard |

### Register: `MB_CONTROL`

| Bits | Name | Description |
| -- | -- | -- |
| 1 | SYNC_TX | Edge-triggered bit which starts the data transmission from the motherboard back to the AMDC. User driver should toggle this bit to request new data. |
| 0 | SYNC_ADC_EN | Enables transmission of signal which synchronizes ADC sampling on motherboard to PWM carrier on AMDC |

### Register: `MB_STATUS`

| Bits | Name | Description |
| -- | -- | -- |
| 0 | RX_IN_PROGRESS | 1: motherboard is currently sending data to AMDC, 0: data transmission is idle. NOTE: the data registers (`ADC_CH1..8`) will always contain valid data; when they are read, the latest valid data is returned. |

### Register: `MB_COUNT_VALID`

| Bits | Name | Description |
| -- | -- | -- |
| 31:16 | VALID_RX1 | Unsigned count of valid bytes received over UART1 from motherboard |
| 15:0  | VALID_RX0 | Unsigned count of valid bytes received over UART0 from motherboard |

### Register: `MB_COUNT_CORRUPT`

| Bits | Name | Description |
| -- | -- | -- |
| 31:16 | CORRUPT_RX1 | Unsigned count of corrupt bytes received over UART1 from motherboard |
| 15:0  | CORRUPT_RX0 | Unsigned count of corrupt bytes received over UART0 from motherboard |

### Register: `MB_COUNT_TIMEOUT`

| Bits | Name | Description |
| -- | -- | -- |
| 31:16 | TIMEOUT_RX1 | Unsigned count of timeout bytes received over UART1 from motherboard |
| 15:0  | TIMEOUT_RX0 | Unsigned count of timeout bytes received over UART0 from motherboard |

## Testing

This IP core has been tested with the AMDC REV D hardware and REV A motherboard. Note that the functionality is fully dependent on the firmware running on the motherboard processor.
