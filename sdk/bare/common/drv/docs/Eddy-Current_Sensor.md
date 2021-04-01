# DAC Expansion Board Driver

This driver is used to control and configure the AMDC eddy current sensor IP core in the FPGA through the AXI4-Lite interface.

## Files
All files for the eddy current sensor driver are in the driver directory ([`common/drv/`](/sdk/bare/common/drv/)).

```
drv/
|-- eddy_current_sensor.c
|-- eddy_current_sensor.h
```

## Driving the Eddy Current Sensor
### Functions
```C
void eddy_current_sensor_init(void)
```

This function initializes the eddy_current_sensor sample rate to 20kHz to match the AMDC callback frequency.

```C
void eddy_current_sensor_enable(void)
```

This function enables SPI transmissions to the eddy current sensor. SPI transmission will occur continuously until the drivers are disabled

```C
void eddy_current_sensor_disable(void)
```

This function disables SPI transmissions to the eddy current sensor.

```C
void eddy_current_sensor_set_sample_rate(double sample_rate)
```

This function sets frequency of the SPI clock and conversion signal to match the given sample_rate parameter in Hz.

```C
void eddy_current_sensor_set_divider(uint8_t divider)
```

This function sets the IP register used to modulate the SPI frequency.

```C
double eddy_current_sensor_read_x_voltage(void)
```

This function reads the X data from the IP data register.

```C
double eddy_current_sensor_read_y_voltage(void)
```

This function reads the Y data from the IP data register.