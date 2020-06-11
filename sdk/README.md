# `sdk/` Contents

This folder contains the C code used on the AMDC.

Each folder is a complete project which can be compiled and ran on the Xilinx Zynq-7000 SoC. There are currently two projects:

1. `basic` &mdash; C code written by Eric Severson for his PhD dissertation

- Designed for BM control on PECB platform (similar to AMDC, but based on ZedBoard; developed at UMN)
- Control performed in ISR context
- Real-time Ethernet (lwIP) / serial logging capabilities

2. `bare` &mdash; C code written by Nathan Petersen at UW-Madison

- Designed for AMDC hardware platform (using PicoZed processor module)
- Architecture based on generic tasks and commands
- Custom-designed cooperative RTOS
- User applications can be build neatly on top of RTOS without changed base code
- Logging capabilities (store buffer + dump via serial)
- Control algorithm injection point framework built into system
