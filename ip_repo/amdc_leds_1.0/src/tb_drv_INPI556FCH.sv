`timescale 1ns / 1ps

module tb_drv_INPI1556FCH();

reg clk, rst_n;
reg [23:0] color;
reg start;

wire done;
wire dout;

initial begin
	clk	= 1'b0;
	rst_n = 1'b1;
	
	@(posedge clk);
	@(posedge clk);	
	
	rst_n = 1'b0;
	@(posedge clk);
	@(negedge clk);
	rst_n = 1'b1;
	
	
	// Let some clock edges pass...
	repeat (20) begin
		@(posedge clk);
	end


	color = 24'b0000_1111_1111_0000_1010_0101;
	start = 1;
	@(posedge clk);
	start = 0;
	
	// Insert extra clock cycle before checking for done signal!
	@(posedge clk);
	while (!done) @(posedge clk);

	// Let some clock edges pass...
	repeat (20) begin
		@(posedge clk);
	end
	
	$stop;
end

always
	#5 clk <= ~clk;


drv_INPI1556FCH iDUT(
	.clk(clk),
	.rst_n(rst_n),
	.color(color),
	.start(start),
	.done(done),
	.dout(dout)
);

endmodule