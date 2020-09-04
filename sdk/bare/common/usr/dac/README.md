# DAC Expansion Board Application

This user application implements a 3-phase +/- 10V signal on DAC channels 0, 1, and 2 and commands to configure and drive the DAC.  

## Supported Commands
`dac init` - *Initialize 3-phase signals*

`dac deinit` - *Deinitialize 3-phase signals*

`dac freq <frequency(Hz)>` - *Set the frequency of the 3-phase signals (the resolution will drop as the frequency increases)*

`dac ch <channel> <voltage>` - *Configure channel output voltage*

`dac reg <register> <value(0xXXXX)>` - *Configure register value in hex*

`dac trigger` - *Triggers a synchronous update*

`dac broadcast <voltage>` - *Configure all broadcast channel outputs voltage*
    
## Usage

The `dac` command is used to interact with this application. The `init`, `deinit`, and `freq` sub commands are used to operate the sample function generator application. The remaing sub commands directly interact with the DAC expansion board.

The `dac ch` commands expects a channel number from 0-7 and a volage between -10 and 10
The `dac reg` commands expects a register number between 0-15 and a hex value to be written to the register. The register numbers and values directly mirror the register on the [DAC60508MC](). More information about the registers can be found in the datasheet.

## Files

The application directory structure contains the minimal set of files to have a functioning app with a single command and task. All application files live in the application directory (`common/usr/blink/`).

```
dac/
|-- cmd/
|   |-- cmd_dac.c
|   |-- cmd_dac.h
|-- app_dac.c
|-- app_dac.h
|-- task_dac.c
|-- task_dac.h
```

### [`app_dac.c`](app_dac.c)

- This is the top level application file that registers the user application and commands with the scheduler

### [`task_dac.c`](task_dac.c)

- This task generates a 3-phase +/- 10V signal on DAC channels 0-2 updates at 1Hz

### [`cmd/cmd_dac.c`](cmd/cmd_dac.c)

- This contains the commands available through the DAC user application

### Configuring the Function Generator

The callback function in [`task_dac.c`](task_dac.c) can be supplied any wafeform or function desired. In the 3-phase example, a sinusoid is implemented. The frequency of the output signal is determined by FREQ parameter. The callback frequency is 10KHz by default but will always output a 1Hz signal multiplied by FREQ. The frequency can be set to any value however as the frequency increases the resolution of the DAC channels will decrease. The exact resolution can be determined by the following equation:

Resolution (bits) = log <sub>2</sub>( Callback Frequency / Desired Frequency )

`Note: The maximum resolution of the DAC is 12-bits`