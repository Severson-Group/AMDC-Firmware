`timescale 1ns / 1ns

module solver_rk4_tb_int();

reg clk,rst_n;
reg [63:0] voltage, load;
wire [63:0] current, speed;
//////////////////////
// Instantiate CPU //
////////////////////
solver_rk4_int iDUT(.clk(clk),.rst_n(rst_n), .voltage(voltage), .load(load), .i(current), .w(speed));
initial begin
    clk = 0;
    rst_n = 0;
    #2 rst_n = 1;
	voltage = 64'h0;
	load = 0;
    repeat(100)@(posedge clk);

	voltage = 64'hFFFFFFFFFFFFFFF;
	//repeat(100000000)@(posedge clk);
	//voltage = 64'h0;
end

always
    #5 clk = ~clk;

endmodule
