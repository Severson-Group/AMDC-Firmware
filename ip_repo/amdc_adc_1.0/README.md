# IP: `amdc_adc_1.0`

This IP core is a driver for the ADC device used on the AMDC hardware ([LTC2320-14](https://www.analog.com/media/en/technical-documentation/data-sheets/232014fa.pdf)).

## Features

- Supports configurable SPI clock rates (100 MHz [buggy], 50 MHz [recommended], 25 MHz, 12.5 MHz)
- ADC acquisition is free-running at maximum sample rate (much higher than expected PWM)
- Eight channel data is transfered synchronously from ADC
- Valid incoming ADC data is copied to user data registers at configurable times:
    - Synchronous to PWM carrier high point
    - Synchronous to PWM carrier low point
    - Synchronous to PWM carrier high and low point
    - Asynchronous to PWM carrier
    
## IP Interface

The IP is accessed via the AXI4-Lite register-based interface from the DSP. This interface can read the raw ADC data and update the control register.

## Configuration

This IP has no build-time configuration parameters. It expects a 200 MHz clock input from the AXI interconnect.

## Register Map

| Offset | Name | R/W | Description |
| -- | -- | -- | -- |
| 0x00 | ANALOG_CHANNEL1 | R | Raw ADC channel 1 data register |
| 0x04 | ANALOG_CHANNEL2 | R | Raw ADC channel 2 data register |
| 0x08 | ANALOG_CHANNEL3 | R | Raw ADC channel 3 data register |
| 0x0C | ANALOG_CHANNEL4 | R |Raw ADC channel 4 data register |
| 0x10 | ANALOG_CHANNEL5 | R | Raw ADC channel 5 data register |
| 0x14 | ANALOG_CHANNEL6 | R | Raw ADC channel 6 data register |
| 0x18 | ANALOG_CHANNEL7 | R | Raw ADC channel 7 data register |
| 0x1C | ANALOG_CHANNEL8 | R | Raw ADC channel 8 data register |
| 0x20 | ANALOG_CONTROL | RW | Configuration register |
| 0x24 | ANALOG_UNUSED1 | RW | Unused register |

### Register: `ANALOG_CHANNEL*`

| Bits | Name | Description |
| -- | -- | -- |
| 31:0 | DATA | Sign-extended, 2's complement, 14-bit raw data from ADC |

### Register: `ANALOG_CONTROL`

| Bits | Name | Description |
| -- | -- | -- |
| 3 | PWM_SYNC_LOW | Enables sync of ADC samples to PWM carrier low point |
| 2 | PWM_SYNC_HIGH | Enables sync of ADC samples to PWM carrier high point |
| 1:0 | CLKDIV | Clock divisor for SCLK generation from 200 MHz main clock (0: DIV2, 1: DIV4, 2: DIV8, 3: DIV16) |

## Testing

This IP has been tested on the AMDC REV D hardware target and works when SCLK = 50 MHz (`CLKDIV` = DIV4). At 100 MHz, data bits start to be missed -- it looks like off-by-one issues with timing in the RTL.
