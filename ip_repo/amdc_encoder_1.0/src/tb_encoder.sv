`timescale 1ns / 1ps

`define PULSES_PER_REVOLUTION_BITS	(32'd12)

module tb_encoder();

reg clk, rst_n;

reg A;
reg B;
reg Z;

wire [31:0] counter;
wire [31:0] position;


task sim_revs;
	input int dir;
	input int num_revs;
	input int cycles_per_rev;
	
	begin
		// Simulate revs
		repeat (num_revs) begin
			Z = 1;
			if (dir) A = 1; else B = 1;
			repeat (5) @(posedge clk);
			Z = 0;
			if (dir) B = 1; else A = 1;
			repeat (5) @(posedge clk);
			if (dir) A = 0; else B = 0;
			repeat (5) @(posedge clk);
			if (dir) B = 0; else A = 0;
			repeat (5) @(posedge clk);


			// Simulate (`cycles_per_rev` - 1) more cycles (or 4x pulses)
			repeat (cycles_per_rev - 1) begin
				if (dir) A = 1; else B = 1;
				repeat (5) @(posedge clk);
				if (dir) B = 1; else A = 1;
				repeat (5) @(posedge clk);
				if (dir) A = 0; else B = 0;
				repeat (5) @(posedge clk);
				if (dir) B = 0; else A = 0;
				repeat (5) @(posedge clk);
			end
		end
	end
endtask






initial begin
	clk	= 1'b0;
	rst_n = 1'b1;
	
	A = 0;
	B = 0;
	Z = 0;
	
	@(posedge clk);
	@(posedge clk);	
	
	rst_n = 1'b0;
	@(posedge clk);
	@(negedge clk);
	rst_n = 1'b1;

	// Simulate some amount of cycles before hitting
	// Z pulse...
	repeat (5) begin
		A = 1;
		repeat (5) @(posedge clk);
		B = 1;
		repeat (5) @(posedge clk);
		A = 0;
		repeat (5) @(posedge clk);
		B = 0;
		repeat (5) @(posedge clk);
	end
	
	sim_revs(1, 1, (1 << `PULSES_PER_REVOLUTION_BITS) / 4);
	sim_revs(0, 1, (1 << `PULSES_PER_REVOLUTION_BITS) / 4);
	sim_revs(1, 1, (1 << `PULSES_PER_REVOLUTION_BITS) / 4);
	sim_revs(1, 1, (1 << `PULSES_PER_REVOLUTION_BITS) / 4);
	
	$stop();
end

always
	#5 clk <= ~clk;

encoder iDUT(
	.clk(clk),
	.rst_n(rst_n),
	.A(A),
	.B(B),
	.Z(Z),
	.counter(counter),
	.position(position),
	.pulses_per_rev_bits(`PULSES_PER_REVOLUTION_BITS) // 256 pulses => 64 cycles
);

endmodule
