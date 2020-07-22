//Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2019.1 (win64) Build 2552052 Fri May 24 14:49:42 MDT 2019
//Date        : Mon Mar 23 21:03:46 2020
//Host        : jeff running 64-bit major release  (build 9200)
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
    adc_clkout,
    adc_cnv,
    adc_sck,
    adc_sdo,
    encoder_1a,
    encoder_1b,
    encoder_1z,
    encoder_2a,
    encoder_2b,
    encoder_2z,
    inverter1_pwm,
    inverter2_pwm,
    inverter3_pwm,
    inverter4_pwm,
    inverter5_pwm,
    inverter6_pwm,
    inverter7_pwm,
    inverter8_pwm,
    inverter_sts_a,
    inverter_sts_b,
    inverter_sts_c,
    inverter_sts_d,
    spi1_in,
    spi1_miso,
    spi1_mosi,
    spi1_nss,
    spi1_out,
    spi1_sclk,
    spi2_in,
    spi2_miso,
    spi2_mosi,
    spi2_nss,
    spi2_out,
    spi2_sclk,
    spi3_in,
    spi3_miso,
    spi3_mosi,
    spi3_nss,
    spi3_out,
    spi3_sclk,
    spi4_in,
    spi4_miso,
    spi4_mosi,
    spi4_nss,
    spi4_out,
    spi4_sclk,
    user_led_din);
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
  input adc_clkout;
  output adc_cnv;
  output adc_sck;
  input [7:0]adc_sdo;
  input encoder_1a;
  input encoder_1b;
  input encoder_1z;
  input encoder_2a;
  input encoder_2b;
  input encoder_2z;
  output [5:0]inverter1_pwm;
  output [5:0]inverter2_pwm;
  output [5:0]inverter3_pwm;
  output [5:0]inverter4_pwm;
  output [5:0]inverter5_pwm;
  output [5:0]inverter6_pwm;
  output [5:0]inverter7_pwm;
  output [5:0]inverter8_pwm;
  output [7:0]inverter_sts_a;
  input [7:0]inverter_sts_b;
  input [7:0]inverter_sts_c;
  input [7:0]inverter_sts_d;
  input spi1_in;
  input spi1_miso;
  output [0:0]spi1_mosi;
  output [0:0]spi1_nss;
  output [0:0]spi1_out;
  output [0:0]spi1_sclk;
  input spi2_in;
  input spi2_miso;
  output [0:0]spi2_mosi;
  output [0:0]spi2_nss;
  output [0:0]spi2_out;
  output [0:0]spi2_sclk;
  input spi3_in;
  input spi3_miso;
  output [0:0]spi3_mosi;
  output [0:0]spi3_nss;
  output [0:0]spi3_out;
  output [0:0]spi3_sclk;
  input spi4_in;
  input spi4_miso;
  output [0:0]spi4_mosi;
  output [0:0]spi4_nss;
  output [0:0]spi4_out;
  output [0:0]spi4_sclk;
  output [0:0]user_led_din;

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
  wire adc_clkout;
  wire adc_cnv;
  wire adc_sck;
  wire [7:0]adc_sdo;
  wire encoder_1a;
  wire encoder_1b;
  wire encoder_1z;
  wire encoder_2a;
  wire encoder_2b;
  wire encoder_2z;
  wire [5:0]inverter1_pwm;
  wire [5:0]inverter2_pwm;
  wire [5:0]inverter3_pwm;
  wire [5:0]inverter4_pwm;
  wire [5:0]inverter5_pwm;
  wire [5:0]inverter6_pwm;
  wire [5:0]inverter7_pwm;
  wire [5:0]inverter8_pwm;
  wire [7:0]inverter_sts_a;
  wire [7:0]inverter_sts_b;
  wire [7:0]inverter_sts_c;
  wire [7:0]inverter_sts_d;
  wire spi1_in;
  wire spi1_miso;
  wire [0:0]spi1_mosi;
  wire [0:0]spi1_nss;
  wire [0:0]spi1_out;
  wire [0:0]spi1_sclk;
  wire spi2_in;
  wire spi2_miso;
  wire [0:0]spi2_mosi;
  wire [0:0]spi2_nss;
  wire [0:0]spi2_out;
  wire [0:0]spi2_sclk;
  wire spi3_in;
  wire spi3_miso;
  wire [0:0]spi3_mosi;
  wire [0:0]spi3_nss;
  wire [0:0]spi3_out;
  wire [0:0]spi3_sclk;
  wire spi4_in;
  wire spi4_miso;
  wire [0:0]spi4_mosi;
  wire [0:0]spi4_nss;
  wire [0:0]spi4_out;
  wire [0:0]spi4_sclk;
  wire [0:0]user_led_din;

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
        .adc_clkout(adc_clkout),
        .adc_cnv(adc_cnv),
        .adc_sck(adc_sck),
        .adc_sdo(adc_sdo),
        .encoder_1a(encoder_1a),
        .encoder_1b(encoder_1b),
        .encoder_1z(encoder_1z),
        .encoder_2a(encoder_2a),
        .encoder_2b(encoder_2b),
        .encoder_2z(encoder_2z),
        .inverter1_pwm(inverter1_pwm),
        .inverter2_pwm(inverter2_pwm),
        .inverter3_pwm(inverter3_pwm),
        .inverter4_pwm(inverter4_pwm),
        .inverter5_pwm(inverter5_pwm),
        .inverter6_pwm(inverter6_pwm),
        .inverter7_pwm(inverter7_pwm),
        .inverter8_pwm(inverter8_pwm),
        .inverter_sts_a(inverter_sts_a),
        .inverter_sts_b(inverter_sts_b),
        .inverter_sts_c(inverter_sts_c),
        .inverter_sts_d(inverter_sts_d),
        .spi1_in(spi1_in),
        .spi1_miso(spi1_miso),
        .spi1_mosi(spi1_mosi),
        .spi1_nss(spi1_nss),
        .spi1_out(spi1_out),
        .spi1_sclk(spi1_sclk),
        .spi2_in(spi2_in),
        .spi2_miso(spi2_miso),
        .spi2_mosi(spi2_mosi),
        .spi2_nss(spi2_nss),
        .spi2_out(spi2_out),
        .spi2_sclk(spi2_sclk),
        .spi3_in(spi3_in),
        .spi3_miso(spi3_miso),
        .spi3_mosi(spi3_mosi),
        .spi3_nss(spi3_nss),
        .spi3_out(spi3_out),
        .spi3_sclk(spi3_sclk),
        .spi4_in(spi4_in),
        .spi4_miso(spi4_miso),
        .spi4_mosi(spi4_mosi),
        .spi4_nss(spi4_nss),
        .spi4_out(spi4_out),
        .spi4_sclk(spi4_sclk),
        .user_led_din(user_led_din));
endmodule
