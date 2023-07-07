// my_custom_adder.v

module my_custom_adder(clk, rst_n, in1, in2, out);

input clk, rst_n;

// Arguments supplied by the user
input wire [31:0] in1;
input wire [31:0] in2;

// Holds the sum of the inputs
output reg [31:0] out;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		out <= 32'b0;
	else
		out <= (in1 << 3) + (in2 >> 2) - 32'd10203;
end

endmodule