# Demo DQ Current Regulator

This user application implements modular dq-axis current control. Eight instances of the current controller are supported. Note that an amplitude invariant transformation is used for all DQ quantities.

## Supported Commands
`cc <cc_idx> init` - *Initialize current controller*

`cc <cc_idx> deinit` - *Deinitialize current controller*

`cc <cc_idx> vdc <voltage>` - *Set DC link voltage*

`cc <cc_idx> pwm <a|b|c> <pwm_chnl>` - *Configure PWM outputs per phase*

`cc <cc_idx> adc <a|b|c> <adc_chnl> <adc_gain> <adc_offset>` - *Configure ADC input per phase*

`cc <cc_idx> tune <Rs> <Ld> <Lq> <bw>` - *Tune with Rs [ohms], Ldq [H], and bandwidth [rad/s]*

`cc <cc_idx> set <Id*> <Iq*> <omega_e>` - *Set operating point for Idq [Adq] at omega_e [rad/s]*
    
## Usage

The `cc` command is used to interact with this application. Users can think of a current control block (cc) as a generic dq-axis current regulator block that has 3x ADC inputs (from current sensors) and 3x PWM outputs (to half-bridges), as well as an internal PI controller. There are 8x of these cc blocks available for usage. Users must configure the cc block parameters before using the module.

Note that each `cc` command is followed by the cc block index to which the command applies. Possible options are 0 to 7. Each index is an independent current controller block and has its own I/O, tuning, etc. The following assume we are setting up `cc` at index `0`.

Please type `help` for info on these `cc` commands.

### Set-up of I/O

You must define and set-up:
- DC link voltage
- PWM output channels
- ADC input channels, gain, offset

Type `help` for info on these arguments.

1. Configure DC link voltage to 30V: `cc 0 vdc 30`
2. Configure phase A PWM output (channel): `cc 0 pwm a 0`
3. Configure phase B PWM output (channel): `cc 0 pwm b 1`
4. Configure phase C PWM output (channel): `cc 0 pwm c 2`
5. Configure phase A ADC input (channel, gain, offset): `cc 0 adc a 0 10 0`
6. Configure phase A ADC input (channel, gain, offset): `cc 0 adc b 1 10 0`
7. Configure phase A ADC input (channel, gain, offset): `cc 0 adc c 2 10 0`

### Tuning

You must now tune the PI current controller. This depends on load resistance and inductance. You must supply the bandwidth in rads/s. 

Type `help` for info on these arguments.

1. Tune the PI controller: `cc 0 tune 1 1.5 1.5 1000`

### Regulating Current

Now, the cc block is set-up and ready to use!! You can now turn it on, set its references, and turn it off.

Type `help` for info on these commands.

1. Turn on the current controller: `cc 0 init`
2. Regulate 10 amps of q-axis current at 100 rad/s: `cc 0 set 0 10 100`
3. Turn off the current controller: `cc 0 deinit`

Note that when omega* = 0, theta is set to 0.
