`timescale 1ns / 1ps

module tb_serial_16b();

reg clk, rst_n;

wire SYNC;
wire SCLK;
wire DIN;

reg [15:0] data;
reg start;
wire done;

initial begin
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

	// Send multiple chunks of data
	repeat (2) begin
		data = 16'b1000_0000_0000_0101;
		start = 1;
		@(posedge clk);
		start = 0;
		
		// Wait for it to be sent
		while (!done) @(posedge clk);
		
		// Wait between data output
		repeat (100) begin
			@(posedge clk);
		end
	end
end

always
	#5 clk <= ~clk;

serial_16b iDUT(
	.clk(clk),
	.rst_n(rst_n),
	.data(data),
	.start(start),
	.done(done),
	.sclk_div(32'd16),
	.SYNC(SYNC),
	.SCLK(SCLK),
	.DIN(DIN)
);

endmodule
