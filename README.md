# AMDC-Firmware

Advanced Motor Drive Controller (AMDC), is an open-source project from the [Severson Research Group](https://severson.wempec.wisc.edu/) at [UW-Madison](http://www.engr.wisc.edu/department/electrical-computer-engineering/), affiliated with [Wisconsin Electric Machines and Power Electronics Consortium (WEMPEC)](https://wempec.wisc.edu/).

AMDC design includes both hardware and firmware. This repository contains the low-level firmware which runs AMDC, as well as example applications.

## Documentation

Documentation has been written to help ease the process of using the AMDC platform. To get started, recommendation is given to read the docs in the order listed below. Once you understand the general [firmware architecture](docs/00-Firmware-Architecture.md), try [downloading the code, building it, and running](docs/01-Building-and-Running-Firmware.md) an example application (i.e., [`blink`](sdk/bare/usr/blink/)) on the AMDC hardware. After your application is stable, try [flashing](docs/02-Flashing-AMDC.md) it to the AMDC hardware for permanent usage.

If you run into low-level issues, consider using the Xilinx [debugging tools](docs/03-Low-Level-Debugging.md) to investigate register state, etc.

### [Firmware Architecture](docs/00-Firmware-Architecture.md)
- [Drivers](docs/00a-Firmware-Arch-Drivers.md)
- [System](docs/00b-Firmware-Arch-System.md)
- [User Apps](docs/00c-Firmware-Arch-UserApps.md)

### [GitHub to AMDC Hardware: Building and Running Firmware](docs/01-Building-and-Running-Firmware.md)

### [Flashing AMDC](docs/02-Flashing-AMDC.md)

### [Low-Level Debugging](docs/03-Low-Level-Debugging.md)

## License

This project is licensed under the BSD-3-Clause License - see the [LICENSE.md](LICENSE.md) file for details.
