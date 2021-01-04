# AMDC IP: `amdc_gpio_mux_1.0`

This IP core is a mux for the GPIO lines on the IsoSPI ports. It allows remapping of internal FPGA device driver signals from internal device IP's to either of the two IsoSPI ports. 
## Features

- Remap IsoSPI ports GPIO lines
- Configurable in real time using the CLI

## IP Interface

The IP is accessed via the AXI4-Lite register-based interface from the DSP.

## Register Map

| Offset | Name | R/W | Description |
| -- | -- | -- | -- |
| 0x00 | PORT1_GPIO_SEL | W | Mux selector for Status A output on port 1 and Status A input on Device 1 |
| 0x04 | PORT2_GPIO_SEL | W | Mux selector for Status A output on port 2 and Status A input on Device 2 |
|----- | Unused | W | Reserved for future use |
| 0x7C | Unused | W | Reserved for future use |

### Register: `PORTn_GPIO_SEL`
| Bits | Name | Description |
| -- | -- | -- |
| 31:0 | DATA | Unsigned number from 0 to 4<br> - indicates which device drivers are connected IsoSPI port _n_ <br> - Value of 0 designates port _n_ as unused and all outputs will be held to logic low


## C drivers

The drivers used to configure this IP core can be found [here](../../sdk/bare/common/drv/docs/GPIO-Mux.md). 
