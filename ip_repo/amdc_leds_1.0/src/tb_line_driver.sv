`timescale 1ns / 1ps

module tb_line_driver();

reg clk, rst_n;
reg code;
reg start;

wire done;
wire dout;

integer i;

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

	for (i = 0; i <= 1; i=i+1)
	begin
		// Send a logic value (0, 1, RST) down the line...
		code = i;
		start = 1;
		@(posedge clk);
		start = 0;
		
		// Insert extra clock cycle before checking for done signal!
		@(posedge clk);
		while (!done) @(posedge clk);
	end
	
	// Let some clock edges pass...
	repeat (20) begin
		@(posedge clk);
	end
	
	$stop;
	
end

always
	#5 clk <= ~clk;


line_driver iDUT(
	.clk(clk),
	.rst_n(rst_n),
	.code(code),
	.start(start),
	.done(done),
	.dout(dout)
);

endmodule