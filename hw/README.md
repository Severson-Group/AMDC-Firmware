# `hw/` Contents

**All the FPGA project related files.**

This folder contains the FPGA design resources needed to build the Xilinx Vivado project. This project is used to compile and build the FPGA bitstream needed for AMDC operation. Only a minimal number of files are needed since most of the platform-specific HDL is structured into IP blocks (see the [`ip_repo/`](../ip_repo/) folder).

## File Description

- `constraints_amdc_rev*.xdc` &mdash; stores the Vivado FPGA constraints (pin voltage levels + pin mappings to HDL netlist)
- `design_1.bd` &mdash; main block diagram used in Vivado (stores logical structure of full FPGA contents)
- `design_1_wrapper.v` &mdash; Verilog wrapper for the block diagram file. Generated based upon the block diagram.
