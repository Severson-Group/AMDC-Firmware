# Eddy Current Sensor Driver

This driver is used to control and configure the AMDC eddy current sensor IP core in the FPGA through the AXI4-Lite interface.

## Files
All files for the eddy current sensor driver are in the driver directory ([`common/drv/`](/sdk/bare/common/drv/)).

```
drv/
|-- eddy_current_sensor.c
|-- eddy_current_sensor.h
```

## Configuring the AMDC

The [GPIO mux](GPIO_Mux.md) is used to map IP cores to the physical GPIO (IsoSPI) ports. It must be configured to map the `eddy_current_sensor_1.0` IP core to the intended GPIO port. The following function can be used to configure the GPIO port mapping:

```C 
void gpio_mux_set_device(port, device);
```

By default the `eddy_current_sensor_1.0` IP core is connected to device 1 in the FPGA firmware. The example below would configure the GPIO mux to map the IP core to port 1.

```C
gpio_mux_set_device(1, 1);
```

## Driving the Eddy Current Sensor
### Functions
```C
void eddy_current_sensor_init(void)
```

This function is called by the BSP and initializes the eddy_current_sensor sample rate to 20kHz to match the AMDC callback frequency.

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

This function sets frequency of the SPI clock and conversion signal to match the given sample_rate parameter in Hz. The SPI clock is 25 times greater than the sample rate. 

```C
void eddy_current_sensor_set_divider(uint8_t divider)
```

This function sets the IP register used to modulate the SPI frequency.

```C
double eddy_current_sensor_read_x_voltage(void)
```

This function reads the X data from the IP data register. The returned data is sign extended 2's compliment 18-bit data.

```C
double eddy_current_sensor_read_y_voltage(void)
```

This function reads the Y data from the IP data register. The returned data is sign extended 2's compliment 18-bit data.

## Example Code

The desired sampling rate for the eddy current sensor should be set in the users initialization of their C drivers. The sampling rate ranges from 2KSps to 500KSps. The sensor data can be read from an existing callback function or one can be created and registered with FreeRTOS. The following code is a simple example of a scheduled callback function that reads the sensors positional data:

```C
///////////////////////////////////
//  In my_driver.c  
///////////////////////////////////

void my_driver_init() {

  // Set the sample rate of the eddy current sensor to 40KSps (1MHz SPI clock)
  eddy_current_sensor_set_sample_rate(40000);

  // Initializes a task to read the eddy current sensor data
  task_eddy_current_sensor_init();

  
  // More Initialization code...

  return;
}


///////////////////////////////////
//  In task_eddy_current_sensor.c   
//////////////////////////////////

// Scheduler TCB which holds task "context"
static task_control_block_t tcb;

int task_eddy_current_sensor_init(void)
{
    if (scheduler_tcb_is_registered(&tcb)) {
        return FAILURE;
    }

    // Fill TCB with parameters
    scheduler_tcb_init(&tcb, task_eddy_current_sensor_callback, NULL, "eddy_current", TASK_DAC_INTERVAL_USEC);

    // Register task with scheduler
    return scheduler_tcb_register(&tcb);
}

void task_eddy_current_sensor_callback(void *arg)
{

  // Read X and Y positional data as voltage
  double voltage_x = eddy_current_sensor_read_x_voltage();
  double voltage_y = eddy_current_sensor_read_y_voltage();


  // Use the positional data
  my_bearing_control_function(voltage_x, voltage_y);

  return;
}
```