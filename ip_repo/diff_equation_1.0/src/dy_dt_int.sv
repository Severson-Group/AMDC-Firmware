`timescale 1ns / 1ns
////////////////////////////////////////////////////////
//
// dy_dt integer solver
// 
// Designer: Lingkai (Harry) Zhao
//
// This differential solver sovles for:
//     dy_dt = (inv_L * v) - (R_div_L * i) - (k_e_div_L * w);
//     dw_dt = (kt_div_J * i) - (b_div_J * w) - (inv_J * load);
//
// This is a 64 bit inter solver. It has better precison(if used properly) but it does
// not protect the user from overflow.
//
// To use this module, modify the paramters below:
//
// How to interpret the input and output of this module:
// The input and output are in relative relationship: 
// If the user specify the input with 0xFFFF as 0.5, 
// then an output of 0xB2C55 would represent: 0xB2C55/0xFFFF x 1 = 5.5875
///////////////////////////////////////////////////////
//input v an i must have same exponent
module dy_dt_int (    
    input wire [63:0] v,     // applied voltage
	input wire [63:0] load,  // applied load
    input wire [63:0] i,     // estimated current
	input wire [63:0] w,     // estimated speed
    output wire [127:0] di_dt,
	output wire [127:0] dw_dt);

// Not directly used paramters. Recommand enter the values here and 
// manually calculate the 64-bit interger representation and enter them in the next secion
//parameter L = 0.0000142;  
//parameter R = 0.283;
//parameter J = 0.00000266;
//parameter b = 0.00000886;
//parameter k_e = 0.00928; // EMF constant
//parameter k_t = 0.00928;

// How to interpret the entered values:
// The X value represent "X * 2 ^ (-X_EXPONENT)". 
// For example, "X == 1324 with X_EXPONENT == 10" would represent "1324/1024 == 1.293 H"
// The maximum X_EXPONENT is 64. 
// The more accurate the value is represented, the better precison this module has.
// For example, representation of 0.255 can also be 4 with exponent of 4, wich is 0.25
//              but representation of 0.255 can be 16712 with exponent of 16, which is 0.25500488281 (much better precision)

parameter inv_L = 70423;    // 70423
parameter inv_L_EXPONENT = 0;

parameter R_div_L = 20141;  // 20141
parameter R_div_L_EXPONENT = 0;

parameter k_e_div_L = 654; // 654
parameter k_e_div_L_EXPONENT = 0;

parameter kt_div_J = 3489;
parameter kt_div_J_EXPONENT = 0; 

parameter b_div_J = 3411;  //3411 * 2^-10
parameter b_div_J_EXPONENT = 10;

parameter inv_J = 375940;
parameter inv_J_EXPONENT = 0;

assign di_dt = ((inv_L * v) >> inv_L_EXPONENT) - ((R_div_L * i) >> R_div_L_EXPONENT) - ((k_e_div_L * w) >> k_e_div_L_EXPONENT);
assign dw_dt = ((kt_div_J * i) >> kt_div_J_EXPONENT) - ((b_div_J * w) >> b_div_J_EXPONENT) - ((inv_J * load) >> inv_J_EXPONENT);
endmodule