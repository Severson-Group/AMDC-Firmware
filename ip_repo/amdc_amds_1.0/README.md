# IP: `amdc_amds_1.0`

This IP core is a driver for the [AMDS platform mainboard](https://github.com/Severson-Group/AMDS/tree/develop/Mainboard). It replaces the legacy driver `amdc_motherboard_1.0` as of AMDC Firmware v1.3, to be compatible with the sensor [Timing Manager](https://github.com/Severson-Group/AMDC-Firmware/tree/develop/ip_repo/amdc_timing_manager_1.0) IP.

## Features

- Uses one GPIO port on the AMDC
- Synchronous triggering of daughtercard ADC conversions with AMDC PWM carrier
- Compatible with updated AMDS Firmware, which sends data from ADCs back to AMDC immediately after sampling
- Compatible with the sensor Timing Manager IP

This IP core has no constraints on the frequency of the input trigger signal (synchronized to the PWM carrier via the timing manager). This means that the user can attempt to run the AMDS interface as fast as they want. It will break at some limit, but this will be due to the AMDS firmware itself [found here](https://github.com/Severson-Group/AMDS/tree/develop/Mainboard/Firmware), not this driver.

## IP Interface

The IP is accessed via the AXI4-Lite register-based interface from the DSP. This interface can read the raw ADC data, read the Channel Valid register to determine which daughtercard ADCs sent back valid data, and update the control registers.

## Configuration

This IP has no build-time configuration parameters. It expects a 200 MHz clock input from the AXI interconnect.

## Register Map

| Offset | Name | R/W | Description |
| -- | -- | -- | -- |
| 0x00 | MB_ADC_CH1 | R | Raw ADC channel 1 data register |
| 0x04 | MB_ADC_CH2 | R | Raw ADC channel 2 data register |
| 0x08 | MB_ADC_CH3 | R | Raw ADC channel 3 data register |
| 0x0C | MB_ADC_CH4 | R | Raw ADC channel 4 data register |
| 0x10 | MB_ADC_CH5 | R | Raw ADC channel 5 data register |
| 0x14 | MB_ADC_CH6 | R | Raw ADC channel 6 data register |
| 0x18 | MB_ADC_CH7 | R | Raw ADC channel 7 data register |
| 0x1C | MB_ADC_CH8 | R | Raw ADC channel 8 data register |
| 0x20 | MB_UNUSED4 | RW | Unused register |
| 0x24 | MB_CH_VALID | R | Channel Valid register |
| 0x28 | MB_COUNT_VALID   | R | Number of valid data recieved over UART |
| 0x2C | MB_COUNT_CORRUPT | R | Number of corrupt data recieved over UART |
| 0x30 | MB_COUNT_TIMEOUT | R | Number of timeout data recieved over UART |
| 0x34 | MB_UNUSED5 | RW | Unused register |
| 0x38 | MB_UNUSED6 | RW | Unused register |
| 0x3C | MB_UNUSED7 | RW | Unused register |

### Register: `MB_ADC_CH*`

| Bits | Name | Description |
| -- | -- | -- |
| 31:0 | DATA | Sign-extended, 2's complement, raw data from ADCs on AMDS daughtercards |

### Register: `MB_CH_VALID`

| Bits | Name | Description |
| -- | -- | -- |
| 7:0 | CH_VALID | Indicates that the raw ADC data contained in a channel's data register is valid. For example, bits[7:0] == '11001101' means that the data registers for channels 1, 3, 4, 7, and 8 contain valid data, while the data registers for channels 2, 5, and 6 contain invalid data |

### Register: `MB_COUNT_VALID`

| Bits | Name | Description |
| -- | -- | -- |
| 31:16 | VALID_RX1 | Unsigned count of valid bytes received over UART1 from AMDS |
| 15:0  | VALID_RX0 | Unsigned count of valid bytes received over UART0 from AMDS |

### Register: `MB_COUNT_CORRUPT`

| Bits | Name | Description |
| -- | -- | -- |
| 31:16 | CORRUPT_RX1 | Unsigned count of corrupt bytes received over UART1 from AMDS |
| 15:0  | CORRUPT_RX0 | Unsigned count of corrupt bytes received over UART0 from AMDS |

### Register: `MB_COUNT_TIMEOUT`

| Bits | Name | Description |
| -- | -- | -- |
| 31:16 | TIMEOUT_RX1 | Unsigned count of timeout bytes received over UART1 from AMDS |
| 15:0  | TIMEOUT_RX0 | Unsigned count of timeout bytes received over UART0 from AMDS |

## Testing

This IP core has been tested with the AMDC REV F and AMDS REV D hardware. Note that the functionality is also fully dependent on the firmware running on the AMDS' processor.
