# Changelog

This document summarizes the changes introduced to the code base for each release.

## v1.0.3

- Fix bug in Vivado project vs. C code config checking
- Reduce max number of samples to dump per time slice over Ethernet

## v1.0.2

- Fix task stats printing
- Fix PWM enable hardware driver function for REV E
- Fix FPGA timer address define for REV D
- Fix C drivers of GPIO expansion port IP cores
- Force SDK build error for hardware target mismatch
- Remove local docs folder in favor of docs website
- Rename `AMDC_Logger` method from `clear()` to `empty()`
- Extend `AMDC_Logger` method `auto_find_vars()` to accept regex input

## v1.0.1

- Fix GPIO subsystem firmware drivers mainly for REV E hardware

## v1.0.0

Initial commit of released code base.

- Dual-core operation (i.e. app_cpu0 and app_cpu1)
- Host interface supports both Ethernet and UART
- Logging supports both buffered and streaming
- Example real-time plotting of variables in Jupyter notebook
