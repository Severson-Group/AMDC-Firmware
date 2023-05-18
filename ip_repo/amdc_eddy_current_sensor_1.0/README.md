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
| 0x08 | SPI_DIVIDER   | W | IP Configuration Register |
| 0x0C | PWM_TRIGGERS  | W | IP Configuration Register |


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

### SPI_DIVIDER
| Bits | Name | Description |
| -- | -- | -- |
| 7:0 | DIVIDER | SCLK will toggle after a number of AXI CLK cycles equal to this register value, default 10 <br /> Since AXI CLK period is 5ns, this will be 50ns high/50ns low, or 10MHz for SCLK |

### PWM_TRIGGERS
| Bits | Name | Description |
| -- | -- | -- |
| 0 | PWM_HIGH_TRIGGER | If this bit is 1, a conversion/acquistion cycle will be triggered when the PWM carrier hits its max value (default 1) |
| 1 | PWM_LOW_TRIGGER | If this bit is 1, a conversion/acquistion cycle will be triggered when the PWM carrier hits its min value (default 1) |




