`timescale 1ns / 1ns

module solver_rk4_tb_fp();

reg clk,rst_n;
reg [31:0] voltage, load;
//////////////////////
// Instantiate CPU //
////////////////////
solver_rk4_fp iDUT(.clk(clk),.rst_n(rst_n), .voltage(voltage), .load(load));
initial begin
    clk = 0;
    rst_n = 0;
    #2 rst_n = 1;
	voltage = 32'h0;
	load = 0;
    repeat(100)@(posedge clk);

	voltage = 32'h41c00000; //24V
	repeat(100000000)@(posedge clk);
	//voltage = 64'h0;
end

always
    #5 clk = ~clk;

endmodule