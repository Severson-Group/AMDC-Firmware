# DAC Expansion Board Application

This user application implements commands executed via the CLI for the DAC expansion board Rev B. The commands act as a direct interface to the internal registers on the [DAC60508MC]() IC for configuration. In addition, a sample 3-phase +/- 10V signal on DAC channels 0, 2, and 4 is implemented as an example application for the DAC board.

## Supported Commands
`dac init` - *Initialize 3-phase signals*

`dac deinit` - *Deinitialize 3-phase signals*

`dac freq <frequency(Hz)>` - *Set the frequency of the 3-phase signals (the resolution will drop as the frequency increases)*

`dac ch <channel> <voltage>` - *Configure channel output voltage*

`dac reg <register> <value(0xXXXX)>` - *Configure register value in hex*

`dac trigger` - *Triggers a synchronous update*

`dac broadcast <voltage>` - *Configure all broadcast channel outputs voltage*
    
## Usage

The `dac` command is used to interact with this application. The `init`, `deinit`, and `freq` sub-commands are used to operate the sample function generator application. The remaining sub-commands directly interact with the DAC expansion board and are used to configure the operation of the DAC board.

The `dac ch` command expects a channel number from 0-7 and a voltage between -10V and +10V. By default, the DAC is initialized with all channels in synchronous mode. The `dac trigger` command must be issued to flush the buffered channel CODE to its output. Below is an example of the `dac ch` and `dac trigger` commands:

```
dac ch 0 10     // Sets the CODE for +10V (0xFFF) in the buffer for channel 0
dac ch 4 -10    // Sets the CODE for -10V (0x000) in the buffer for channel 4
dac trigger     // Flushes the buffered CODE to the output of the channels
```

The `dac broadcast` command expects a voltage between -10V and +10V. By default, the DAC is initialized with broadcast enabled on all channels. Issuing the `dac broadcast` command will set the CODE in the BRDCAST data register and all channels outputs will update to the broadcast voltage.

The `dac reg` command expects a register number between 0-15 and a hex value to be written to the register. Issuing the `dac reg` command will write the hex value to the given register offset. This can be used to write data to the configuration registers in the DAC. The register numbers and values directly mirror the registers in the [DAC60508MC](). More information about the registers can be found in the datasheet. The example below configures channels 4-7 to be asynchronous.

```
dac reg 2 FF0F   // Bits [7:4] correspond to channels 7-4 synchronous enable
```

## Files

All application files for the DAC are in the application directory (`common/usr/dac/`).

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

- This is the top-level application file that registers the user application and commands with the scheduler

### [`task_dac.c`](task_dac.c)

- This task generates a 3-phase +/- 10V signal on DAC channels 0-2 updated at 1Hz

### [`cmd/cmd_dac.c`](cmd/cmd_dac.c)

- This contains the commands available through the DAC user application

## Configuring the DAC

The [DAC60508MC]() has several configuration registers that can be used to change each channel to be synchronous or asynchronous, enable broadcasting, and change the gain of the output. In the initialization function in [`dac.c`](../drv/dac.c), two important configurations are made. 

The first configuration that is made must be performed for the DAC to operate properly. The GAIN register must be set to 0x01FF such that the direct output of each channel is between 0 and V<sub>REF</sub>. 
 
The second configuration performed sets all channels to be updated synchronously. Bits [7:0] in the SYNC register enable synchronous mode, bits[15:8] enable broadcast mode. 0xFFFF is written to the SYNC register such that all channels are synchronous with broadcast enabled. `Note:` This requires `dac_set_trigger()` to be called or the `dac trigger` command to be issued for the voltages to appear. This configuration only affects when buffer flushes are triggered within the DAC and is not critical to operation.

## Configuring the Function Generator

The callback function in [`task_dac.c`](task_dac.c) can be supplied any waveform or function desired. In the 3-phase example, a sinusoid is implemented. The frequency of the output signal is determined by the FREQ parameter. The callback frequency is 10KHz by default but will always output a 1Hz signal multiplied by FREQ. The frequency can be set to any value however as the frequency increases the resolution of the DAC channels will decrease. The exact resolution can be determined by the following equation:

Resolution (bits) = log <sub>2</sub>( Callback Frequency / Desired Frequency )

`Note: The maximum resolution of the DAC is 12-bits`

Within the callback function `dac_set_voltage(ch, voltage, min, max)` is called to write the voltage from the function to the channel buffers on the DAC.`dac_set_trigger()` is used to flush the buffers to the channel outputs each cycle. 