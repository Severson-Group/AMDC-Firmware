# Contents of `sdk/`

**All the DSP related files.**

This folder contains the C code which is ran on the DSP on the AMDC hardware.

## Application Folders

The following folders are complete application projects which can be compiled and ran on the Xilinx Zynq-7000 SoC:

### 1. `app_cpuX` &mdash; Core framework for AMDC (**actively maintained**)

Combination of both `app_cpu0` and `app_cpu1`:

- Designed for AMDC hardware platform (using PicoZed processor module)
- Dual-core AMP design (Ethernet core + real-time core)
- Based on generic cooperative tasks and commands
- User applications built neatly on top of core library code
- Logging capabilities (buffered memory + streaming)
- Control algorithm injection point framework

### 2. `freertos_app_cpuX` &mdash; Core framework for AMDC (**actively maintained**)

Combination of both `freertos_app_cpu0` and `freertos_app_cpu1`:

- Designed for AMDC hardware platform (using PicoZed processor module)
- Dual-core AMP design
- Based on [FreeRTOS](https://freertos.org/)
- User applications built neatly on top of core library code

### 3. `basic` &mdash; Preliminary C code developed for ZedBoard (*archived and not maintained*)

- Designed for BM control (based on ZedBoard)
- Control performed in ISR context
- Real-time Ethernet (lwIP) / serial logging capabilities

## Source Folders

The following folders contain source code reference by the applications:

### 1. `FreeRTOS-Kernel`

- Is a [Git submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules) of the [FreeRTOS-Kernel](https://github.com/FreeRTOS/FreeRTOS-Kernel) source code, referenced by the FreeRTOS application projects

### 2. `shared`

- Contains system level and hardware driver code commonly referenced by `v2` application projects
- See `shared/README.md` for more details

