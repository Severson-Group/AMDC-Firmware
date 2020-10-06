# Inverter Status Mux Driver

This driver is used to control and configure the DAC expansion board Rev B IP core in the FPGA through the AXI4-Lite interface.

## Files
All files for the inverter status mux driver are in the driver directory ([`common/drv/`](/sdk/bare/common/drv/)).

```
drv/
|-- dac.c
|-- dac.h
```

## Configuring the DAC
### Functions
```C
void dac_init(void)
```

This function initializes the DAC expansion board to +/- 10V output range with all channels synchronously triggering.

```C
void dac_set_raw(dac_reg_t reg, uint32_t value)
```

This function initiates a SPI transmission to the given register. Calling this function can be thought of as transmitting the given value directly to the given register in DAC IC on the expansion board.

## Driving the DAC
### Functions
```C
void dac_set_voltage(uint8_t ch, double voltage)
```

This function converts the given voltage into a hex code used by the DAC IC on the expansion board for setting the desired voltage. Calling this function will initiate a SPI transmission to the channel register on the DAC IC.

```C
void dac_set_broadcast(double voltage)
```

This function converts the given voltage into a hex code used by the DAC IC on the expansion board for setting the desired voltages on all broadcast enabled channels simultaneously.

```C
void dac_set_trigger(void)
```

This function initiates a trigger event on the DAC IC. All synchronous enabled channels will update their outputs with the current register value. 