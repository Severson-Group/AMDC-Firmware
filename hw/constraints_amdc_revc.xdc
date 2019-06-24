###########
# ENCODER #
###########

set_property IOSTANDARD LVCMOS18 [get_ports encoder_a];
set_property IOSTANDARD LVCMOS18 [get_ports encoder_b];
set_property IOSTANDARD LVCMOS18 [get_ports encoder_z];
set_property IOSTANDARD LVCMOS18 [get_ports encoder_alarm_a];
set_property IOSTANDARD LVCMOS18 [get_ports encoder_alarm_b];
set_property IOSTANDARD LVCMOS18 [get_ports encoder_alarm_z];
set_property IOSTANDARD LVCMOS18 [get_ports encoder_alarm_d];

set_property PACKAGE_PIN A5  [get_ports encoder_a]; # JX1-68
set_property PACKAGE_PIN A7  [get_ports encoder_b]; # JX1-74
set_property PACKAGE_PIN C8  [get_ports encoder_z]; # JX1-82
set_property PACKAGE_PIN D6  [get_ports encoder_alarm_a]; # JX1-64
set_property PACKAGE_PIN A4  [get_ports encoder_alarm_b]; # JX1-70
set_property PACKAGE_PIN A6  [get_ports encoder_alarm_z]; # JX1-76
set_property PACKAGE_PIN B8  [get_ports encoder_alarm_d]; # JX1-84


#########
# ADC 1 #
#########

set_property IOSTANDARD LVCMOS18 [get_ports adc1_sdo[0]];
set_property IOSTANDARD LVCMOS18 [get_ports adc1_sdo[1]];
set_property IOSTANDARD LVCMOS18 [get_ports adc1_sdo[2]];
set_property IOSTANDARD LVCMOS18 [get_ports adc1_sdo[3]];
set_property IOSTANDARD LVCMOS18 [get_ports adc1_sdo[4]];
set_property IOSTANDARD LVCMOS18 [get_ports adc1_sdo[5]];
set_property IOSTANDARD LVCMOS18 [get_ports adc1_sdo[6]];
set_property IOSTANDARD LVCMOS18 [get_ports adc1_sdo[7]];
set_property IOSTANDARD LVCMOS18 [get_ports adc1_sck];
set_property IOSTANDARD LVCMOS18 [get_ports adc1_cnv];
set_property IOSTANDARD LVCMOS18 [get_ports adc1_clkout];

set_property PACKAGE_PIN G4 [get_ports adc1_cnv]; # JX1-23 
set_property PACKAGE_PIN D1 [get_ports adc1_sck]; # JX1-53 
set_property PACKAGE_PIN D8 [get_ports adc1_clkout]; # JX1-37
set_property PACKAGE_PIN F4 [get_ports adc1_sdo[0]]; # JX1-25                                                              
set_property PACKAGE_PIN G6 [get_ports adc1_sdo[1]]; # JX1-29
set_property PACKAGE_PIN F6 [get_ports adc1_sdo[2]]; # JX1-31
set_property PACKAGE_PIN E8 [get_ports adc1_sdo[3]]; # JX1-35
set_property PACKAGE_PIN C6 [get_ports adc1_sdo[4]]; # JX1-41
set_property PACKAGE_PIN C5 [get_ports adc1_sdo[5]]; # JX1-43
set_property PACKAGE_PIN B4 [get_ports adc1_sdo[6]]; # JX1-47
set_property PACKAGE_PIN B3 [get_ports adc1_sdo[7]]; # JX1-49


#########
# ADC 2 #
#########

set_property IOSTANDARD LVCMOS18 [get_ports adc2_sdo[0]];
set_property IOSTANDARD LVCMOS18 [get_ports adc2_sdo[1]];
set_property IOSTANDARD LVCMOS18 [get_ports adc2_sdo[2]];
set_property IOSTANDARD LVCMOS18 [get_ports adc2_sdo[3]];
set_property IOSTANDARD LVCMOS18 [get_ports adc2_sdo[4]];
set_property IOSTANDARD LVCMOS18 [get_ports adc2_sdo[5]];
set_property IOSTANDARD LVCMOS18 [get_ports adc2_sdo[6]];
set_property IOSTANDARD LVCMOS18 [get_ports adc2_sdo[7]];
set_property IOSTANDARD LVCMOS18 [get_ports adc2_sck];
set_property IOSTANDARD LVCMOS18 [get_ports adc2_cnv];
set_property IOSTANDARD LVCMOS18 [get_ports adc2_clkout];

set_property PACKAGE_PIN C1   [get_ports adc2_cnv]; # JX1-55
set_property PACKAGE_PIN AA15 [get_ports adc2_sck]; # JX1-89
set_property PACKAGE_PIN G8   [get_ports adc2_clkout]; # JX1-73
set_property PACKAGE_PIN E2   [get_ports adc2_sdo[0]]; # JX1-61                                                              
set_property PACKAGE_PIN D2   [get_ports adc2_sdo[1]]; # JX1-63
set_property PACKAGE_PIN F7   [get_ports adc2_sdo[2]]; # JX1-67
set_property PACKAGE_PIN E7   [get_ports adc2_sdo[3]]; # JX1-69
set_property PACKAGE_PIN G7   [get_ports adc2_sdo[4]]; # JX1-75
set_property PACKAGE_PIN B7   [get_ports adc2_sdo[5]]; # JX1-81
set_property PACKAGE_PIN B6   [get_ports adc2_sdo[6]]; # JX1-83
set_property PACKAGE_PIN AA14 [get_ports adc2_sdo[7]]; # JX1-87


##########
# GPIO A #
##########

set_property IOSTANDARD LVCMOS18 [get_ports gpioa[0]];
set_property IOSTANDARD LVCMOS18 [get_ports gpioa[1]];
set_property IOSTANDARD LVCMOS18 [get_ports gpioa[2]];
set_property IOSTANDARD LVCMOS18 [get_ports gpioa[3]];
set_property IOSTANDARD LVCMOS18 [get_ports gpioa[4]];
set_property IOSTANDARD LVCMOS18 [get_ports gpioa[5]];
set_property IOSTANDARD LVCMOS18 [get_ports gpioa[6]];
set_property IOSTANDARD LVCMOS18 [get_ports gpioa[7]];
set_property IOSTANDARD LVCMOS18 [get_ports gpioa[8]];
set_property IOSTANDARD LVCMOS18 [get_ports gpioa[9]];
set_property IOSTANDARD LVCMOS18 [get_ports gpioa[10]];
set_property IOSTANDARD LVCMOS18 [get_ports gpioa[11]];

set_property PACKAGE_PIN V18  [get_ports gpioa[0]]; # JX1-92
set_property PACKAGE_PIN Y15  [get_ports gpioa[1]]; # JX1-90
set_property PACKAGE_PIN Y14  [get_ports gpioa[2]]; # JX1-88
set_property PACKAGE_PIN AA20 [get_ports gpioa[3]]; # JX2-99
set_property PACKAGE_PIN AA19 [get_ports gpioa[4]]; # JX2-97
set_property PACKAGE_PIN AB22 [get_ports gpioa[5]]; # JX2-95
set_property PACKAGE_PIN AB21 [get_ports gpioa[6]]; # JX2-93
set_property PACKAGE_PIN M7   [get_ports gpioa[7]]; # JX2-89
set_property PACKAGE_PIN P8   [get_ports gpioa[8]]; # JX2-90
set_property PACKAGE_PIN AB18 [get_ports gpioa[9]]; # JX2-94
set_property PACKAGE_PIN AB19 [get_ports gpioa[10]]; # JX2-96
set_property PACKAGE_PIN T16  [get_ports gpioa[11]]; # JX2-100


##########
# GPIO B #
##########

set_property IOSTANDARD LVCMOS18 [get_ports gpiob[0]];
set_property IOSTANDARD LVCMOS18 [get_ports gpiob[1]];
set_property IOSTANDARD LVCMOS18 [get_ports gpiob[2]];
set_property IOSTANDARD LVCMOS18 [get_ports gpiob[3]];
set_property IOSTANDARD LVCMOS18 [get_ports gpiob[4]];
set_property IOSTANDARD LVCMOS18 [get_ports gpiob[5]];
set_property IOSTANDARD LVCMOS18 [get_ports gpiob[6]];
set_property IOSTANDARD LVCMOS18 [get_ports gpiob[7]];
set_property IOSTANDARD LVCMOS18 [get_ports gpiob[8]];
set_property IOSTANDARD LVCMOS18 [get_ports gpiob[9]];
set_property IOSTANDARD LVCMOS18 [get_ports gpiob[10]];
set_property IOSTANDARD LVCMOS18 [get_ports gpiob[11]];

set_property PACKAGE_PIN D7 [get_ports gpiob[0]]; # JX1-62
set_property PACKAGE_PIN A1 [get_ports gpiob[1]]; # JX1-56
set_property PACKAGE_PIN A2 [get_ports gpiob[2]]; # JX1-54
set_property PACKAGE_PIN C3 [get_ports gpiob[3]]; # JX1-50
set_property PACKAGE_PIN D3 [get_ports gpiob[4]]; # JX1-48
set_property PACKAGE_PIN C4 [get_ports gpiob[5]]; # JX1-44
set_property PACKAGE_PIN D5 [get_ports gpiob[6]]; # JX1-42
set_property PACKAGE_PIN G1 [get_ports gpiob[7]]; # JX1-38
set_property PACKAGE_PIN E3 [get_ports gpiob[8]]; # JX1-26
set_property PACKAGE_PIN B2 [get_ports gpiob[9]]; # JX1-30
set_property PACKAGE_PIN B1 [get_ports gpiob[10]]; # JX1-32
set_property PACKAGE_PIN H1 [get_ports gpiob[11]]; # JX1-36



#############
# INVERTERS #
#############

set_property IOSTANDARD LVCMOS18 [get_ports inverter1_pwm[0]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter1_pwm[1]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter1_pwm[2]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter1_pwm[3]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter1_pwm[4]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter1_pwm[5]];

set_property IOSTANDARD LVCMOS18 [get_ports inverter2_pwm[0]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter2_pwm[1]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter2_pwm[2]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter2_pwm[3]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter2_pwm[4]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter2_pwm[5]];

set_property IOSTANDARD LVCMOS18 [get_ports inverter3_pwm[0]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter3_pwm[1]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter3_pwm[2]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter3_pwm[3]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter3_pwm[4]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter3_pwm[5]];

set_property IOSTANDARD LVCMOS18 [get_ports inverter4_pwm[0]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter4_pwm[1]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter4_pwm[2]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter4_pwm[3]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter4_pwm[4]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter4_pwm[5]];

set_property IOSTANDARD LVCMOS18 [get_ports inverter5_pwm[0]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter5_pwm[1]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter5_pwm[2]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter5_pwm[3]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter5_pwm[4]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter5_pwm[5]];

set_property IOSTANDARD LVCMOS18 [get_ports inverter6_pwm[0]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter6_pwm[1]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter6_pwm[2]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter6_pwm[3]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter6_pwm[4]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter6_pwm[5]];

set_property IOSTANDARD LVCMOS18 [get_ports inverter7_pwm[0]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter7_pwm[1]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter7_pwm[2]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter7_pwm[3]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter7_pwm[4]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter7_pwm[5]];

set_property IOSTANDARD LVCMOS18 [get_ports inverter8_pwm[0]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter8_pwm[1]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter8_pwm[2]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter8_pwm[3]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter8_pwm[4]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter8_pwm[5]];

set_property IOSTANDARD LVCMOS18 [get_ports inverter_rst[0]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_rst[1]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_rst[2]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_rst[3]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_rst[4]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_rst[5]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_rst[6]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_rst[7]];

set_property IOSTANDARD LVCMOS18 [get_ports inverter_rdy[0]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_rdy[1]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_rdy[2]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_rdy[3]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_rdy[4]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_rdy[5]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_rdy[6]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_rdy[7]];

set_property IOSTANDARD LVCMOS18 [get_ports inverter_flt_desat[0]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_flt_desat[1]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_flt_desat[2]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_flt_desat[3]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_flt_desat[4]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_flt_desat[5]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_flt_desat[6]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_flt_desat[7]];

set_property IOSTANDARD LVCMOS18 [get_ports inverter_flt_temp[0]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_flt_temp[1]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_flt_temp[2]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_flt_temp[3]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_flt_temp[4]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_flt_temp[5]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_flt_temp[6]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_flt_temp[7]];

set_property PACKAGE_PIN L7 [get_ports inverter1_pwm[0]]; # JX2-25
set_property PACKAGE_PIN K7 [get_ports inverter1_pwm[1]]; # JX2-23
set_property PACKAGE_PIN J2 [get_ports inverter1_pwm[2]]; # JX2-18
set_property PACKAGE_PIN J1 [get_ports inverter1_pwm[3]]; # JX2-20
set_property PACKAGE_PIN M3 [get_ports inverter1_pwm[4]]; # JX2-19
set_property PACKAGE_PIN M4 [get_ports inverter1_pwm[5]]; # JX2-17

set_property PACKAGE_PIN J3 [get_ports inverter2_pwm[0]]; # JX2-24
set_property PACKAGE_PIN K2 [get_ports inverter2_pwm[1]]; # JX2-26
set_property PACKAGE_PIN P7 [get_ports inverter2_pwm[2]]; # JX2-29
set_property PACKAGE_PIN R7 [get_ports inverter2_pwm[3]]; # JX2-31
set_property PACKAGE_PIN R8 [get_ports inverter2_pwm[4]]; # JX2-14
set_property PACKAGE_PIN H8 [get_ports inverter2_pwm[5]]; # JX2-13

set_property PACKAGE_PIN M1 [get_ports inverter3_pwm[0]]; # JX2-43
set_property PACKAGE_PIN M2 [get_ports inverter3_pwm[1]]; # JX2-41
set_property PACKAGE_PIN P3 [get_ports inverter3_pwm[2]]; # JX2-36
set_property PACKAGE_PIN P2 [get_ports inverter3_pwm[3]]; # JX2-38
set_property PACKAGE_PIN N3 [get_ports inverter3_pwm[4]]; # JX2-37
set_property PACKAGE_PIN N4 [get_ports inverter3_pwm[5]]; # JX2-35

set_property PACKAGE_PIN K4 [get_ports inverter4_pwm[0]]; # JX2-47
set_property PACKAGE_PIN K3 [get_ports inverter4_pwm[1]]; # JX2-49
set_property PACKAGE_PIN N1 [get_ports inverter4_pwm[2]]; # JX2-42
set_property PACKAGE_PIN P1 [get_ports inverter4_pwm[3]]; # JX2-44
set_property PACKAGE_PIN L2 [get_ports inverter4_pwm[4]]; # JX2-30
set_property PACKAGE_PIN L1 [get_ports inverter4_pwm[5]]; # JX2-32

set_property PACKAGE_PIN R2 [get_ports inverter5_pwm[0]]; # JX2-63
set_property PACKAGE_PIN R3 [get_ports inverter5_pwm[1]]; # JX2-61
set_property PACKAGE_PIN U2 [get_ports inverter5_pwm[2]]; # JX2-54
set_property PACKAGE_PIN U1 [get_ports inverter5_pwm[3]]; # JX2-56
set_property PACKAGE_PIN T1 [get_ports inverter5_pwm[4]]; # JX2-55
set_property PACKAGE_PIN T2 [get_ports inverter5_pwm[5]]; # JX2-53

set_property PACKAGE_PIN L6 [get_ports inverter6_pwm[0]]; # JX2-62
set_property PACKAGE_PIN M6 [get_ports inverter6_pwm[1]]; # JX2-64
set_property PACKAGE_PIN J5 [get_ports inverter6_pwm[2]]; # JX2-67
set_property PACKAGE_PIN K5 [get_ports inverter6_pwm[3]]; # JX2-69
set_property PACKAGE_PIN L5 [get_ports inverter6_pwm[4]]; # JX2-48
set_property PACKAGE_PIN L4 [get_ports inverter6_pwm[5]]; # JX2-50

set_property PACKAGE_PIN K8 [get_ports inverter7_pwm[0]]; # JX2-83
set_property PACKAGE_PIN J8 [get_ports inverter7_pwm[1]]; # JX2-81
set_property PACKAGE_PIN P6 [get_ports inverter7_pwm[2]]; # JX2-74
set_property PACKAGE_PIN P5 [get_ports inverter7_pwm[3]]; # JX2-76
set_property PACKAGE_PIN R5 [get_ports inverter7_pwm[4]]; # JX2-68
set_property PACKAGE_PIN R4 [get_ports inverter7_pwm[5]]; # JX2-70

set_property PACKAGE_PIN N8 [get_ports inverter8_pwm[0]]; # JX2-88
set_property PACKAGE_PIN M8 [get_ports inverter8_pwm[1]]; # JX2-87
set_property PACKAGE_PIN N6 [get_ports inverter8_pwm[2]]; # JX2-82
set_property PACKAGE_PIN N5 [get_ports inverter8_pwm[3]]; # JX2-84
set_property PACKAGE_PIN J6 [get_ports inverter8_pwm[4]]; # JX2-75
set_property PACKAGE_PIN J7 [get_ports inverter8_pwm[5]]; # JX2-73

set_property PACKAGE_PIN G2   [get_ports inverter_rst[0]]; # JX1-19
set_property PACKAGE_PIN E4   [get_ports inverter_rst[1]]; # JX1-24
set_property PACKAGE_PIN W15  [get_ports inverter_rst[2]]; # JX3-99
set_property PACKAGE_PIN H5   [get_ports inverter_rst[3]]; # JX1-10
set_property PACKAGE_PIN T17  [get_ports inverter_rst[4]]; # JX3-94
set_property PACKAGE_PIN W12  [get_ports inverter_rst[5]]; # JX3-91
set_property PACKAGE_PIN Y13  [get_ports inverter_rst[6]]; # JX3-82
set_property PACKAGE_PIN AA11 [get_ports inverter_rst[7]]; # JX3-79

set_property PACKAGE_PIN H6   [get_ports inverter_rdy[0]]; # JX1-9
set_property PACKAGE_PIN H4   [get_ports inverter_rdy[1]]; # JX1-11
set_property PACKAGE_PIN W13  [get_ports inverter_rdy[2]]; # JX3-93
set_property PACKAGE_PIN V16  [get_ports inverter_rdy[3]]; # JX3-98
set_property PACKAGE_PIN AB11 [get_ports inverter_rdy[4]]; # JX3-81
set_property PACKAGE_PIN V13  [get_ports inverter_rdy[5]]; # JX3-86
set_property PACKAGE_PIN V19  [get_ports inverter_rdy[6]]; # BUG: this net is connected to GND :( JX1-93 is unused pin, map it there for now
set_property PACKAGE_PIN AA16 [get_ports inverter_rdy[7]]; # JX3-74

set_property PACKAGE_PIN H3   [get_ports inverter_flt_desat[0]]; # JX1-13
set_property PACKAGE_PIN F2   [get_ports inverter_flt_desat[1]]; # JX1-18
set_property PACKAGE_PIN W16  [get_ports inverter_flt_desat[2]]; # JX3-100
set_property PACKAGE_PIN V15  [get_ports inverter_flt_desat[3]]; # JX3-97
set_property PACKAGE_PIN V14  [get_ports inverter_flt_desat[4]]; # JX3-88
set_property PACKAGE_PIN V11  [get_ports inverter_flt_desat[5]]; # JX3-85
set_property PACKAGE_PIN AA17 [get_ports inverter_flt_desat[6]]; # JX3-76
set_property PACKAGE_PIN Y18  [get_ports inverter_flt_desat[7]]; # JX3-73

set_property PACKAGE_PIN F1   [get_ports inverter_flt_temp[0]]; # JX1-20
set_property PACKAGE_PIN G3   [get_ports inverter_flt_temp[1]]; # JX1-17
set_property PACKAGE_PIN F5   [get_ports inverter_flt_temp[2]]; # JX1-12
set_property PACKAGE_PIN E5   [get_ports inverter_flt_temp[3]]; # JX1-14
set_property PACKAGE_PIN W11  [get_ports inverter_flt_temp[4]]; # JX3-87
set_property PACKAGE_PIN R17  [get_ports inverter_flt_temp[5]]; # JX3-92
set_property PACKAGE_PIN Y19  [get_ports inverter_flt_temp[6]]; # JX3-75
set_property PACKAGE_PIN Y12  [get_ports inverter_flt_temp[7]]; # JX3-80