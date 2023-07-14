// Another self-explaining module
module left_shifter(In, ShAmt, Out);

input  [23:0] In; 	// Input operand
input  [4:0] ShAmt; // Amount to shift/rotate
output [23:0] Out; 	// Result of shift/rotate

wire [23:0] shft_stg1, shft_stg2, shft_stg3, shft_stg4;

assign shft_stg1 = ShAmt[0] ? {In[22:0],1'b0} : {In};
assign shft_stg2 = ShAmt[1] ? {shft_stg1[21:0],2'b0} : {shft_stg1};
assign shft_stg3 = ShAmt[2] ? {shft_stg2[19:0],4'b0} : {shft_stg2};
assign shft_stg4 = ShAmt[3] ? {shft_stg3[15:0],8'b0} : {shft_stg3};
assign Out = ShAmt[4] ? {shft_stg4[7:0],16'b0} : {shft_stg4};

endmodule