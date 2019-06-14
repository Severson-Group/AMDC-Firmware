`timescale 1ns / 1ps

// ****************
// Triangle Carrier
// ****************
//
// Original implementation by Eric Severson,
// 
// Cleaned up by Nathan Petersen, March 7, 2019
//
// ------
// 
// Generates a triangle waveform from 0 to carrier_max to 0, etc.
//
// INPUTS:
//   - clk:     system clock, expected 50MHz
//   - rst_n:   active low reset signal
//   - divider: 8-bit clock divider
//              
//       Switching frequency = ((50e6 / (divider+1)) / (2*carrier_max)) Hz
//
// OUTPUTS:
//   - carrier: triangle waveform
//
//

module triangle_carrier(clk, rst_n, divider, carrier, carrier_high, carrier_low, carrier_max);

input clk, rst_n;
input [7:0] divider;
input [15:0] carrier_max;
output wire [15:0] carrier;

output wire carrier_high;
output wire carrier_low;

reg [15:0] count;
reg [7:0] div_count;


// Find high and low points of carrier
assign carrier_low  = (carrier == 16'b0)       ? 1'b1 : 1'b0;
assign carrier_high = (carrier == carrier_max) ? 1'b1 : 1'b0;


// Direction to count:
// 1 for up, 0 for down
reg dp;

assign carrier = count;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n) begin
		// Reset registers
		count     <= 16'b0;
		div_count <= 8'b0;
		dp        <= 1'b1;
	end else begin
		if (div_count < divider) begin
			div_count <= div_count + 8'd1;
		end
		
		else begin
			div_count <= 8'd0;
		
			if (dp == 1'b1) begin
				// Counter going up
				
				if (count < carrier_max) begin
					// count < carrier_max
					count <= count + 1;
				end
				
				else begin
					// count == carrier_max
					dp <= 1'b0;
					count <= count - 1;
				end
			end
			
			else begin
				// Counter going down
				
				if (count > 8'b0) begin
					// count > 0
					count <= count - 1;
				end
				
				else begin
					// count == 0
					dp <= 1'b1;
					count <= count + 1;
				end
			end
		end
	end
end

endmodule
