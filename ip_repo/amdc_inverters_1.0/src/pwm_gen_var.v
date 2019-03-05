`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    15:32:12 08/28/2012 
// Design Name: 
// Module Name:    pwm_gen_var 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module pwm_gen_var(sys_clk, sys_ce, carrier, D, So);

input sys_clk, sys_ce; //50MHz clock
input [7:0] carrier; // clock divider, switching frequency = 50e6/(divider+1)/510 Hz
input [7:0] D;			// duty cycle, expressed as 8 bit number: 7'hFF = duty cycle of 1
output reg So = 0;//	output signal 

wire CLK;
assign CLK = sys_clk & sys_ce;

always @(posedge CLK)
	begin			
		//now compare to duty cycle:
		if (D == 8'd0)
			So <= 1'b0;
		else if (D == 8'd255)
			So <= 1'b1;
		else if (carrier < D)
			So <= 1'b1;
		else
			So <= 1'b0;
	end
				
	
endmodule
