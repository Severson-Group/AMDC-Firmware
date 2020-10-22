
`timescale 1 ns / 1 ps

	module amdc_inv_status_mux_v1_0 #
	(
		// Users to add parameters here

		// User parameters ends
		// Do not modify the parameters beyond this line


		// Parameters of Axi Slave Bus Interface S00_AXI
		parameter integer C_S00_AXI_DATA_WIDTH	= 32,
		parameter integer C_S00_AXI_ADDR_WIDTH	= 8
	)
	(
		// Users to add ports here
        inout [7:0] sts_a_ext,
        inout [7:0] sts_b_ext,
        inout [7:0] sts_c_ext,
        inout [7:0] sts_d_ext,
        
        // Default status line connections to inverter IP
        input [7:0] sts_a_int,
        output [7:0] sts_b_int,
        output [7:0] sts_c_int,
        output [7:0] sts_d_int,
        
        // Device I/O      
        output [3:0] device1_in,
        input [3:0] device1_out, 
        output [3:0] device2_in,
        input [3:0] device2_out,   
        output [3:0] device3_in,
        input [3:0] device3_out,   
        output [3:0] device4_in,
        input [3:0] device4_out,  
        output [3:0] device5_in,
        input [3:0] device5_out,    
        output [3:0] device6_in,
        input [3:0] device6_out,   
        output [3:0] device7_in,
        input [3:0] device7_out,
        output [3:0] device8_in,
        input [3:0] device8_out, 
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
	amdc_inv_status_mux_v1_0_S00_AXI # ( 
		.C_S_AXI_DATA_WIDTH(C_S00_AXI_DATA_WIDTH),
		.C_S_AXI_ADDR_WIDTH(C_S00_AXI_ADDR_WIDTH)
	) amdc_inv_status_mux_v1_0_S00_AXI_inst (
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
		.sts_a_ext(sts_a_ext),
		.sts_b_ext(sts_b_ext),
		.sts_c_ext(sts_c_ext),
		.sts_d_ext(sts_d_ext),
		.sts_a_int(sts_a_int),
		.sts_b_int(sts_b_int),
		.sts_c_int(sts_c_int),
		.sts_d_int(sts_d_int),
		.device1_in(device1_in),
		.device1_out(device1_out),
		.device2_in(device2_in),
		.device2_out(device2_out),
		.device3_in(device3_in),
		.device3_out(device3_out),
		.device4_in(device4_in),
		.device4_out(device4_out),
		.device5_in(device5_in),
		.device5_out(device5_out),
		.device6_in(device6_in),
		.device6_out(device6_out),
		.device7_in(device7_in),
		.device7_out(device7_out),
	    .device8_in(device8_in),
		.device8_out(device8_out)
	);

	// Add user logic here

	// User logic ends

	endmodule
