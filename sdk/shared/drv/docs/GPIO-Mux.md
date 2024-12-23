# Inverter GPIO Mux Driver

This driver is used to control and configure the GPIO mux IP core in the FPGA through the AXI4-Lite interface.

## Files
All files for the GPIO mux driver are in the driver directory ([`/sdk/app_cpu1/common/drv/`](/sdk/app_cpu1/common/drv/)).

```
drv/
|-- gpio_mux.c
|-- gpio_mux.h
```
## Default Connections and Operation

The mux IP can be connected to one of eight different IP drivers

## Configuring the Mux

The following functions are used to configuring the GPIO mux IP. The `gpio_mux_init()` function is called upon initialization of the AMDC and sets the GPIO mux into a default state defined by the macros as described below. The `gpio_mux_set_device()` function is the intended way of configuring the GPIO mux IP. The function must be called by the user app. During runtime, the CLI commands can be used to configure the mux. 

### Functions

`gpio_mux_init(void)`

Initializes the GPIO mux with default configurations. Each IsoSPI port is mapped to the device drivers defined in its respective macro in `gpio_mux.h`.


`void gpio_mux_set_device(port, device);`

Remaps the GPIO lines on the specified IsoSPI port to the specified device driver. Each register corresponding to the IsoSPI port are written to via the AXI4-Lite interface with the device address (1-4).

### CLI Commands

The GPIO mux can be configured at runtime using commands entered in the command line interface. The command follows the form `hw mux gpio <port> <device>` where `<port>` and `<device>` specify which device driver is connected to which IsoSPI port.

### Macros

The following macros are used to configure which device driver each IsoSPI port is mapped to upon initialization.  

Each port is set to the `GPIO_MUX_UNUSED` macro by default which disables the GPIO lines on the port by holding the lines to logic low. Each port can be mapped to a specific driver IP core upon initialization of the AMDC by setting the port macro to one of the devices (`GPIO_MUX_DEVICE1` - `GPIO_MUX_DEVICE4`)

```C
#define GPIO_PORT1 GPIO_MUX_UNUSED

#define GPIO_PORT2 GPIO_MUX_UNUSED
```
