# Changelog

This document summarizes the changes introduced to the code base for each release.

## v1.1.0

- Add GPIO driver for direct pin-level access
- Support encoders with non `2^n` pulses per revolution
- Synchronize encoder feedback to PWM carrier
- Synchronize Kaman eddy current sensor feedback to PWM carrier
- Latch user PWM duty request at both high/low of triangle carrier
- Fix AMDS data requesting on REV D target
- Add requirements.txt for Python interface dependencies

## v1.0.5

- Fix GitHub action for code format check
- Throw exception when mapfile is not given to `AMDC_Logger` python class

## v1.0.4

- Fix injection period inconsistencies
- Add ramp injection function
- Add AMDS driver counter access API
- Add top priority scheduler task register method

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
