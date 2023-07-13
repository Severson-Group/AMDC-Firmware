`timescale 1ns / 1ns
////////////////////////////////////////////////////////
//
// dy_dt floating point solver
// 
// Designer: Lingkai (Harry) Zhao
//
// This module implement the fourth order Runge Kutta solver in integer format, given dy_dt
// It outputs the computed i(current), w(speed) of the current model
//
// To use this module, modify the paramters below in 32 bit IEEE floating point format:
//
///////////////////////////////////////////////////////

module solver_rk4_fp (    
    input wire clk, rst_n, 
    input wire [31:0] voltage,  // voltage at present in 32 bit IEEE floating point format
    input wire [31:0] load,     // current at present in 32 bit IEEE floating point format
    output reg [31:0] i,
    output reg [31:0] w);

parameter STEP = 32'h322bcc77; // STEP size is 10 ns in this application
parameter STEP_div_2 = 32'h31abcc77; // 5ns
parameter h_div_6 = 32'h30e5109f;//STEP/6 

wire [31:0] didt_1, didt_2, didt_3, didt_4; //output from dy/dt, exponent == current_exponent
wire [31:0] dwdt_1, dwdt_2, dwdt_3, dwdt_4;
reg [31:0] voltage_0,voltage_1, load_0, load_1; // correspond to voltage at (tn-h), (tn-h/2)

reg tmr;
//reg [31:0] i,w;
wire [31:0] i_slope, w_slope, update_i, update_w;

// values used for computation. The names explains the purpose of each wire.
wire [31:0] step_x_didt_1, step_x_didt_2, step_x_didt_3;
wire [31:0] step_x_dwdt_1, step_x_dwdt_2, step_x_dwdt_3;
wire [31:0] i_and_step_x_didt_1_div_2, w_and_step_x_dwdt_1_div_2;
wire [31:0] i_and_step_x_didt_2_div_2, w_and_step_x_dwdt_2_div_2;
wire [31:0] i_and_step_x_didt_3, w_and_step_x_dwdt_3;

wire[31:0] value0, value1, value2, value3, value4, value5;

// a trick to double a floating point value (add the exponent by 1)
wire [7:0] exp_plus_1_0, exp_plus_1_1, exp_plus_1_2, exp_plus_1_3;
assign exp_plus_1_0 = didt_2[30:23] + 1;
assign exp_plus_1_1 = didt_3[30:23] + 1;
assign exp_plus_1_2 = dwdt_2[30:23] + 1;
assign exp_plus_1_3 = dwdt_3[30:23] + 1;

// This clk is two times faster than the STEP time (if the STEP time is 2 ns, then the clk is 1ns)
always_ff @(posedge clk, negedge rst_n) begin
    if (!rst_n) begin
        voltage_0 <= 32'h0000;
        voltage_1 <= 32'h0000;
    end else begin
        voltage_0 <= voltage_1;    // voltage_0 becomes voltage at the (tn-h)
        voltage_1 <= voltage;      // voltage_1 becomes voltage at the (tn-h/2), voltage gets the updated voltage at present
    end
end

// This clk is two times faster than the STEP time (if the STEP time is 2 ns, then the clk is 1ns)
always_ff @(posedge clk, negedge rst_n) begin
    if (!rst_n) begin
        load_0 <= 32'h0000;
        load_1 <= 32'h0000;
    end else begin
        load_0 <= load_1;    // load_0 gets load at the oldest time (tn-h)
        load_1 <= load;      // load_1 gets load at (tn-h/2)
    end
end

// An alternative timer. The i/w value only increase with a freqency that is half of the clk speed. (clock runs 5ns, we update the value at 10ns)
always @(posedge clk, negedge rst_n)
    if (!rst_n)
        tmr <= 1'b0;
    else
        tmr <= tmr + 1'b1;

// update i
always @(posedge clk, negedge rst_n)
    if (!rst_n)
        i <= 32'h0000;
    else if (tmr)
        i <= update_i ;

// update w
always @(posedge clk, negedge rst_n)
    if (!rst_n)
        w <= 32'h0000;
    else if (tmr)
        w <= update_w;

// The modules below are used to implement the following two equations
// assign i_slope = h_div_6 * (didt_1 + {didt_2[126:0], 1'b0} + {didt_3[126:0], 1'b0} + didt_4);
// assign w_slope = h_div_6 * (dwdt_1 + {dwdt_2[126:0], 1'b0} + {dwdt_3[126:0], 1'b0} + dwdt_4);

// compute STEP x di_dt
FP_mul istep_x_didt_1(.A(STEP_div_2), .B(didt_1), .OUT(step_x_didt_1));
FP_mul istep_x_didt_2(.A(STEP_div_2), .B(didt_2), .OUT(step_x_didt_2));
FP_mul istep_x_didt_3(.A(STEP), .B(didt_3), .OUT(step_x_didt_3));

// compute STEP x dw_dt
FP_mul istep_x_dwdt_1(.A(STEP_div_2), .B(dwdt_1), .OUT(step_x_dwdt_1));
FP_mul istep_x_dwdt_2(.A(STEP_div_2), .B(dwdt_2), .OUT(step_x_dwdt_2));
FP_mul istep_x_dwdt_3(.A(STEP), .B(dwdt_3), .OUT(step_x_dwdt_3));

// Intermediates used for computing i_slope and w_slope
FP_adder iAdder0(.A(i), .B(step_x_didt_1), .out(i_and_step_x_didt_1_div_2)); 
FP_adder iAdder1(.A(w), .B(step_x_dwdt_1), .out(w_and_step_x_dwdt_1_div_2)); 

FP_adder iAdder2(.A(i), .B(step_x_didt_2), .out(i_and_step_x_didt_2_div_2)); 
FP_adder iAdder3(.A(w), .B(step_x_dwdt_2), .out(w_and_step_x_dwdt_2_div_2)); 

FP_adder iAdder4(.A(i), .B(step_x_didt_3), .out(i_and_step_x_didt_3)); 
FP_adder iAdder5(.A(w), .B(step_x_dwdt_3), .out(w_and_step_x_dwdt_3)); 

// floating point dy_dt modules
dy_dt_fp dy_dt_1(.v(voltage_0), .load(load_0), .i(i), .w(w), .dw_dt(dwdt_1), .di_dt(didt_1));
dy_dt_fp dy_dt_2(.v(voltage_1), .load(load_1),.i(i_and_step_x_didt_1_div_2), .w(w_and_step_x_dwdt_1_div_2), .dw_dt(dwdt_2), .di_dt(didt_2));
dy_dt_fp dy_dt_3(.v(voltage_1), .load(load_1),.i(i_and_step_x_didt_2_div_2), .w(w_and_step_x_dwdt_2_div_2), .dw_dt(dwdt_3), .di_dt(didt_3));
dy_dt_fp dy_dt_4(.v(voltage), .load(load),.i(i_and_step_x_didt_3), .w(w_and_step_x_dwdt_3), .dw_dt(dwdt_4), .di_dt(didt_4));

// Compute the delta value for i and w
FP_adder iAdder6(.A(didt_1), .B({didt_2[31], exp_plus_1_0, didt_2[22:0]}), .out(value0)); 
FP_adder iAdder7(.A(didt_4), .B({didt_3[31], exp_plus_1_1, didt_3[22:0]}), .out(value1)); 
FP_adder iAdder8(.A(value0), .B(value1), .out(value2)); 
FP_mul ii_slope(.A(h_div_6), .B(value2), .OUT(i_slope));

FP_adder iAdder9(.A(dwdt_1), .B({dwdt_2[31], exp_plus_1_2, dwdt_2[22:0]}), .out(value3)); 
FP_adder iAdder10(.A(dwdt_4), .B({dwdt_3[31], exp_plus_1_3, dwdt_3[22:0]}), .out(value4)); 
FP_adder iAdder11(.A(value3), .B(value4), .out(value5)); 
FP_mul iw_slope(.A(h_div_6), .B(value5), .OUT(w_slope));

FP_adder iUpdateI(.A(i), .B(i_slope), .out(update_i)); 
FP_adder iUpdateW(.A(w), .B(w_slope), .out(update_w)); 

endmodule