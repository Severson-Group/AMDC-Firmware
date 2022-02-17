# Advanced Motor Drive Controller (AMDC) Firmware

![CI badge](https://github.com/Severson-Group/AMDC-Firmware/actions/workflows/main.yml/badge.svg)

*The Advanced Motor Drive Controller (AMDC) is an open-source project from the [Severson Research Group](https://severson.wempec.wisc.edu/) at [UW-Madison](http://www.engr.wisc.edu/department/electrical-computer-engineering/), affiliated with [Wisconsin Electric Machines and Power Electronics Consortium (WEMPEC)](https://wempec.wisc.edu/).*

Learn more at [docs.amdc.dev](https://docs.amdc.dev/).

---

**AMDC-Firmware** is a collection of embedded system code (written in C and Verilog) which runs on the [AMDC Hardware](https://github.com/Severson-Group/AMDC-Hardware) and controls advanced motor systems. It is open-source, high-performance, flexible, and research-oriented.

The target processor is the [Xilinx Zynq-7000 SoC](https://www.xilinx.com/products/silicon-devices/soc/zynq-7000.html) which includes both a dual-core DSP and tightly integrated FPGA. The AMDC firmware utilizes both parts of the processor (DSP + FPGA).

## Getting Started

1. Obtain a working hardware platform -- see the [obtaining hardware](https://docs.amdc.dev/hardware/obtaining-hardware.html) page for more.
2. Follow the steps outlined [here](https://docs.amdc.dev/firmware/xilinx-tools/building-and-running-firmware.html) to download, build, and run the firmware.
3. Read the [documentation](https://docs.amdc.dev/) for more insight into the platform.

## Documentation

Detailed documentation about the firmware is available online at: [docs.amdc.dev/firmware](https://docs.amdc.dev/firmware/).

## License

This project is licensed under the BSD-3-Clause License - see the [LICENSE.md](LICENSE.md) file for details.

## Contribute

Want to help build the open-source motor drive platform of choice? Join the people below -- **[Contribute today!](./CONTRIBUTING.md)**

<a href="https://github.com/Severson-Group/AMDC-Firmware/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=Severson-Group/AMDC-Firmware" />
</a>
