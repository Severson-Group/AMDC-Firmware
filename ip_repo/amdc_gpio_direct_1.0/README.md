# IP: `amdc_gpio_direct_1.0`

This IP core is a very simple driver for the GPIO port on the AMDC. It allows the user to read/write voltages *directly* on the desired pins.

## Features

- Allows the user to read/write digital voltages on the pins of the AMDC GPIO ports.

### Differences between AMDC Revisions
- AMDC REV E
	- On AMDC REV E, this is GPIO Ports 1-4. Each port has Read pins 1-3 and Write pins 1-3 (Read and Write pins are different electric connections).
	- There are four instances of the `gpio_direct` IP block, one in each GPIO port hierarchy. Each instance has a read and write register.
- AMDC REV D
	- On AMDC REV D, this is Expansion Ports 1-2. Each port has Read pins 1-2 and Write pins 1-2 (Read and Write pins are different electric connections).
	- There are two instances of the `gpio_direct` IP block in the top-level block design for REV D. Each instance has a read and write register.
    
## IP Interface

The IP is accessed via the AXI4-Lite register-based interface from the DSP. 

This interface can read digital voltages on the GPIO input pins by performing register reads (using the [abstracted C code](https://github.com/Severson-Group/AMDC-Firmware/blob/aa4e90a43b2c7c4206aaa8d6adfa4fed2d530c96/sdk/app_cpu1/common/drv/gpio_direct.c)), and write digital voltages out to the GPIO output pins by performing register writes (again, using the abstracted C code).

## Configuration

This IP has no build-time configuration parameters. It expects a 200 MHz clock input from the AXI interconnect.

## Register Map

| Offset | Name | R/W | Description |
| -- | -- | -- | -- |
| 0x00 | GPIO_DIRECT_READ_REGISTER  | R  | Read data register  |
| 0x04 | GPIO_DIRECT_WRITE_REGISTER | RW | Write data register |
| 0x08 | GPIO_DIRECT_UNUSED1        | RW | Unused register |
| 0x0C | GPIO_DIRECT_UNUSED2        | RW | Unused register |

### Register: `GPIO_DIRECT_READ_REGISTER`

| Bits | Name | Description |
| -- | -- | -- |
| 31:3 | UNUSED | Zero |
| 2:0  | DATA   | Bits that represent the HIGH/LOW voltage status of the input lines on this AMDC GPIO Port |

### Register: `GPIO_DIRECT_WRITE_REGISTER`

| Bits | Name | Description |
| -- | -- | -- |
| 31:3 | UNUSED | Zero |
| 2:0  | DATA   | Bits that can ve written to alter the HIGH/LOW voltage status of the output lines on this AMDC GPIO Port |

## Testing

This IP is undergoing testing on the AMDC REV E and AMDC REV D hardware targets.
