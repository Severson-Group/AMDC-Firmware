`timescale 1ns / 1ps

// ****************
// Triangle Carrier
// ****************
//
// Original implementation by Eric Severson,
// 
// Cleaned up by Nathan Petersen, March 5, 2019
//
// ------
// 
// Generates a triangle waveform from 0 to 255 to 0, etc.
//
// INPUTS:
//   - clk:     system clock, expected 50MHz
//   - rst_n:   active low reset signal
//   - divider: 8-bit clock divider
//              
//       Switching frequency = ((50e6 / (divider+1)) / (2*255)) Hz
//
// OUTPUTS:
//   - carrier: triangle waveform
//
//

module triangle_carrier(clk, rst_n, divider, carrier);

input clk, rst_n;
input [7:0] divider;
output wire [7:0] carrier;

reg [7:0] count;	
reg [7:0] div_count;

// Direction to count:
// 1 for up, 0 for down
reg dp;

assign carrier = count;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n) begin
		// Reset registers
		count     <= 8'b0;
		div_count <= 8'b0;
		dp        <= 1'b1;
	end else begin
		if (div_count < divider)
			div_count <= div_count + 8'd1;
		else begin
			div_count <= 8'd0;
		
			if (dp == 1'b1) begin
				// Counter going up
				
				if (count < 8'hFF) begin
					// count < 8'hFF
					count <= count + 1;
				end else begin
					// count == 8'hFF
					dp <= 1'b0;
					count <= count - 1;
				end
			end else begin
				// Counter going down
				
				if (count > 8'b0) begin
					// count > 0
					count <= count - 1;
				end else begin
					// count == 0
					dp <= 1'b1;
					count <= count + 1;
				end
			end
		end
	end
end

endmodule
