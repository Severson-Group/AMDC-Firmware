# Advanced Motor Drive Controller (AMDC) Firmware

The Advanced Motor Drive Controller (AMDC) is an open-source project from the [Severson Research Group](https://severson.wempec.wisc.edu/) at [UW-Madison](http://www.engr.wisc.edu/department/electrical-computer-engineering/), affiliated with [Wisconsin Electric Machines and Power Electronics Consortium (WEMPEC)](https://wempec.wisc.edu/).

AMDC is a next generation open-source hardware and software control platform for power electronics and motor drives, designed and developed for academic applications. AMDC contains high performance dual-core digital signal processors, tightly integrated custom digital logic (FPGA), and a plethora of extremely flexible inputs and outputs (I/O) designed to interface to motor drives. Low-level firmware drivers have been developed to abstract away the complexities of the software / hardware interface, letting users focus on implementing custom control algorithms. Common use-case controller code is provided which gets new users up and running efficiently. AMDC empowers engineers to control advanced systems with ease.

## Documentation

Documentation has been written to help ease the process of using the AMDC platform. To get started, recommendation is given to read the docs in the order listed below. Once you understand the general [firmware architecture](docs/Firmware-Architecture.md), try [downloading the code, building it, and running](docs/Building-and-Running-Firmware.md) an example application (i.e., [`blink`](sdk/bare/user/usr/blink/)) on the AMDC hardware. After your application is stable, try [flashing](docs/Flashing-AMDC.md) it to the AMDC hardware for permanent usage.

If you run into low-level issues (i.e. strange restart issues), consider using the Xilinx [debugging tools](docs/Low-Level-Debugging.md) to investigate register state, etc.

### [Firmware Architecture](docs/Firmware-Architecture.md)
- [Drivers](docs/Firmware-Arch-Drivers.md)
- [System](docs/Firmware-Arch-System.md)
- [User Apps](docs/Firmware-Arch-UserApps.md)

### [GitHub to AMDC Hardware: Building and Running Firmware](docs/Building-and-Running-Firmware.md)
- [Create Private Repo](docs/Create-Private-Repo.md)

### [Flashing AMDC](docs/Flashing-AMDC.md)

### [Low-Level Debugging](docs/Low-Level-Debugging.md)

## License

This project is licensed under the BSD-3-Clause License - see the [LICENSE.md](LICENSE.md) file for details.
