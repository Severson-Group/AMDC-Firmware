# Contents of `scripts/`

This folder contains miscellaneous scripts related to the AMDC firmware.

## AMDC Python Class

The AMDC can be interfaced to via the serial interface. This interface has been encapsulted into a simple python class which relies on the follow two files:

1. `AMDC.py`
2. `Mapfile.py`

NOTE: these python scripts were built for compatibility with the `bare` DSP project.

## Auto-Formatting C Code via `clang-format`

The C code within `/sdk/bare/` is auto-formatted to keep a consistent coding style. See the [CONTRIBUTING](../CONTRIBUTING.md) file for information about how to use the auto-formater.
