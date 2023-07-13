`timescale 1ns / 1ns

////////////////////////////////////////////////////////
//
// dy_dt floating point solver
// 
// Designer: Lingkai (Harry) Zhao
//
// This floating point differential solver sovles for:
//     dy_dt = (inv_L * v) - (R_div_L * i) - (k_e_div_L * w);
//     dw_dt = (kt_div_J * i) - (b_div_J * w) - (inv_J * load);
//
// To use this module, modify the paramters below in 32 bit IEEE floating point values
///////////////////////////////////////////////////////

module dy_dt_fp (    
    input wire [31:0] v,       // applied voltage
	input wire [31:0] load,    // applied load
    input wire [31:0] i,       // estimated current
	input wire [31:0] w,       // estimated speed
    output wire [31:0] di_dt,
	output wire [31:0] dw_dt);

// Not directly used paramters. Recommand enter the values here and 
// manually calculate the 32-bit floating point values and enter them in the next secion
//parameter L = 0.0000142;  
//parameter R = 0.283;
//parameter J = 0.00000266;
//parameter b = 0.00000886;
//parameter k_e = 0.00928; // EMF constant
//parameter k_t = 0.00928;

// Enter each value below.
// See the comments for the value to be entered
// Website for convertion: https://www.h-schmidt.net/FloatConverter/IEEE754.html
// -------- start modifying values ---------------
parameter inv_L = 32'h47898b45;    // 1/L = 70422.5352112676/H
parameter R_div_L = 32'h469bb328;  // R/L = 19929.5774647887 ohm/H
parameter k_e_div_L = 32'h4423615a; // k_e/L = 653.5211267606
parameter kt_div_J = 32'h43ae6fa4; //kt/J = 348.8721804511
parameter b_div_J = 32'h40552c45; // b/J = 3.3308270677
parameter inv_J = 32'h4712d9fc; //   1/J = 37593.984962406
// -------- end modifying values ---------------

wire [31:0] inv_L_X_v, R_div_L_X_i, k_e_div_L_X_w, kt_div_J_X_i, b_div_J_X_w, inv_L_X_load, di_dt_intermediate, dw_dt_intermediate;

// The below modules implement the di_dt formulas.
FP_mul iinv_L_X_v(.A(inv_L), .B(v), .OUT(inv_L_X_v));
FP_mul iR_div_L_X_i(.A(R_div_L), .B(i), .OUT(R_div_L_X_i));
FP_mul ik_e_div_L_X_w(.A(k_e_div_L), .B(w), .OUT(k_e_div_L_X_w));
FP_mul ikt_div_J_X_i(.A(kt_div_J), .B(i), .OUT(kt_div_J_X_i));
FP_mul ib_div_J_X_w(.A(b_div_J), .B(w), .OUT(b_div_J_X_w));
FP_mul iinv_L_X_load(.A(inv_J), .B(load), .OUT(inv_L_X_load));

FP_adder iAdder0(.A(inv_L_X_v), .B({~R_div_L_X_i[31],R_div_L_X_i[30:0]}), .out(di_dt_intermediate)); 
FP_adder iAdder1(.A(di_dt_intermediate), .B({~k_e_div_L_X_w[31],k_e_div_L_X_w[30:0]}), .out(di_dt)); 
FP_adder iAdder2(.A(kt_div_J_X_i), .B({~b_div_J_X_w[31],b_div_J_X_w[30:0]}), .out(dw_dt_intermediate)); 
FP_adder iAdder3(.A(dw_dt_intermediate), .B({~inv_L_X_load[31],inv_L_X_load[30:0]}), .out(dw_dt)); 
//assign di_dt = ((inv_L * v) >> inv_L_EXPONENT) - ((R_div_L * i) >> R_div_L_EXPONENT) - ((k_e_div_L * w) >> k_e_div_L_EXPONENT);
//assign dw_dt = ((kt_div_J * i) >> kt_div_J_EXPONENT) - ((b_div_J * w) >> b_div_J_EXPONENT) - ((inv_J * load) >> inv_J_EXPONENT);
endmodule