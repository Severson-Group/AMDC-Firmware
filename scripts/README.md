# Contents of `scripts/`

This folder contains miscellaneous scripts related to the AMDC firmware.

## AMDC Python Class

The AMDC can be interfaced to via the serial interface. This interface has been encapsulted into a simple python class which relies on the follow file:

1. `AMDC.py`

NOTE: these python scripts were built for compatibility with the `bare` DSP project.

## AMDC_Logger Python Class

The AMDC supports logging capabilities. Though it is possible to perform these logging functions directly through the serial terminal, it is very difficult. Because of this, an `AMDC_Logger` class was created to make logging easier. It relies on the following file:

1. `AMDC_Logger.py`

Documentation for logging on the AMDC can be found [here]()

## Auto-Formatting C Code via `clang-format`

The C code within `/sdk/bare/` is auto-formatted to keep a consistent coding style. See the [CONTRIBUTING](../CONTRIBUTING.md) file for information about how to use the auto-formater.
