# Contents of `scripts/`

This folder contains miscellaneous scripts that support usage of the AMDC. All scripts were built for compatibility with the `bare` DSP project.

## AMDC Python Class

`AMDC.py` is a Python class that encapsulates communication to the AMDC via the serial UART interface. This class provides several convenience functions to make communication to the AMDC more efficient than directly issuing commands via a command line terminal.

## AMDC_Logger Python Class

`AMDC_Logger.py` is a Python class that encapsulates the AMDC's logging capabilities. It is designed to be injected with an `AMDC.py` object for data communication. Although it is possible to perform logging functions directly by issuing serial commands from a terminal, doing so is challenging and inefficient. `AMDC_Logger.py` makes logging easy and allows for data to be directly interpretted and plotted by the wealth of libraries available via Python. Learn more [here](../docs/Firmware-Logging.md)

## Auto-Formatting C Code via `clang-format`

The C code within `/sdk/bare/` is auto-formatted to keep a consistent coding style. See the [CONTRIBUTING](../CONTRIBUTING.md) file for information about how to use the auto-formater.
