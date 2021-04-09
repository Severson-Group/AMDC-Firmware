# AMDC IP: `amdc_eddy_current_sensor_1.0`

This IP core is a driver for the digital interface of the eddy current sensor used for levitation control in the CRAMB. The IP is designed as the SPI master to the dual [AD4011](https://www.analog.com/media/en/technical-documentation/data-sheets/AD4003-4007-4011.pdf) ADCs that are integrated into the eddy current sensor.

## Features

- 12.5 MHz - 50 kHz SPI clock
- 500KSps - 2KSps sample rate
- 3-Wire operation 

More details specific to the ADC timing requirements can be found in the [datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/AD4003-4007-4011.pdf)
    
## IP Interface

The IP is accessed via the AXI4-Lite register-based interface from the DSP. This interface retrieves X and Y positional data from the eddy current sensor through control of its integrated ADCs

## Register Map

| Offset | Name | R/W | Description |
| -- | -- | -- | -- |
| 0x00 | SENSOR_DATA_X | R | IP DATA REGISTER |
| 0x04 | SENSOR_DATA_Y | R | IP DATA REGISTER |
| 0x08 | CLOCK_DIVIDER | W | IP Configuration Register |
| 0x0C | CONVERSION_EN | W | IP Configuration Register |


### SENSOR_DATA_X
| Bits | Name | Description |
| -- | -- | -- |
| 31:18 | SIGN_EXT | Sign extended bit 17 |
| 17:0  | DATA | 2's compliment 18 bit positional data of the X direction |

### SENSOR_DATA_Y
| Bits | Name | Description |
| -- | -- | -- |
| 31:18 | SIGN_EXT | Sign extended bit 17 |
| 17:0  | DATA | 2's compliment 18 bit positional data of the Y direction |

### CLOCK_DIVIDER
| Bits | Name | Description |
| -- | -- | -- |
| 7:0 | DIVIDER | Divides the SPI clock and conversion signal transition times by the value in the register plus one|

### CONVERSION_EN
| Bits | Name | Description |
| -- | -- | -- |
| 0 | ENABLE | Enables the SPI clock and conversion signals to transition. A value of 0x1 enables the SPI interface. While enabled the IP core will continuously read from the eddy current sensor's ADCs |




