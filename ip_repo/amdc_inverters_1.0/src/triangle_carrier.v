`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    17:22:05 06/07/2013 
// Design Name: 
// Module Name:    triangle_carrier 
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
module triangle_carrier(sys_clk, sys_ce,
    divider, carrier);


input sys_clk, sys_ce;
input [7:0] divider; // clock divider, switching frequency = 50e6/(divider+1)/510 Hz
output wire [7:0] carrier;

reg [7:0] count = 0;	
reg [7:0] div_count = 0;
reg dp = 1;		//direction to count: 1 for up, 0 for down



wire CLK;
assign CLK = sys_clk & sys_ce;
assign carrier = count;

always @(posedge CLK)
	begin
	if (div_count < divider)
		div_count <= div_count + 8'd1;
	else
	begin
		div_count <= 8'd0;
       if (dp == 1'b1)
			if (count < 8'hFF)
          count <=count+1;
         else //(count == 7'hFF) 
			begin
          dp <= 1'b0;
			 count <= count-1;
			end
		else //dp == 1'b0 <-- going down
			if (count > 7'b0)
				count <= count-1;
			else	//count == 0
			begin
				count <= count+1;
				dp <= 1'b1;
			end
	end
			
end
endmodule
