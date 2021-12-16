# Contents of `sdk/`

**All the DSP related files.**

This folder contains the C code which is ran on the DSP on the AMDC hardware.

Each folder is a complete project which can be compiled and ran on the Xilinx Zynq-7000 SoC. There are currently two projects:

## 1. `app_cpuX` &mdash; Core framework for AMDC (**actively maintained**)

Combination of both `app_cpu0` and `app_cpu1`:

- Designed for AMDC hardware platform (using PicoZed processor module)
- Dual-core AMP design (Ethernet core + real-time core)
- Based on generic cooperative tasks and commands
- User applications built neatly on top of core library code
- Logging capabilities (buffered memory + streaming)
- Control algorithm injection point framework

## 2. `basic` &mdash; Preliminary C code developed for ZedBoard (*archived and not maintained*)

- Designed for BM control (based on ZedBoard)
- Control performed in ISR context
- Real-time Ethernet (lwIP) / serial logging capabilities
