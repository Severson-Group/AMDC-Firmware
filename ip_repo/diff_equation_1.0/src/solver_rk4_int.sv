`timescale 1ns / 1ns
////////////////////////////////////////////////////////
//
// dy_dt integer solver
// 
// Designer: Lingkai (Harry) Zhao
//
// This module implement the fourth order Runge Kutta solver in integer format, given dy_dt
// It outputs the computed i(current), w(speed) of the current model
//
// To use this module, modify the paramters below:
//
// How to interpret the input and output of this module:
// The input and output are in relative relationship: 
// If the user specify the input with 0xFFFF as 0.5, 
// then an output of 0xB2C55 would represent: 0xB2C55/0xFFFF x 1 = 5.5875
///////////////////////////////////////////////////////
//input v an i must have same exponent
module solver_rk4_int (    
    input wire clk, rst_n, 
    input wire [63:0] voltage,  // voltage at present. See above meta section to understand how the value is represented
    input wire [63:0] load,     // load at present
    output reg [63:0] i,
    output reg [63:0] w);

// The STEP value represent "STEP * 2 ^ (-STEP_EXPONENT) Volt". For example, "STEP == 1324 with STEP_EXPONENT = 10" would represent "1324/1024 == 1.293 Sec"
parameter STEP = 38'h2AF31DC461; // STEP size is 10 ns in this application
parameter STEP_EXPONET = 64; // 27'h6B5FCA6 x 2^-64 == 0.00000001 s
parameter h_div_6 = 35'h72884F610;//STEP/6 

wire [127:0] didt_1, didt_2, didt_3, didt_4; //output from dy/dt
wire [127:0] dwdt_1, dwdt_2, dwdt_3, dwdt_4; //output from dy/dt
reg [63:0] voltage_0,voltage_1, load_0, load_1; // correspond to voltage at (tn-h), (tn-h/2)

reg tmr;
//reg [63:0] i,w;
wire [127:0] i_slope, w_slope;
wire [127:0] step_x_didt_1, step_x_didt_2, step_x_didt_3;
wire [127:0] step_x_dwdt_1, step_x_dwdt_2, step_x_dwdt_3;

// Compute STEP x di_dt for each time point
assign step_x_didt_1 = (STEP * didt_1) >> (STEP_EXPONET);    // STEP(10 ns) x current  
assign step_x_didt_2 = (STEP * didt_2) >> (STEP_EXPONET);
assign step_x_didt_3 = (STEP * didt_3) >> (STEP_EXPONET);

// Compute STEP x dw_dt for each time point
assign step_x_dwdt_1 = (STEP * dwdt_1) >> (STEP_EXPONET);    // STEP(10 ns) x speed
assign step_x_dwdt_2 = (STEP * dwdt_2) >> (STEP_EXPONET);
assign step_x_dwdt_3 = (STEP * dwdt_3) >> (STEP_EXPONET);

// This clk is two times faster than the STEP time (if the STEP time is 10 ns, then the clk is 5ns)
always_ff @(posedge clk, negedge rst_n) begin
    if (!rst_n) begin
        voltage_0 <= 64'h0000;
        voltage_1 <= 64'h0000;
    end else begin
        voltage_0 <= voltage_1;    // voltage_0 becomes voltage at the (tn-h)
        voltage_1 <= voltage;      // voltage_1 becomes voltage at the (tn-h/2), voltage gets the updated voltage at present
    end
end

// This clk is two times faster than the STEP time (if the STEP time is 2 ns, then the clk is 1ns)
always_ff @(posedge clk, negedge rst_n) begin
    if (!rst_n) begin
        load_0 <= 64'h0000;
        load_1 <= 64'h0000;
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

// Increment i
always @(posedge clk, negedge rst_n)
    if (!rst_n)
        i <= 64'h0000;
    else if (tmr)
        i <= i + i_slope ;

// Increment w
always @(posedge clk, negedge rst_n)
    if (!rst_n)
        w <= 64'h0000;
    else if (tmr)
        w <= w + w_slope ;

// integer dy_dt modules
dy_dt_int dy_dt_1(.v(voltage_0), .load(load_0), .i(i), .w(w), .dw_dt(dwdt_1), .di_dt(didt_1));
dy_dt_int dy_dt_2(.v(voltage_1), .load(load_1),.i(i+{1'b0,step_x_didt_1[63:1]}), .w(w+{1'b0,step_x_dwdt_1[63:1]}), .dw_dt(dwdt_2), .di_dt(didt_2));
dy_dt_int dy_dt_3(.v(voltage_1), .load(load_1),.i(i+{1'b0,step_x_didt_2[63:1]}), .w(w+{1'b0,step_x_dwdt_2[63:1]}), .dw_dt(dwdt_3), .di_dt(didt_3));
dy_dt_int dy_dt_4(.v(voltage), .load(load),.i(i+step_x_didt_3), .w(w+step_x_dwdt_3), .dw_dt(dwdt_4), .di_dt(didt_4));

// incremental values for i/w
assign i_slope = (h_div_6 * (didt_1 + {didt_2[126:0], 1'b0} + {didt_3[126:0], 1'b0} + didt_4))>> STEP_EXPONET;
assign w_slope = (h_div_6 * (dwdt_1 + {dwdt_2[126:0], 1'b0} + {dwdt_3[126:0], 1'b0} + dwdt_4))>> STEP_EXPONET;
endmodule