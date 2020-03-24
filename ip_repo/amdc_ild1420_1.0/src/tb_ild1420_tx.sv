`timescale 1ns / 1ps

module tb_ild1420_tx();

reg clk, rst_n;
reg [15:0] distance;
reg [1:0] error;
reg start;
wire done;
wire dout;

initial begin
	// Set the inputs
	start = 0;
	distance = 16'h0F8A;
	error = 2'b10;

	clk	= 1'b0;
	rst_n = 1'b1;
	
	@(posedge clk);
	@(posedge clk);
	
	rst_n = 1'b0;
	@(posedge clk);
	@(negedge clk);
	rst_n = 1'b1;
	
	@(posedge clk);
	@(posedge clk);
	
	// Add buffer of clocks
	repeat (10) @(posedge clk);
	
	start = 1;
	@(posedge clk);
	start = 0;

	// Wait a single clock to let "done" become LOW
	@(posedge clk);
	
	// Wait for it to be sent
	while (!done) @(posedge clk);
	
	// Add buffer of clocks
	repeat (50) @(posedge clk);
	
	$stop;
end

always
	#5 clk <= ~clk;

ild1420_tx iDUT(
	.clk(clk),
	.rst_n(rst_n),
	.distance(distance),
	.error(error),
	.start(start),
	.dout(dout),
	.done(done)
);

endmodule