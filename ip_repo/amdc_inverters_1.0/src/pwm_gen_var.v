`timescale 1ns / 1ps

module pwm_gen_var(clk, rst_n, carrier, carrier_max, D, So);

input clk, rst_n;

input [15:0] carrier_max;  // carrier maximum, configurable from C code
input [15:0] carrier;      // carrier, triangle waveform
input [15:0] D;            // duty cycle, expressed as 16-bit number: (D = carrier_max) => duty cycle of 1

output reg So;             // output signal 

always @(posedge clk, negedge rst_n)
	if (!rst_n)
		So <= 1'b0;	
	else
		// Compare to duty cycle:
		if (D == 16'd0)
			So <= 1'b0;
		else if (D == carrier_max)
			So <= 1'b1;
		else if (carrier < D)
			So <= 1'b1;
		else
			So <= 1'b0;

endmodule
