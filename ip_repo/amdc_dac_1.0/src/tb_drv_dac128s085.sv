`timescale 1ns / 1ps

module tb_drv_dac128s085();

reg clk, rst_n;

wire SYNC;
wire SCLK;
wire DIN;

reg [11:0] dac1;
reg [11:0] dac2;
reg [11:0] dac3;
reg [11:0] dac4;
reg [11:0] dac5;
reg [11:0] dac6;
reg [11:0] dac7;
reg [11:0] dac8;

initial begin
	dac1 = 12'b0000_0000_0001;
	dac2 = 12'b0000_0000_0011;
	dac3 = 12'b0000_0000_0111;
	dac4 = 12'b0000_0000_1111;
	dac5 = 12'b0000_0001_1111;
	dac6 = 12'b0000_0011_1111;
	dac7 = 12'b0000_0111_1111;
	dac8 = 12'b0000_1111_1111;

	clk	= 1'b0;
	rst_n = 1'b1;
	
	@(posedge clk);
	@(posedge clk);	
	
	rst_n = 1'b0;
	@(posedge clk);
	@(negedge clk);
	rst_n = 1'b1;
	

	// Let er rip!
	repeat (100000) begin
		@(posedge clk);
	end
end

always
	#5 clk <= ~clk;



drv_dac128s085 iDUT(
	.clk(clk),
	.rst_n(rst_n),
	.dac1(dac1),
	.dac2(dac2),
	.dac3(dac3),
	.dac4(dac4),
	.dac5(dac5),
	.dac6(dac6),
	.dac7(dac7),
	.dac8(dac8),
	.sclk_div(32'd8),
	.sample_delay(32'd100),
	.SYNC(SYNC),
	.SCLK(SCLK),
	.DIN(DIN)
);



endmodule