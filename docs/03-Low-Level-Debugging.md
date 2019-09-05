# Low-Level Debugging

This document outlines various helpful commands for debugging the firmware. These will mostly use the Xilinx System Debugger (`xsdb`).
This is a command line tool which interacts with the hardware. One can program the FPGA, read memory and registers, etc.

See the [official Xilinx documentation](https://www.xilinx.com/support/documentation/sw_manuals/xilinx2014_3/SDK_Doc/concepts/sdk_c_xsd_xsdb_commands.htm) for more `xsdb` commands. Also, see this [helpful guide](https://github.com/imrickysu/ZYNQ-Custom-Board-Bring-Up-Guide) for debugging Zynq-based projects.


## Open `xsdb` prompt

1. Run `C:\Xilinx\Vivado\2017.2\bin\xsdb.bat`

## Connect to AMDC via `xsdb`

1. Ensure AMDC JTAG is plugged into PC
2. Ensure AMDC is powered on and in a reset state
3. `connect` -- should print `tcfchan#0` if connected
4. `targets` -- should print list of targets, 1-4

## Program AMDC through development environment

1. Load the FPGA bitstream and DSP firmware

## Read registers of running program

In `xsdb`:

1. `targets 2` -- connect to the ARM Cortex-A9 core that the code is running on
2. `rrd` -- prints registers with current values

## Read registers of hardware peripherials

In `xsdb`:

1. `targets 1` -- connect to APU
2. `rrd` -- lists all possible peripherials to read from
3. Example: `rrd uart0` -- read UART0 registers

## Read memory address

Ensure code if up and running. In `xsdb`:

1. `targets 2` -- connect to the running ARM Cortex-A9 core
2. `mrd <memory_address>` -- read from hex global memory address

## Determine why reboot occurs:

Boot AMDC with your code prior to reboot. Make sure it is running fine.

In `xsdb`:

1. `targets 1` -- connect to API
2. `rrd slcr` -- read from slcr h/w
3. Look for `reboot_status` in left column. This encodes the reason for the last reboot (persisted through POR). Bits should be set to indicate JTAG debug reset...
4. `rwr slcr reboot_status 0x0` -- clear the `REBOOT_STATUS` register
5. Force the reboot to occur by breaking the firmware, etc
6. `rrd slcr` -- reread the `REBOOT_STATUS` register
7. Lookup bits that have been set: https://www.xilinx.com/support/answers/52030.html
