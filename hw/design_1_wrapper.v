//Copyright 1986-2017 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2017.2 (win64) Build 1909853 Thu Jun 15 18:39:09 MDT 2017
//Date        : Sun Feb 24 17:21:51 2019
//Host        : severson-03 running 64-bit major release  (build 9200)
//Command     : generate_target design_1_wrapper.bd
//Design      : design_1_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module design_1_wrapper
   (DDR_addr,
    DDR_ba,
    DDR_cas_n,
    DDR_ck_n,
    DDR_ck_p,
    DDR_cke,
    DDR_cs_n,
    DDR_dm,
    DDR_dq,
    DDR_dqs_n,
    DDR_dqs_p,
    DDR_odt,
    DDR_ras_n,
    DDR_reset_n,
    DDR_we_n,
    FIXED_IO_ddr_vrn,
    FIXED_IO_ddr_vrp,
    FIXED_IO_mio,
    FIXED_IO_ps_clk,
    FIXED_IO_ps_porb,
    FIXED_IO_ps_srstb,
    adc1_clkout,
    adc1_cnv,
    adc1_sck,
    adc1_sdo,
    adc2_clkout,
    adc2_cnv,
    adc2_sck,
    adc2_sdo,
    encoder_a,
    encoder_alarm_a,
    encoder_alarm_b,
    encoder_alarm_d,
    encoder_alarm_z,
    encoder_b,
    encoder_z,
    gpioa,
    gpiob,
    inverter1_pwm,
    inverter2_pwm,
    inverter3_pwm,
    inverter4_pwm,
    inverter5_pwm,
    inverter6_pwm,
    inverter7_pwm,
    inverter8_pwm,
    inverter_flt_desat,
    inverter_flt_temp,
    inverter_rdy,
    inverter_rst);
  inout [14:0]DDR_addr;
  inout [2:0]DDR_ba;
  inout DDR_cas_n;
  inout DDR_ck_n;
  inout DDR_ck_p;
  inout DDR_cke;
  inout DDR_cs_n;
  inout [3:0]DDR_dm;
  inout [31:0]DDR_dq;
  inout [3:0]DDR_dqs_n;
  inout [3:0]DDR_dqs_p;
  inout DDR_odt;
  inout DDR_ras_n;
  inout DDR_reset_n;
  inout DDR_we_n;
  inout FIXED_IO_ddr_vrn;
  inout FIXED_IO_ddr_vrp;
  inout [53:0]FIXED_IO_mio;
  inout FIXED_IO_ps_clk;
  inout FIXED_IO_ps_porb;
  inout FIXED_IO_ps_srstb;
  input adc1_clkout;
  output adc1_cnv;
  output adc1_sck;
  input [7:0]adc1_sdo;
  input adc2_clkout;
  output adc2_cnv;
  output adc2_sck;
  input [7:0]adc2_sdo;
  input encoder_a;
  input encoder_alarm_a;
  input encoder_alarm_b;
  input encoder_alarm_d;
  input encoder_alarm_z;
  input encoder_b;
  input encoder_z;
  output [11:0]gpioa;
  output [11:0]gpiob;
  output [5:0]inverter1_pwm;
  output [5:0]inverter2_pwm;
  output [5:0]inverter3_pwm;
  output [5:0]inverter4_pwm;
  output [5:0]inverter5_pwm;
  output [5:0]inverter6_pwm;
  output [5:0]inverter7_pwm;
  output [5:0]inverter8_pwm;
  input [7:0]inverter_flt_desat;
  input [7:0]inverter_flt_temp;
  input [7:0]inverter_rdy;
  output [7:0]inverter_rst;

  wire [14:0]DDR_addr;
  wire [2:0]DDR_ba;
  wire DDR_cas_n;
  wire DDR_ck_n;
  wire DDR_ck_p;
  wire DDR_cke;
  wire DDR_cs_n;
  wire [3:0]DDR_dm;
  wire [31:0]DDR_dq;
  wire [3:0]DDR_dqs_n;
  wire [3:0]DDR_dqs_p;
  wire DDR_odt;
  wire DDR_ras_n;
  wire DDR_reset_n;
  wire DDR_we_n;
  wire FIXED_IO_ddr_vrn;
  wire FIXED_IO_ddr_vrp;
  wire [53:0]FIXED_IO_mio;
  wire FIXED_IO_ps_clk;
  wire FIXED_IO_ps_porb;
  wire FIXED_IO_ps_srstb;
  wire adc1_clkout;
  wire adc1_cnv;
  wire adc1_sck;
  wire [7:0]adc1_sdo;
  wire adc2_clkout;
  wire adc2_cnv;
  wire adc2_sck;
  wire [7:0]adc2_sdo;
  wire encoder_a;
  wire encoder_alarm_a;
  wire encoder_alarm_b;
  wire encoder_alarm_d;
  wire encoder_alarm_z;
  wire encoder_b;
  wire encoder_z;
  wire [11:0]gpioa;
  wire [11:0]gpiob;
  wire [5:0]inverter1_pwm;
  wire [5:0]inverter2_pwm;
  wire [5:0]inverter3_pwm;
  wire [5:0]inverter4_pwm;
  wire [5:0]inverter5_pwm;
  wire [5:0]inverter6_pwm;
  wire [5:0]inverter7_pwm;
  wire [5:0]inverter8_pwm;
  wire [7:0]inverter_flt_desat;
  wire [7:0]inverter_flt_temp;
  wire [7:0]inverter_rdy;
  wire [7:0]inverter_rst;

  design_1 design_1_i
       (.DDR_addr(DDR_addr),
        .DDR_ba(DDR_ba),
        .DDR_cas_n(DDR_cas_n),
        .DDR_ck_n(DDR_ck_n),
        .DDR_ck_p(DDR_ck_p),
        .DDR_cke(DDR_cke),
        .DDR_cs_n(DDR_cs_n),
        .DDR_dm(DDR_dm),
        .DDR_dq(DDR_dq),
        .DDR_dqs_n(DDR_dqs_n),
        .DDR_dqs_p(DDR_dqs_p),
        .DDR_odt(DDR_odt),
        .DDR_ras_n(DDR_ras_n),
        .DDR_reset_n(DDR_reset_n),
        .DDR_we_n(DDR_we_n),
        .FIXED_IO_ddr_vrn(FIXED_IO_ddr_vrn),
        .FIXED_IO_ddr_vrp(FIXED_IO_ddr_vrp),
        .FIXED_IO_mio(FIXED_IO_mio),
        .FIXED_IO_ps_clk(FIXED_IO_ps_clk),
        .FIXED_IO_ps_porb(FIXED_IO_ps_porb),
        .FIXED_IO_ps_srstb(FIXED_IO_ps_srstb),
        .adc1_clkout(adc1_clkout),
        .adc1_cnv(adc1_cnv),
        .adc1_sck(adc1_sck),
        .adc1_sdo(adc1_sdo),
        .adc2_clkout(adc2_clkout),
        .adc2_cnv(adc2_cnv),
        .adc2_sck(adc2_sck),
        .adc2_sdo(adc2_sdo),
        .encoder_a(encoder_a),
        .encoder_alarm_a(encoder_alarm_a),
        .encoder_alarm_b(encoder_alarm_b),
        .encoder_alarm_d(encoder_alarm_d),
        .encoder_alarm_z(encoder_alarm_z),
        .encoder_b(encoder_b),
        .encoder_z(encoder_z),
        .gpioa(gpioa),
        .gpiob(gpiob),
        .inverter1_pwm(inverter1_pwm),
        .inverter2_pwm(inverter2_pwm),
        .inverter3_pwm(inverter3_pwm),
        .inverter4_pwm(inverter4_pwm),
        .inverter5_pwm(inverter5_pwm),
        .inverter6_pwm(inverter6_pwm),
        .inverter7_pwm(inverter7_pwm),
        .inverter8_pwm(inverter8_pwm),
        .inverter_flt_desat(inverter_flt_desat),
        .inverter_flt_temp(inverter_flt_temp),
        .inverter_rdy(inverter_rdy),
        .inverter_rst(inverter_rst));
endmodule
