# Contents of `sdk/`

**All the DSP related files.**

This folder contains the C code which is ran on the DSP on the AMDC hardware.

Each folder is a complete project which can be compiled and ran on the Xilinx Zynq-7000 SoC. There are currently two projects:

1. `basic` &mdash; Preliminary C code developed for ZedBoard (*archived and not maintained*)

- Designed for BM control (based on ZedBoard)
- Control performed in ISR context
- Real-time Ethernet (lwIP) / serial logging capabilities

2. `bare` &mdash; Minimal functionality, core framework for AMDC (**actively maintained**)

- Designed for AMDC hardware platform (using PicoZed processor module)
- Architecture based on generic tasks and commands
- Custom-designed cooperative RTOS
- User applications can be build neatly on top of RTOS without changed base code
- Logging capabilities (store buffer + dump via serial)
- Control algorithm injection point framework built into system
