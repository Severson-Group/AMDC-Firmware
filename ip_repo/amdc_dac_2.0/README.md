# AMDC IP: `amdc_dac_2.0`

This IP core is a driver for the DAC expansion board used on the AMDC hardware. The IP is designed to mirror the internal registers of the [DAC60508MC](https://github.com/Severson-Group/AMDC-Hardware/blob/develop/Accessories/ExpansionBoard_DAC/REV20200720B/datasheets/dac60508.pdf) and act as a bridge between the AXI4-Lite interface and the SPI interface on the DAC IC. Functionally, it behaves as if the DSP is directly connected to the DAC60508MC.

## Features

- 25MHz SPI clock rate
- 80KSps maximum sample rate on all 8 channels
- All Eight channels can be configured to synchronously update outputs. See [datasheet](https://github.com/Severson-Group/AMDC-Hardware/blob/develop/Accessories/ExpansionBoard_DAC/REV20200720B/datasheets/dac60508.pdf) for more details
- Eight 12-bit output channels at +/- 5V or +/- 10V
- Configurable external trigger of synchronous channels

More details specific to the DAC IC can be found in the [datasheet](https://github.com/Severson-Group/AMDC-Hardware/blob/develop/Accessories/ExpansionBoard_DAC/REV20200720B/datasheets/dac60508.pdf)
    
## IP Interface

The IP is accessed via the AXI4-Lite register-based interface from the DSP. This interface updates the internal DAC registers through a simplified SPI connection via the Inverter Power Stack or IsoSPI ports.

## Register Map

| Offset | Name | R/W | Description |
| -- | -- | -- | -- |
| 0x00 | DAC_CONFIG1 | W | Configuration Register |
| 0x04 | DAC_UNUSED1 | W | Unused register |
| 0x08 | SYNC_REGISTER | W | More information can be found in the [datasheet](https://github.com/Severson-Group/AMDC-Hardware/blob/develop/Accessories/ExpansionBoard_DAC/REV20200720B/datasheets/dac60508.pdf) |
| 0x0C | CONFIG_REGISTER | W | More information can be found in the [datasheet](https://github.com/Severson-Group/AMDC-Hardware/blob/develop/Accessories/ExpansionBoard_DAC/REV20200720B/datasheets/dac60508.pdf) |
| 0x10 | GAIN_REGISTER | W | More information can be found in the [datasheet](https://github.com/Severson-Group/AMDC-Hardware/blob/develop/Accessories/ExpansionBoard_DAC/REV20200720B/datasheets/dac60508.pdf) |
| 0x14 | TRIGGER_REGISTER | W | More information can be found in the [datasheet](https://github.com/Severson-Group/AMDC-Hardware/blob/develop/Accessories/ExpansionBoard_DAC/REV20200720B/datasheets/dac60508.pdf) |
| 0x18 | BROADCAST_REGISTER | W | More information can be found in the [datasheet](https://github.com/Severson-Group/AMDC-Hardware/blob/develop/Accessories/ExpansionBoard_DAC/REV20200720B/datasheets/dac60508.pdf) |
| 0x1C | DAC_UNUSED2 | W | Unused register |
| 0x20 | DAC_CHANNEL0 | W | Raw DAC channel 0 data register |
| 0x24 | DAC_CHANNEL1 | W | Raw DAC channel 1 data register |
| 0x28 | DAC_CHANNEL2 | W | Raw DAC channel 2 data register |
| 0x2C | DAC_CHANNEL3 | W | Raw DAC channel 3 data register |
| 0x30 | DAC_CHANNEL4 | W | Raw DAC channel 4 data register |
| 0x34 | DAC_CHANNEL5 | W | Raw DAC channel 5 data register |
| 0x38 | DAC_CHANNEL6 | W | Raw DAC channel 6 data register |
| 0x3C | DAC_CHANNEL7 | W | Raw DAC channel 7 data register |

### DAC_CONFIG1
| Bits | Name | Description |
| -- | -- | -- |
| 0 | EXT_TRIGGER_EN | Enables the DAC to trigger on an external source when 1 |

