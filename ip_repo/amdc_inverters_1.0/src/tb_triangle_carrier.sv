`timescale 1ns / 1ps

module tb_triangle_carrier();

reg clk, rst_n;

reg [7:0] divider;
reg [15:0] carrier_max;

wire [15:0] carrier;
wire carrier_high;
wire carrier_low;

initial begin
	divider = 8'h4;
	carrier_max = 16'd100;

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

triangle_carrier iDUT(
	.clk(clk),
	.rst_n(rst_n),
	.divider(divider),
	.carrier(carrier),
	.carrier_high(carrier_high),
	.carrier_low(carrier_low),
	.carrier_max(carrier_max)
);

endmodule
