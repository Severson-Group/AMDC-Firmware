
`timescale 1 ns / 1 ps

	module amdc_timing_manager_v1_0 #
	(
		// Users to add parameters here

		// User parameters ends
		// Do not modify the parameters beyond this line


		// Parameters of Axi Slave Bus Interface S00_AXI
		parameter integer C_S00_AXI_DATA_WIDTH	= 32,
		parameter integer C_S00_AXI_ADDR_WIDTH	= 6
	)
	(
		// Users to add ports here
		output wire trigger,
		input  wire pwm_carrier_low,
		input  wire pwm_carrier_high,
		input  wire adc_done,
		input  wire encoder_done,
		input  wire amds_0_done,
		input  wire amds_1_done,
		input  wire amds_2_done,
		input  wire amds_3_done,
		input  wire eddy_0_done,
		input  wire eddy_1_done,
		input  wire eddy_2_done,
		input  wire eddy_3_done,
		output wire sched_isr,
		output wire en_amds_0,
		output wire en_amds_1,
		output wire en_amds_2,
		output wire en_amds_3,
		output wire en_eddy_0,
		output wire en_eddy_1,
		output wire en_eddy_2,
		output wire en_eddy_3,
    	output wire en_adc,
		output wire en_encoder,
		output wire [2:0] debug,

		// User ports ends
		// Do not modify the ports beyond this line


		// Ports of Axi Slave Bus Interface S00_AXI
		input wire  s00_axi_aclk,
		input wire  s00_axi_aresetn,
		input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_awaddr,
		input wire [2 : 0] s00_axi_awprot,
		input wire  s00_axi_awvalid,
		output wire  s00_axi_awready,
		input wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_wdata,
		input wire [(C_S00_AXI_DATA_WIDTH/8)-1 : 0] s00_axi_wstrb,
		input wire  s00_axi_wvalid,
		output wire  s00_axi_wready,
		output wire [1 : 0] s00_axi_bresp,
		output wire  s00_axi_bvalid,
		input wire  s00_axi_bready,
		input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_araddr,
		input wire [2 : 0] s00_axi_arprot,
		input wire  s00_axi_arvalid,
		output wire  s00_axi_arready,
		output wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_rdata,
		output wire [1 : 0] s00_axi_rresp,
		output wire  s00_axi_rvalid,
		input wire  s00_axi_rready
	);
// Instantiation of Axi Bus Interface S00_AXI
	amdc_timing_manager_v1_0_S00_AXI # ( 
		.C_S_AXI_DATA_WIDTH(C_S00_AXI_DATA_WIDTH),
		.C_S_AXI_ADDR_WIDTH(C_S00_AXI_ADDR_WIDTH)
	) amdc_timing_manager_v1_0_S00_AXI_inst (
		.S_AXI_ACLK(s00_axi_aclk),
		.S_AXI_ARESETN(s00_axi_aresetn),
		.S_AXI_AWADDR(s00_axi_awaddr),
		.S_AXI_AWPROT(s00_axi_awprot),
		.S_AXI_AWVALID(s00_axi_awvalid),
		.S_AXI_AWREADY(s00_axi_awready),
		.S_AXI_WDATA(s00_axi_wdata),
		.S_AXI_WSTRB(s00_axi_wstrb),
		.S_AXI_WVALID(s00_axi_wvalid),
		.S_AXI_WREADY(s00_axi_wready),
		.S_AXI_BRESP(s00_axi_bresp),
		.S_AXI_BVALID(s00_axi_bvalid),
		.S_AXI_BREADY(s00_axi_bready),
		.S_AXI_ARADDR(s00_axi_araddr),
		.S_AXI_ARPROT(s00_axi_arprot),
		.S_AXI_ARVALID(s00_axi_arvalid),
		.S_AXI_ARREADY(s00_axi_arready),
		.S_AXI_RDATA(s00_axi_rdata),
		.S_AXI_RRESP(s00_axi_rresp),
		.S_AXI_RVALID(s00_axi_rvalid),
		.S_AXI_RREADY(s00_axi_rready),
		.pwm_carrier_high(pwm_carrier_high),
		.pwm_carrier_low(pwm_carrier_low),
		.trigger(trigger),
		.sched_isr(sched_isr),
		.adc_done(adc_done),
		.encoder_done(encoder_done),
    	.amds_0_done(amds_0_done),
		.amds_1_done(amds_1_done),
		.amds_2_done(amds_2_done),
		.amds_3_done(amds_3_done),
		.eddy_0_done(eddy_0_done),
		.eddy_1_done(eddy_1_done),
		.eddy_2_done(eddy_2_done),
		.eddy_3_done(eddy_3_done),
		.en_amds_0(en_amds_0),
		.en_amds_1(en_amds_1),
		.en_amds_2(en_amds_2),
		.en_amds_3(en_amds_3),
		.en_eddy_0(en_eddy_0),
		.en_eddy_1(en_eddy_1),
		.en_eddy_2(en_eddy_2),
		.en_eddy_3(en_eddy_3),
		.en_adc(en_adc),
		.en_encoder(en_encoder),
		.debug(debug)
	);

	// Add user logic here

	// User logic ends

	endmodule
