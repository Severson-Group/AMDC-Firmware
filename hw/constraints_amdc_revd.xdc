# FPGA Pin Mapping for AMDC REV D
#
# Please keep this document in sync with
# the documentation in the AMDC-Hardware
# repo: docs/PinMappingRevD.md


###########
# RGB LED #
###########

set_property IOSTANDARD LVCMOS18 [get_ports {user_led_din[0]}]

set_property PACKAGE_PIN R8 [get_ports {user_led_din[0]}]


###########
# ENCODER #
###########

set_property IOSTANDARD LVCMOS18 [get_ports encoder_1a]
set_property IOSTANDARD LVCMOS18 [get_ports encoder_1b]
set_property IOSTANDARD LVCMOS18 [get_ports encoder_1z]
set_property IOSTANDARD LVCMOS18 [get_ports encoder_2a]
set_property IOSTANDARD LVCMOS18 [get_ports encoder_2b]
set_property IOSTANDARD LVCMOS18 [get_ports encoder_2z]

set_property PACKAGE_PIN A2 [get_ports encoder_1a]
set_property PACKAGE_PIN A1 [get_ports encoder_1b]
set_property PACKAGE_PIN D7 [get_ports encoder_1z]
set_property PACKAGE_PIN D6 [get_ports encoder_2a]
set_property PACKAGE_PIN A5 [get_ports encoder_2b]
set_property PACKAGE_PIN A4 [get_ports encoder_2z]


#########
#  ADC  #
#########

set_property IOSTANDARD LVCMOS18 [get_ports {adc_sdo[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {adc_sdo[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {adc_sdo[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {adc_sdo[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {adc_sdo[4]}]
set_property IOSTANDARD LVCMOS18 [get_ports {adc_sdo[5]}]
set_property IOSTANDARD LVCMOS18 [get_ports {adc_sdo[6]}]
set_property IOSTANDARD LVCMOS18 [get_ports {adc_sdo[7]}]
set_property IOSTANDARD LVCMOS18 [get_ports adc_sck]
set_property IOSTANDARD LVCMOS18 [get_ports adc_cnv]
set_property IOSTANDARD LVCMOS18 [get_ports adc_clkout]

set_property PACKAGE_PIN H6 [get_ports adc_cnv]
set_property PACKAGE_PIN D8 [get_ports adc_sck]
set_property PACKAGE_PIN G4 [get_ports adc_clkout]
set_property PACKAGE_PIN H4 [get_ports {adc_sdo[0]}]
set_property PACKAGE_PIN H3 [get_ports {adc_sdo[1]}]
set_property PACKAGE_PIN G3 [get_ports {adc_sdo[2]}]
set_property PACKAGE_PIN G2 [get_ports {adc_sdo[3]}]
set_property PACKAGE_PIN F4 [get_ports {adc_sdo[4]}]
set_property PACKAGE_PIN G6 [get_ports {adc_sdo[5]}]
set_property PACKAGE_PIN F6 [get_ports {adc_sdo[6]}]
set_property PACKAGE_PIN E8 [get_ports {adc_sdo[7]}]


########
# SPI1 #
########

set_property IOSTANDARD LVCMOS18 [get_ports {spi1_sclk[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports spi1_miso]
set_property IOSTANDARD LVCMOS18 [get_ports {spi1_mosi[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {spi1_nss[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports spi1_in]
set_property IOSTANDARD LVCMOS18 [get_ports {spi1_out[0]}]

set_property PACKAGE_PIN B4 [get_ports {spi1_sclk[0]}]
set_property PACKAGE_PIN C6 [get_ports spi1_miso]
set_property PACKAGE_PIN B3 [get_ports {spi1_mosi[0]}]
set_property PACKAGE_PIN C5 [get_ports {spi1_nss[0]}]
set_property PACKAGE_PIN Y14 [get_ports spi1_in]
set_property PACKAGE_PIN A7 [get_ports {spi1_out[0]}]


########
# SPI2 #
########

set_property IOSTANDARD LVCMOS18 [get_ports {spi2_sclk[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports spi2_miso]
set_property IOSTANDARD LVCMOS18 [get_ports {spi2_mosi[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {spi2_nss[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports spi2_in]
set_property IOSTANDARD LVCMOS18 [get_ports {spi2_out[0]}]

set_property PACKAGE_PIN E2 [get_ports {spi2_sclk[0]}]
set_property PACKAGE_PIN D1 [get_ports spi2_miso]
set_property PACKAGE_PIN D2 [get_ports {spi2_mosi[0]}]
set_property PACKAGE_PIN C1 [get_ports {spi2_nss[0]}]
set_property PACKAGE_PIN Y15 [get_ports spi2_in]
set_property PACKAGE_PIN A6 [get_ports {spi2_out[0]}]


########
# SPI3 #
########

set_property IOSTANDARD LVCMOS18 [get_ports {spi3_sclk[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports spi3_miso]
set_property IOSTANDARD LVCMOS18 [get_ports {spi3_mosi[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {spi3_nss[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports spi3_in]
set_property IOSTANDARD LVCMOS18 [get_ports {spi3_out[0]}]

set_property PACKAGE_PIN G8 [get_ports {spi3_sclk[0]}]
set_property PACKAGE_PIN F7 [get_ports spi3_miso]
set_property PACKAGE_PIN G7 [get_ports {spi3_mosi[0]}]
set_property PACKAGE_PIN E7 [get_ports {spi3_nss[0]}]
set_property PACKAGE_PIN V18 [get_ports spi3_in]
set_property PACKAGE_PIN C8 [get_ports {spi3_out[0]}]


########
# SPI4 #
########

set_property IOSTANDARD LVCMOS18 [get_ports {spi4_sclk[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports spi4_miso]
set_property IOSTANDARD LVCMOS18 [get_ports {spi4_mosi[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {spi4_nss[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports spi4_in]
set_property IOSTANDARD LVCMOS18 [get_ports {spi4_out[0]}]

set_property PACKAGE_PIN AA14 [get_ports {spi4_sclk[0]}]
set_property PACKAGE_PIN B7 [get_ports spi4_miso]
set_property PACKAGE_PIN AA15 [get_ports {spi4_mosi[0]}]
set_property PACKAGE_PIN B6 [get_ports {spi4_nss[0]}]
set_property PACKAGE_PIN W18 [get_ports spi4_in]
set_property PACKAGE_PIN B8 [get_ports {spi4_out[0]}]


#############
# INVERTERS #
#############

set_property IOSTANDARD LVCMOS18 [get_ports {inverter1_pwm[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter1_pwm[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter1_pwm[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter1_pwm[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter1_pwm[4]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter1_pwm[5]}]

set_property IOSTANDARD LVCMOS18 [get_ports {inverter2_pwm[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter2_pwm[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter2_pwm[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter2_pwm[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter2_pwm[4]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter2_pwm[5]}]

set_property IOSTANDARD LVCMOS18 [get_ports {inverter3_pwm[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter3_pwm[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter3_pwm[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter3_pwm[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter3_pwm[4]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter3_pwm[5]}]

set_property IOSTANDARD LVCMOS18 [get_ports {inverter4_pwm[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter4_pwm[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter4_pwm[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter4_pwm[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter4_pwm[4]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter4_pwm[5]}]

set_property IOSTANDARD LVCMOS18 [get_ports {inverter5_pwm[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter5_pwm[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter5_pwm[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter5_pwm[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter5_pwm[4]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter5_pwm[5]}]

set_property IOSTANDARD LVCMOS18 [get_ports {inverter6_pwm[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter6_pwm[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter6_pwm[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter6_pwm[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter6_pwm[4]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter6_pwm[5]}]

set_property IOSTANDARD LVCMOS18 [get_ports {inverter7_pwm[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter7_pwm[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter7_pwm[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter7_pwm[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter7_pwm[4]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter7_pwm[5]}]

set_property IOSTANDARD LVCMOS18 [get_ports {inverter8_pwm[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter8_pwm[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter8_pwm[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter8_pwm[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter8_pwm[4]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter8_pwm[5]}]

set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_a[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_a[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_a[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_a[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_a[4]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_a[5]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_a[6]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_a[7]}]

set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_b[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_b[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_b[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_b[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_b[4]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_b[5]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_b[6]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_b[7]}]

set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_c[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_c[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_c[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_c[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_c[4]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_c[5]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_c[6]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_c[7]}]

set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_d[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_d[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_d[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_d[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_d[4]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_d[5]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_d[6]}]
set_property IOSTANDARD LVCMOS18 [get_ports {inverter_sts_d[7]}]

set_property PACKAGE_PIN J1 [get_ports {inverter1_pwm[0]}]
set_property PACKAGE_PIN J2 [get_ports {inverter1_pwm[1]}]
set_property PACKAGE_PIN M1 [get_ports {inverter1_pwm[2]}]
set_property PACKAGE_PIN M2 [get_ports {inverter1_pwm[3]}]
set_property PACKAGE_PIN H8 [get_ports {inverter1_pwm[4]}]
set_property PACKAGE_PIN M4 [get_ports {inverter1_pwm[5]}]

set_property PACKAGE_PIN R7 [get_ports {inverter2_pwm[0]}]
set_property PACKAGE_PIN P7 [get_ports {inverter2_pwm[1]}]
set_property PACKAGE_PIN L7 [get_ports {inverter2_pwm[2]}]
set_property PACKAGE_PIN K7 [get_ports {inverter2_pwm[3]}]
set_property PACKAGE_PIN M3 [get_ports {inverter2_pwm[4]}]
set_property PACKAGE_PIN N4 [get_ports {inverter2_pwm[5]}]

set_property PACKAGE_PIN U1 [get_ports {inverter3_pwm[0]}]
set_property PACKAGE_PIN U2 [get_ports {inverter3_pwm[1]}]
set_property PACKAGE_PIN L4 [get_ports {inverter3_pwm[2]}]
set_property PACKAGE_PIN L5 [get_ports {inverter3_pwm[3]}]
set_property PACKAGE_PIN P1 [get_ports {inverter3_pwm[4]}]
set_property PACKAGE_PIN N1 [get_ports {inverter3_pwm[5]}]

set_property PACKAGE_PIN K2 [get_ports {inverter4_pwm[0]}]
set_property PACKAGE_PIN L2 [get_ports {inverter4_pwm[1]}]
set_property PACKAGE_PIN L1 [get_ports {inverter4_pwm[2]}]
set_property PACKAGE_PIN P3 [get_ports {inverter4_pwm[3]}]
set_property PACKAGE_PIN P2 [get_ports {inverter4_pwm[4]}]
set_property PACKAGE_PIN J3 [get_ports {inverter4_pwm[5]}]

set_property PACKAGE_PIN N8 [get_ports {inverter5_pwm[0]}]
set_property PACKAGE_PIN P8 [get_ports {inverter5_pwm[1]}]
set_property PACKAGE_PIN N6 [get_ports {inverter5_pwm[2]}]
set_property PACKAGE_PIN N5 [get_ports {inverter5_pwm[3]}]
set_property PACKAGE_PIN K5 [get_ports {inverter5_pwm[4]}]
set_property PACKAGE_PIN J5 [get_ports {inverter5_pwm[5]}]

set_property PACKAGE_PIN M6 [get_ports {inverter6_pwm[0]}]
set_property PACKAGE_PIN R5 [get_ports {inverter6_pwm[1]}]
set_property PACKAGE_PIN R4 [get_ports {inverter6_pwm[2]}]
set_property PACKAGE_PIN P6 [get_ports {inverter6_pwm[3]}]
set_property PACKAGE_PIN P5 [get_ports {inverter6_pwm[4]}]
set_property PACKAGE_PIN L6 [get_ports {inverter6_pwm[5]}]

set_property PACKAGE_PIN J8 [get_ports {inverter7_pwm[0]}]
set_property PACKAGE_PIN K8 [get_ports {inverter7_pwm[1]}]
set_property PACKAGE_PIN M8 [get_ports {inverter7_pwm[2]}]
set_property PACKAGE_PIN M7 [get_ports {inverter7_pwm[3]}]
set_property PACKAGE_PIN AB21 [get_ports {inverter7_pwm[4]}]
set_property PACKAGE_PIN AB22 [get_ports {inverter7_pwm[5]}]

set_property PACKAGE_PIN AB18 [get_ports {inverter8_pwm[0]}]
set_property PACKAGE_PIN T16 [get_ports {inverter8_pwm[1]}]
set_property PACKAGE_PIN J6 [get_ports {inverter8_pwm[2]}]
set_property PACKAGE_PIN AA20 [get_ports {inverter8_pwm[3]}]
set_property PACKAGE_PIN AA19 [get_ports {inverter8_pwm[4]}]
set_property PACKAGE_PIN AB19 [get_ports {inverter8_pwm[5]}]

set_property PACKAGE_PIN B2 [get_ports {inverter_sts_a[0]}]
set_property PACKAGE_PIN E3 [get_ports {inverter_sts_a[1]}]
set_property PACKAGE_PIN E4 [get_ports {inverter_sts_a[2]}]
set_property PACKAGE_PIN F1 [get_ports {inverter_sts_a[3]}]
set_property PACKAGE_PIN F2 [get_ports {inverter_sts_a[4]}]
set_property PACKAGE_PIN E5 [get_ports {inverter_sts_a[5]}]
set_property PACKAGE_PIN F5 [get_ports {inverter_sts_a[6]}]
set_property PACKAGE_PIN H5 [get_ports {inverter_sts_a[7]}]

set_property PACKAGE_PIN W15 [get_ports {inverter_sts_b[0]}]
set_property PACKAGE_PIN C3 [get_ports {inverter_sts_b[1]}]
set_property PACKAGE_PIN D3 [get_ports {inverter_sts_b[2]}]
set_property PACKAGE_PIN C4 [get_ports {inverter_sts_b[3]}]
set_property PACKAGE_PIN D5 [get_ports {inverter_sts_b[4]}]
set_property PACKAGE_PIN G1 [get_ports {inverter_sts_b[5]}]
set_property PACKAGE_PIN H1 [get_ports {inverter_sts_b[6]}]
set_property PACKAGE_PIN B1 [get_ports {inverter_sts_b[7]}]

set_property PACKAGE_PIN Y19 [get_ports {inverter_sts_c[0]}]
set_property PACKAGE_PIN AA11 [get_ports {inverter_sts_c[1]}]
set_property PACKAGE_PIN AB11 [get_ports {inverter_sts_c[2]}]
set_property PACKAGE_PIN V11 [get_ports {inverter_sts_c[3]}]
set_property PACKAGE_PIN W11 [get_ports {inverter_sts_c[4]}]
set_property PACKAGE_PIN W12 [get_ports {inverter_sts_c[5]}]
set_property PACKAGE_PIN W13 [get_ports {inverter_sts_c[6]}]
set_property PACKAGE_PIN V15 [get_ports {inverter_sts_c[7]}]

set_property PACKAGE_PIN Y12 [get_ports {inverter_sts_d[0]}]
set_property PACKAGE_PIN Y13 [get_ports {inverter_sts_d[1]}]
set_property PACKAGE_PIN V13 [get_ports {inverter_sts_d[2]}]
set_property PACKAGE_PIN V14 [get_ports {inverter_sts_d[3]}]
set_property PACKAGE_PIN R17 [get_ports {inverter_sts_d[4]}]
set_property PACKAGE_PIN T17 [get_ports {inverter_sts_d[5]}]
set_property PACKAGE_PIN V16 [get_ports {inverter_sts_d[6]}]
set_property PACKAGE_PIN W16 [get_ports {inverter_sts_d[7]}]

create_debug_core u_ila_0 ila
set_property ALL_PROBE_SAME_MU true [get_debug_cores u_ila_0]
set_property ALL_PROBE_SAME_MU_CNT 1 [get_debug_cores u_ila_0]
set_property C_ADV_TRIGGER false [get_debug_cores u_ila_0]
set_property C_DATA_DEPTH 1024 [get_debug_cores u_ila_0]
set_property C_EN_STRG_QUAL false [get_debug_cores u_ila_0]
set_property C_INPUT_PIPE_STAGES 0 [get_debug_cores u_ila_0]
set_property C_TRIGIN_EN false [get_debug_cores u_ila_0]
set_property C_TRIGOUT_EN false [get_debug_cores u_ila_0]
set_property port_width 1 [get_debug_ports u_ila_0/clk]
connect_debug_port u_ila_0/clk [get_nets [list amdc_revd_i/processing_system7_0/inst/FCLK_CLK0]]
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe0]
set_property port_width 8 [get_debug_ports u_ila_0/probe0]
connect_debug_port u_ila_0/probe0 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_c_ext[0]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_c_ext[1]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_c_ext[2]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_c_ext[3]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_c_ext[4]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_c_ext[5]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_c_ext[6]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_c_ext[7]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe1]
set_property port_width 8 [get_debug_ports u_ila_0/probe1]
connect_debug_port u_ila_0/probe1 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_d_ext[0]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_d_ext[1]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_d_ext[2]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_d_ext[3]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_d_ext[4]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_d_ext[5]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_d_ext[6]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_d_ext[7]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe2]
set_property port_width 8 [get_debug_ports u_ila_0/probe2]
connect_debug_port u_ila_0/probe2 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_b_ext[0]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_b_ext[1]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_b_ext[2]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_b_ext[3]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_b_ext[4]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_b_ext[5]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_b_ext[6]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_b_ext[7]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe3]
set_property port_width 8 [get_debug_ports u_ila_0/probe3]
connect_debug_port u_ila_0/probe3 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_a_ext[0]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_a_ext[1]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_a_ext[2]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_a_ext[3]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_a_ext[4]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_a_ext[5]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_a_ext[6]} {amdc_revd_i/amdc_inv_status_mux_0/inst/sts_a_ext[7]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe4]
set_property port_width 3 [get_debug_ports u_ila_0/probe4]
connect_debug_port u_ila_0/probe4 [get_nets [list {amdc_revd_i/amdc_dac_0/inst/dac_sts[0]} {amdc_revd_i/amdc_dac_0/inst/dac_sts[1]} {amdc_revd_i/amdc_dac_0/inst/dac_sts[2]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe5]
set_property port_width 1 [get_debug_ports u_ila_0/probe5]
connect_debug_port u_ila_0/probe5 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[0]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe6]
set_property port_width 1 [get_debug_ports u_ila_0/probe6]
connect_debug_port u_ila_0/probe6 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[1]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe7]
set_property port_width 1 [get_debug_ports u_ila_0/probe7]
connect_debug_port u_ila_0/probe7 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[2]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe8]
set_property port_width 1 [get_debug_ports u_ila_0/probe8]
connect_debug_port u_ila_0/probe8 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[3]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe9]
set_property port_width 1 [get_debug_ports u_ila_0/probe9]
connect_debug_port u_ila_0/probe9 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[4]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe10]
set_property port_width 1 [get_debug_ports u_ila_0/probe10]
connect_debug_port u_ila_0/probe10 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[5]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe11]
set_property port_width 1 [get_debug_ports u_ila_0/probe11]
connect_debug_port u_ila_0/probe11 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[6]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe12]
set_property port_width 1 [get_debug_ports u_ila_0/probe12]
connect_debug_port u_ila_0/probe12 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[7]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe13]
set_property port_width 1 [get_debug_ports u_ila_0/probe13]
connect_debug_port u_ila_0/probe13 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[8]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe14]
set_property port_width 1 [get_debug_ports u_ila_0/probe14]
connect_debug_port u_ila_0/probe14 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[9]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe15]
set_property port_width 1 [get_debug_ports u_ila_0/probe15]
connect_debug_port u_ila_0/probe15 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[10]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe16]
set_property port_width 1 [get_debug_ports u_ila_0/probe16]
connect_debug_port u_ila_0/probe16 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[11]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe17]
set_property port_width 1 [get_debug_ports u_ila_0/probe17]
connect_debug_port u_ila_0/probe17 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[12]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe18]
set_property port_width 1 [get_debug_ports u_ila_0/probe18]
connect_debug_port u_ila_0/probe18 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[13]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe19]
set_property port_width 1 [get_debug_ports u_ila_0/probe19]
connect_debug_port u_ila_0/probe19 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[14]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe20]
set_property port_width 1 [get_debug_ports u_ila_0/probe20]
connect_debug_port u_ila_0/probe20 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[15]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe21]
set_property port_width 1 [get_debug_ports u_ila_0/probe21]
connect_debug_port u_ila_0/probe21 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[16]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe22]
set_property port_width 1 [get_debug_ports u_ila_0/probe22]
connect_debug_port u_ila_0/probe22 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[17]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe23]
set_property port_width 1 [get_debug_ports u_ila_0/probe23]
connect_debug_port u_ila_0/probe23 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[18]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe24]
set_property port_width 1 [get_debug_ports u_ila_0/probe24]
connect_debug_port u_ila_0/probe24 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[19]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe25]
set_property port_width 1 [get_debug_ports u_ila_0/probe25]
connect_debug_port u_ila_0/probe25 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[20]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe26]
set_property port_width 1 [get_debug_ports u_ila_0/probe26]
connect_debug_port u_ila_0/probe26 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[21]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe27]
set_property port_width 1 [get_debug_ports u_ila_0/probe27]
connect_debug_port u_ila_0/probe27 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[22]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe28]
set_property port_width 1 [get_debug_ports u_ila_0/probe28]
connect_debug_port u_ila_0/probe28 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[23]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe29]
set_property port_width 1 [get_debug_ports u_ila_0/probe29]
connect_debug_port u_ila_0/probe29 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[24]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe30]
set_property port_width 1 [get_debug_ports u_ila_0/probe30]
connect_debug_port u_ila_0/probe30 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[25]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe31]
set_property port_width 1 [get_debug_ports u_ila_0/probe31]
connect_debug_port u_ila_0/probe31 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[26]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe32]
set_property port_width 1 [get_debug_ports u_ila_0/probe32]
connect_debug_port u_ila_0/probe32 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[27]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe33]
set_property port_width 1 [get_debug_ports u_ila_0/probe33]
connect_debug_port u_ila_0/probe33 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[28]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe34]
set_property port_width 1 [get_debug_ports u_ila_0/probe34]
connect_debug_port u_ila_0/probe34 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[29]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe35]
set_property port_width 1 [get_debug_ports u_ila_0/probe35]
connect_debug_port u_ila_0/probe35 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[30]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe36]
set_property port_width 1 [get_debug_ports u_ila_0/probe36]
connect_debug_port u_ila_0/probe36 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg0_reg_n_0_[31]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe37]
set_property port_width 1 [get_debug_ports u_ila_0/probe37]
connect_debug_port u_ila_0/probe37 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[0]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe38]
set_property port_width 1 [get_debug_ports u_ila_0/probe38]
connect_debug_port u_ila_0/probe38 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[1]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe39]
set_property port_width 1 [get_debug_ports u_ila_0/probe39]
connect_debug_port u_ila_0/probe39 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[2]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe40]
set_property port_width 1 [get_debug_ports u_ila_0/probe40]
connect_debug_port u_ila_0/probe40 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[3]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe41]
set_property port_width 1 [get_debug_ports u_ila_0/probe41]
connect_debug_port u_ila_0/probe41 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[4]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe42]
set_property port_width 1 [get_debug_ports u_ila_0/probe42]
connect_debug_port u_ila_0/probe42 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[5]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe43]
set_property port_width 1 [get_debug_ports u_ila_0/probe43]
connect_debug_port u_ila_0/probe43 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[6]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe44]
set_property port_width 1 [get_debug_ports u_ila_0/probe44]
connect_debug_port u_ila_0/probe44 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[7]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe45]
set_property port_width 1 [get_debug_ports u_ila_0/probe45]
connect_debug_port u_ila_0/probe45 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[8]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe46]
set_property port_width 1 [get_debug_ports u_ila_0/probe46]
connect_debug_port u_ila_0/probe46 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[9]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe47]
set_property port_width 1 [get_debug_ports u_ila_0/probe47]
connect_debug_port u_ila_0/probe47 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[10]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe48]
set_property port_width 1 [get_debug_ports u_ila_0/probe48]
connect_debug_port u_ila_0/probe48 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[11]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe49]
set_property port_width 1 [get_debug_ports u_ila_0/probe49]
connect_debug_port u_ila_0/probe49 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[12]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe50]
set_property port_width 1 [get_debug_ports u_ila_0/probe50]
connect_debug_port u_ila_0/probe50 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[13]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe51]
set_property port_width 1 [get_debug_ports u_ila_0/probe51]
connect_debug_port u_ila_0/probe51 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[14]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe52]
set_property port_width 1 [get_debug_ports u_ila_0/probe52]
connect_debug_port u_ila_0/probe52 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[15]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe53]
set_property port_width 1 [get_debug_ports u_ila_0/probe53]
connect_debug_port u_ila_0/probe53 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[16]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe54]
set_property port_width 1 [get_debug_ports u_ila_0/probe54]
connect_debug_port u_ila_0/probe54 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[17]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe55]
set_property port_width 1 [get_debug_ports u_ila_0/probe55]
connect_debug_port u_ila_0/probe55 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[18]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe56]
set_property port_width 1 [get_debug_ports u_ila_0/probe56]
connect_debug_port u_ila_0/probe56 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[19]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe57]
set_property port_width 1 [get_debug_ports u_ila_0/probe57]
connect_debug_port u_ila_0/probe57 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[20]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe58]
set_property port_width 1 [get_debug_ports u_ila_0/probe58]
connect_debug_port u_ila_0/probe58 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[21]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe59]
set_property port_width 1 [get_debug_ports u_ila_0/probe59]
connect_debug_port u_ila_0/probe59 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[22]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe60]
set_property port_width 1 [get_debug_ports u_ila_0/probe60]
connect_debug_port u_ila_0/probe60 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[23]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe61]
set_property port_width 1 [get_debug_ports u_ila_0/probe61]
connect_debug_port u_ila_0/probe61 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[24]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe62]
set_property port_width 1 [get_debug_ports u_ila_0/probe62]
connect_debug_port u_ila_0/probe62 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[25]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe63]
set_property port_width 1 [get_debug_ports u_ila_0/probe63]
connect_debug_port u_ila_0/probe63 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[26]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe64]
set_property port_width 1 [get_debug_ports u_ila_0/probe64]
connect_debug_port u_ila_0/probe64 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[27]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe65]
set_property port_width 1 [get_debug_ports u_ila_0/probe65]
connect_debug_port u_ila_0/probe65 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[28]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe66]
set_property port_width 1 [get_debug_ports u_ila_0/probe66]
connect_debug_port u_ila_0/probe66 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[29]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe67]
set_property port_width 1 [get_debug_ports u_ila_0/probe67]
connect_debug_port u_ila_0/probe67 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[30]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe68]
set_property port_width 1 [get_debug_ports u_ila_0/probe68]
connect_debug_port u_ila_0/probe68 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg8_reg_n_0_[31]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe69]
set_property port_width 1 [get_debug_ports u_ila_0/probe69]
connect_debug_port u_ila_0/probe69 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[0]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe70]
set_property port_width 1 [get_debug_ports u_ila_0/probe70]
connect_debug_port u_ila_0/probe70 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[1]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe71]
set_property port_width 1 [get_debug_ports u_ila_0/probe71]
connect_debug_port u_ila_0/probe71 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[2]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe72]
set_property port_width 1 [get_debug_ports u_ila_0/probe72]
connect_debug_port u_ila_0/probe72 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[3]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe73]
set_property port_width 1 [get_debug_ports u_ila_0/probe73]
connect_debug_port u_ila_0/probe73 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[4]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe74]
set_property port_width 1 [get_debug_ports u_ila_0/probe74]
connect_debug_port u_ila_0/probe74 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[5]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe75]
set_property port_width 1 [get_debug_ports u_ila_0/probe75]
connect_debug_port u_ila_0/probe75 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[6]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe76]
set_property port_width 1 [get_debug_ports u_ila_0/probe76]
connect_debug_port u_ila_0/probe76 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[7]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe77]
set_property port_width 1 [get_debug_ports u_ila_0/probe77]
connect_debug_port u_ila_0/probe77 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[8]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe78]
set_property port_width 1 [get_debug_ports u_ila_0/probe78]
connect_debug_port u_ila_0/probe78 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[9]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe79]
set_property port_width 1 [get_debug_ports u_ila_0/probe79]
connect_debug_port u_ila_0/probe79 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[10]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe80]
set_property port_width 1 [get_debug_ports u_ila_0/probe80]
connect_debug_port u_ila_0/probe80 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[11]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe81]
set_property port_width 1 [get_debug_ports u_ila_0/probe81]
connect_debug_port u_ila_0/probe81 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[12]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe82]
set_property port_width 1 [get_debug_ports u_ila_0/probe82]
connect_debug_port u_ila_0/probe82 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[13]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe83]
set_property port_width 1 [get_debug_ports u_ila_0/probe83]
connect_debug_port u_ila_0/probe83 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[14]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe84]
set_property port_width 1 [get_debug_ports u_ila_0/probe84]
connect_debug_port u_ila_0/probe84 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[15]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe85]
set_property port_width 1 [get_debug_ports u_ila_0/probe85]
connect_debug_port u_ila_0/probe85 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[16]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe86]
set_property port_width 1 [get_debug_ports u_ila_0/probe86]
connect_debug_port u_ila_0/probe86 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[17]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe87]
set_property port_width 1 [get_debug_ports u_ila_0/probe87]
connect_debug_port u_ila_0/probe87 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[18]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe88]
set_property port_width 1 [get_debug_ports u_ila_0/probe88]
connect_debug_port u_ila_0/probe88 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[19]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe89]
set_property port_width 1 [get_debug_ports u_ila_0/probe89]
connect_debug_port u_ila_0/probe89 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[20]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe90]
set_property port_width 1 [get_debug_ports u_ila_0/probe90]
connect_debug_port u_ila_0/probe90 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[21]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe91]
set_property port_width 1 [get_debug_ports u_ila_0/probe91]
connect_debug_port u_ila_0/probe91 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[22]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe92]
set_property port_width 1 [get_debug_ports u_ila_0/probe92]
connect_debug_port u_ila_0/probe92 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[23]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe93]
set_property port_width 1 [get_debug_ports u_ila_0/probe93]
connect_debug_port u_ila_0/probe93 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[24]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe94]
set_property port_width 1 [get_debug_ports u_ila_0/probe94]
connect_debug_port u_ila_0/probe94 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[25]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe95]
set_property port_width 1 [get_debug_ports u_ila_0/probe95]
connect_debug_port u_ila_0/probe95 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[26]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe96]
set_property port_width 1 [get_debug_ports u_ila_0/probe96]
connect_debug_port u_ila_0/probe96 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[27]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe97]
set_property port_width 1 [get_debug_ports u_ila_0/probe97]
connect_debug_port u_ila_0/probe97 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[28]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe98]
set_property port_width 1 [get_debug_ports u_ila_0/probe98]
connect_debug_port u_ila_0/probe98 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[29]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe99]
set_property port_width 1 [get_debug_ports u_ila_0/probe99]
connect_debug_port u_ila_0/probe99 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[30]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe100]
set_property port_width 1 [get_debug_ports u_ila_0/probe100]
connect_debug_port u_ila_0/probe100 [get_nets [list {amdc_revd_i/amdc_inv_status_mux_0/inst/amdc_inv_status_mux_v1_0_S00_AXI_inst/slv_reg16_reg_n_0_[31]}]]
set_property C_CLK_INPUT_FREQ_HZ 300000000 [get_debug_cores dbg_hub]
set_property C_ENABLE_CLK_DIVIDER false [get_debug_cores dbg_hub]
set_property C_USER_SCAN_CHAIN 1 [get_debug_cores dbg_hub]
connect_debug_port dbg_hub/clk [get_nets u_ila_0_FCLK_CLK0]
