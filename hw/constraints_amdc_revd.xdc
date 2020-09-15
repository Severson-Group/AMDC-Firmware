# FPGA Pin Mapping for AMDC REV D
# 
# Please keep this document in sync with 
# the documentation in the AMDC-Hardware
# repo: docs/PinMappingRevD.md


###########
# RGB LED #
###########

set_property IOSTANDARD LVCMOS18 [get_ports user_led_din];

set_property PACKAGE_PIN R8 [get_ports user_led_din]; # JX2-14


###########
# ENCODER #
###########

set_property IOSTANDARD LVCMOS18 [get_ports encoder_1a];
set_property IOSTANDARD LVCMOS18 [get_ports encoder_1b];
set_property IOSTANDARD LVCMOS18 [get_ports encoder_1z];
set_property IOSTANDARD LVCMOS18 [get_ports encoder_2a];
set_property IOSTANDARD LVCMOS18 [get_ports encoder_2b];
set_property IOSTANDARD LVCMOS18 [get_ports encoder_2z];

set_property PACKAGE_PIN A2  [get_ports encoder_1a]; # JX1-54
set_property PACKAGE_PIN A1  [get_ports encoder_1b]; # JX1-56
set_property PACKAGE_PIN D7  [get_ports encoder_1z]; # JX1-62
set_property PACKAGE_PIN D6  [get_ports encoder_2a]; # JX1-64
set_property PACKAGE_PIN A5  [get_ports encoder_2b]; # JX1-68
set_property PACKAGE_PIN A4  [get_ports encoder_2z]; # JX1-70


#########
#  ADC  #
#########

set_property IOSTANDARD LVCMOS18 [get_ports adc_sdo[0]];
set_property IOSTANDARD LVCMOS18 [get_ports adc_sdo[1]];
set_property IOSTANDARD LVCMOS18 [get_ports adc_sdo[2]];
set_property IOSTANDARD LVCMOS18 [get_ports adc_sdo[3]];
set_property IOSTANDARD LVCMOS18 [get_ports adc_sdo[4]];
set_property IOSTANDARD LVCMOS18 [get_ports adc_sdo[5]];
set_property IOSTANDARD LVCMOS18 [get_ports adc_sdo[6]];
set_property IOSTANDARD LVCMOS18 [get_ports adc_sdo[7]];
set_property IOSTANDARD LVCMOS18 [get_ports adc_sck];
set_property IOSTANDARD LVCMOS18 [get_ports adc_cnv];
set_property IOSTANDARD LVCMOS18 [get_ports adc_clkout];

set_property PACKAGE_PIN H6 [get_ports adc_cnv];    # JX1-9 
set_property PACKAGE_PIN D8 [get_ports adc_sck];    # JX1-37 
set_property PACKAGE_PIN G4 [get_ports adc_clkout]; # JX1-23
set_property PACKAGE_PIN H4 [get_ports adc_sdo[0]]; # JX1-11                                                              
set_property PACKAGE_PIN H3 [get_ports adc_sdo[1]]; # JX1-13
set_property PACKAGE_PIN G3 [get_ports adc_sdo[2]]; # JX1-17
set_property PACKAGE_PIN G2 [get_ports adc_sdo[3]]; # JX1-19
set_property PACKAGE_PIN F4 [get_ports adc_sdo[4]]; # JX1-25
set_property PACKAGE_PIN G6 [get_ports adc_sdo[5]]; # JX1-29
set_property PACKAGE_PIN F6 [get_ports adc_sdo[6]]; # JX1-31
set_property PACKAGE_PIN E8 [get_ports adc_sdo[7]]; # JX1-35


########
# SPI1 #
########

set_property IOSTANDARD LVCMOS18 [get_ports spi1_sclk];
set_property IOSTANDARD LVCMOS18 [get_ports spi1_miso];
set_property IOSTANDARD LVCMOS18 [get_ports spi1_mosi];
set_property IOSTANDARD LVCMOS18 [get_ports spi1_nss];
set_property IOSTANDARD LVCMOS18 [get_ports spi1_in];
set_property IOSTANDARD LVCMOS18 [get_ports spi1_out];

set_property PACKAGE_PIN B4  [get_ports spi1_sclk]; # JX1-47
set_property PACKAGE_PIN C6  [get_ports spi1_miso]; # JX1-41
set_property PACKAGE_PIN B3  [get_ports spi1_mosi]; # JX1-49
set_property PACKAGE_PIN C5  [get_ports spi1_nss];  # JX1-43
set_property PACKAGE_PIN Y14 [get_ports spi1_in];   # JX1-88
set_property PACKAGE_PIN A7  [get_ports spi1_out];  # JX1-74


########
# SPI2 #
########

set_property IOSTANDARD LVCMOS18 [get_ports spi2_sclk];
set_property IOSTANDARD LVCMOS18 [get_ports spi2_miso];
set_property IOSTANDARD LVCMOS18 [get_ports spi2_mosi];
set_property IOSTANDARD LVCMOS18 [get_ports spi2_nss];
set_property IOSTANDARD LVCMOS18 [get_ports spi2_in];
set_property IOSTANDARD LVCMOS18 [get_ports spi2_out];

set_property PACKAGE_PIN E2  [get_ports spi2_sclk]; # JX1-61
set_property PACKAGE_PIN D1  [get_ports spi2_miso]; # JX1-53
set_property PACKAGE_PIN D2  [get_ports spi2_mosi]; # JX1-63
set_property PACKAGE_PIN C1  [get_ports spi2_nss];  # JX1-55
set_property PACKAGE_PIN Y15 [get_ports spi2_in];   # JX1-90
set_property PACKAGE_PIN A6  [get_ports spi2_out];  # JX1-76


########
# SPI3 #
########

set_property IOSTANDARD LVCMOS18 [get_ports spi3_sclk];
set_property IOSTANDARD LVCMOS18 [get_ports spi3_miso];
set_property IOSTANDARD LVCMOS18 [get_ports spi3_mosi];
set_property IOSTANDARD LVCMOS18 [get_ports spi3_nss];
set_property IOSTANDARD LVCMOS18 [get_ports spi3_in];
set_property IOSTANDARD LVCMOS18 [get_ports spi3_out];

set_property PACKAGE_PIN G8  [get_ports spi3_sclk]; # JX1-73
set_property PACKAGE_PIN F7  [get_ports spi3_miso]; # JX1-67
set_property PACKAGE_PIN G7  [get_ports spi3_mosi]; # JX1-75
set_property PACKAGE_PIN E7  [get_ports spi3_nss];  # JX1-69
set_property PACKAGE_PIN V18 [get_ports spi3_in];   # JX1-92
set_property PACKAGE_PIN C8  [get_ports spi3_out];  # JX1-82


########
# SPI4 #
########

set_property IOSTANDARD LVCMOS18 [get_ports spi4_sclk];
set_property IOSTANDARD LVCMOS18 [get_ports spi4_miso];
set_property IOSTANDARD LVCMOS18 [get_ports spi4_mosi];
set_property IOSTANDARD LVCMOS18 [get_ports spi4_nss];
set_property IOSTANDARD LVCMOS18 [get_ports spi4_in];
set_property IOSTANDARD LVCMOS18 [get_ports spi4_out];

set_property PACKAGE_PIN AA14 [get_ports spi4_sclk]; # JX1-87
set_property PACKAGE_PIN B7   [get_ports spi4_miso]; # JX1-81
set_property PACKAGE_PIN AA15 [get_ports spi4_mosi]; # JX1-89
set_property PACKAGE_PIN B6   [get_ports spi4_nss];  # JX1-83
set_property PACKAGE_PIN W18  [get_ports spi4_in];   # JX1-94
set_property PACKAGE_PIN B8   [get_ports spi4_out];  # JX1-84


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

set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_a[0]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_a[1]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_a[2]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_a[3]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_a[4]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_a[5]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_a[6]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_a[7]];

set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_b[0]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_b[1]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_b[2]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_b[3]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_b[4]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_b[5]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_b[6]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_b[7]];

set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_c[0]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_c[1]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_c[2]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_c[3]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_c[4]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_c[5]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_c[6]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_c[7]];

set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_d[0]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_d[1]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_d[2]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_d[3]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_d[4]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_d[5]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_d[6]];
set_property IOSTANDARD LVCMOS18 [get_ports inverter_sts_d[7]];

set_property PACKAGE_PIN J1 [get_ports inverter1_pwm[0]]; # JX2-20
set_property PACKAGE_PIN J2 [get_ports inverter1_pwm[1]]; # JX2-18
set_property PACKAGE_PIN M1 [get_ports inverter1_pwm[2]]; # JX2-43
set_property PACKAGE_PIN M2 [get_ports inverter1_pwm[3]]; # JX2-41
set_property PACKAGE_PIN H8 [get_ports inverter1_pwm[4]]; # JX2-13
set_property PACKAGE_PIN M4 [get_ports inverter1_pwm[5]]; # JX2-17

set_property PACKAGE_PIN R7 [get_ports inverter2_pwm[0]]; # JX2-31
set_property PACKAGE_PIN P7 [get_ports inverter2_pwm[1]]; # JX2-29
set_property PACKAGE_PIN L7 [get_ports inverter2_pwm[2]]; # JX2-25
set_property PACKAGE_PIN K7 [get_ports inverter2_pwm[3]]; # JX2-23
set_property PACKAGE_PIN M3 [get_ports inverter2_pwm[4]]; # JX2-19
set_property PACKAGE_PIN N4 [get_ports inverter2_pwm[5]]; # JX2-35

set_property PACKAGE_PIN U1 [get_ports inverter3_pwm[0]]; # JX2-56
set_property PACKAGE_PIN U2 [get_ports inverter3_pwm[1]]; # JX2-54
set_property PACKAGE_PIN L4 [get_ports inverter3_pwm[2]]; # JX2-50
set_property PACKAGE_PIN L5 [get_ports inverter3_pwm[3]]; # JX2-48
set_property PACKAGE_PIN P1 [get_ports inverter3_pwm[4]]; # JX2-44
set_property PACKAGE_PIN N1 [get_ports inverter3_pwm[5]]; # JX2-42

set_property PACKAGE_PIN K2 [get_ports inverter4_pwm[0]]; # JX2-26
set_property PACKAGE_PIN L2 [get_ports inverter4_pwm[1]]; # JX2-30
set_property PACKAGE_PIN L1 [get_ports inverter4_pwm[2]]; # JX2-32
set_property PACKAGE_PIN P3 [get_ports inverter4_pwm[3]]; # JX2-36
set_property PACKAGE_PIN P2 [get_ports inverter4_pwm[4]]; # JX2-38
set_property PACKAGE_PIN J3 [get_ports inverter4_pwm[5]]; # JX2-24

set_property PACKAGE_PIN N8 [get_ports inverter5_pwm[0]]; # JX2-88
set_property PACKAGE_PIN P8 [get_ports inverter5_pwm[1]]; # JX2-90
set_property PACKAGE_PIN N6 [get_ports inverter5_pwm[2]]; # JX2-82
set_property PACKAGE_PIN N5 [get_ports inverter5_pwm[3]]; # JX2-84
set_property PACKAGE_PIN K5 [get_ports inverter5_pwm[4]]; # JX2-69
set_property PACKAGE_PIN J5 [get_ports inverter5_pwm[5]]; # JX2-67

set_property PACKAGE_PIN M6 [get_ports inverter6_pwm[0]]; # JX2-64
set_property PACKAGE_PIN R5 [get_ports inverter6_pwm[1]]; # JX2-68
set_property PACKAGE_PIN R4 [get_ports inverter6_pwm[2]]; # JX2-70
set_property PACKAGE_PIN P6 [get_ports inverter6_pwm[3]]; # JX2-74
set_property PACKAGE_PIN P5 [get_ports inverter6_pwm[4]]; # JX2-76
set_property PACKAGE_PIN L6 [get_ports inverter6_pwm[5]]; # JX2-62

set_property PACKAGE_PIN J8   [get_ports inverter7_pwm[0]]; # JX2-81
set_property PACKAGE_PIN K8   [get_ports inverter7_pwm[1]]; # JX2-83
set_property PACKAGE_PIN M8   [get_ports inverter7_pwm[2]]; # JX2-87
set_property PACKAGE_PIN M7   [get_ports inverter7_pwm[3]]; # JX2-89
set_property PACKAGE_PIN AB21 [get_ports inverter7_pwm[4]]; # JX2-93
set_property PACKAGE_PIN AB22 [get_ports inverter7_pwm[5]]; # JX2-95

set_property PACKAGE_PIN AB18 [get_ports inverter8_pwm[0]]; # JX2-94
set_property PACKAGE_PIN T16  [get_ports inverter8_pwm[1]]; # JX2-100
set_property PACKAGE_PIN J6   [get_ports inverter8_pwm[2]]; # JX2-75
set_property PACKAGE_PIN AA20 [get_ports inverter8_pwm[3]]; # JX2-99
set_property PACKAGE_PIN AA19 [get_ports inverter8_pwm[4]]; # JX2-97
set_property PACKAGE_PIN AB19 [get_ports inverter8_pwm[5]]; # JX2-96

set_property PACKAGE_PIN B2 [get_ports inverter_sts_a[0]]; # JX1-30
set_property PACKAGE_PIN E3 [get_ports inverter_sts_a[1]]; # JX1-26
set_property PACKAGE_PIN E4 [get_ports inverter_sts_a[2]]; # JX1-24
set_property PACKAGE_PIN F1 [get_ports inverter_sts_a[3]]; # JX1-20
set_property PACKAGE_PIN F2 [get_ports inverter_sts_a[4]]; # JX1-18
set_property PACKAGE_PIN E5 [get_ports inverter_sts_a[5]]; # JX1-14
set_property PACKAGE_PIN F5 [get_ports inverter_sts_a[6]]; # JX1-12
set_property PACKAGE_PIN H5 [get_ports inverter_sts_a[7]]; # JX1-10

set_property PACKAGE_PIN W15 [get_ports inverter_sts_b[0]]; # JX3-99
set_property PACKAGE_PIN C3  [get_ports inverter_sts_b[1]]; # JX1-50
set_property PACKAGE_PIN D3  [get_ports inverter_sts_b[2]]; # JX1-48
set_property PACKAGE_PIN C4  [get_ports inverter_sts_b[3]]; # JX1-44
set_property PACKAGE_PIN D5  [get_ports inverter_sts_b[4]]; # JX1-42
set_property PACKAGE_PIN G1  [get_ports inverter_sts_b[5]]; # JX1-38
set_property PACKAGE_PIN H1  [get_ports inverter_sts_b[6]]; # JX1-36
set_property PACKAGE_PIN B1  [get_ports inverter_sts_b[7]]; # JX1-32

set_property PACKAGE_PIN Y19  [get_ports inverter_sts_c[0]]; # JX3-75
set_property PACKAGE_PIN AA11 [get_ports inverter_sts_c[1]]; # JX3-79
set_property PACKAGE_PIN AB11 [get_ports inverter_sts_c[2]]; # JX3-81
set_property PACKAGE_PIN V11  [get_ports inverter_sts_c[3]]; # JX3-85
set_property PACKAGE_PIN W11  [get_ports inverter_sts_c[4]]; # JX3-87
set_property PACKAGE_PIN W12  [get_ports inverter_sts_c[5]]; # JX3-91
set_property PACKAGE_PIN W13  [get_ports inverter_sts_c[6]]; # JX3-93
set_property PACKAGE_PIN V15  [get_ports inverter_sts_c[7]]; # JX3-97

set_property PACKAGE_PIN Y12 [get_ports inverter_sts_d[0]]; # JX3-80
set_property PACKAGE_PIN Y13 [get_ports inverter_sts_d[1]]; # JX3-82
set_property PACKAGE_PIN V13 [get_ports inverter_sts_d[2]]; # JX3-86
set_property PACKAGE_PIN V14 [get_ports inverter_sts_d[3]]; # JX3-88
set_property PACKAGE_PIN R17 [get_ports inverter_sts_d[4]]; # JX3-92
set_property PACKAGE_PIN T17 [get_ports inverter_sts_d[5]]; # JX3-94
set_property PACKAGE_PIN V16 [get_ports inverter_sts_d[6]]; # JX3-98
set_property PACKAGE_PIN W16 [get_ports inverter_sts_d[7]]; # JX3-100
