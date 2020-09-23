# Inverter Status Mux Driver

This driver is used to control and configure the inverter status mux IP core in the FPGA through the AXI4-Lite interface.

## Files
All files for the inverter status mux driver are in the driver directory ([`common/drv/`](/sdk/bare/common/drv/)).

```
drv/
|-- sts_mux.c
|-- sts_mux.h
```

## Configuring the Mux
### Macros

The following macros are used to configure which device driver each inverter port is mapped to upon initialization. Each port is set to the `STS_MUX_UNUSED` macro by default which maps the port status lines directly to the inverter IP core. Each port can be mapped to a specific driver IP core by setting the port macro to one of the devices (`STS_MUX_DEVICE1` - `STS_MUX_DEVICE8`)

`#define INVERTER_PORT1 STS_MUX_UNUSED`<br />
`#define INVERTER_PORT2 STS_MUX_UNUSED`<br />
`#define INVERTER_PORT3 STS_MUX_UNUSED`<br />
`#define INVERTER_PORT4 STS_MUX_UNUSED`<br />
`#define INVERTER_PORT5 STS_MUX_UNUSED`<br />
`#define INVERTER_PORT6 STS_MUX_UNUSED`<br />
`#define INVERTER_PORT7 STS_MUX_UNUSED`<br />
`#define INVERTER_PORT8 STS_MUX_UNUSED`

### Functions
`sts_mux_init(void)`

Initializes the inverter status mux with default configurations. All status A lines are configured as outputs and all status B, C, and D lines are configured as inputs. Each inverter port in mapped to the device defined in its respective macro in `sts_mux.h`.

`void sts_mux_set_line(port, device, line);`

Remaps a single status line. The register corresponding to the specified port and line is written to via the AXI4-Lite interface with the device address (1-8). 

`void sts_mux_set_device(port, device);`

Remaps all the status lines on the given port to the device. Each register corresponding to the status lines are written to via the AXI4-Lite interface with the device address (1-8).

`void sts_mux_set_output(line, bool);`

Configures all of the given lines (A, B, C or D) to be outputs if the bool is true or inputs if the bool is false.
