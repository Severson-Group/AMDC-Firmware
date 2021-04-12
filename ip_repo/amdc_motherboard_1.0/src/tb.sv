`timescale 1ns / 1ps

module tb();

reg clk, rst_n;
reg start_rx;
reg din;

wire is_dout_valid;
wire [15:0] adc_dout0;
wire [15:0] adc_dout1;
wire [15:0] adc_dout2;
wire [15:0] adc_dout3;
wire [15:0] counter_data_valid;
wire [15:0] counter_data_corrupt;
wire [15:0] counter_data_timeout;

initial begin
	// Set the inputs
	start_rx = 0;
	din = 1;

	clk	= 1'b0;
	rst_n = 1'b1;
	
	@(posedge clk);
	@(posedge clk);
	
	rst_n = 1'b0;
	@(posedge clk);
	@(negedge clk);
	rst_n = 1'b1;
	
	@(posedge clk);
	@(posedge clk);
	
	
	repeat (10) begin
	
        // Add buffer of clocks
        repeat (10) @(posedge clk);
        
        start_rx = 1;
        @(posedge clk);
        start_rx = 0;
    
        // Wait a single clock to let "done" become LOW
        @(posedge clk);
        
        repeat (16) @(posedge clk);
            
        // PACKET 0
        uart_send(8'h90);
        uart_send(8'b10101010);
        uart_send(8'b01010101);
        
        // PACKET 1
        uart_send(8'h91);
        uart_send(8'h12);
        uart_send(8'h34);
        
        // PACKET 2
        uart_send(8'h92);
        uart_send(8'h45);
        uart_send(8'h67);
        
        // PACKET 3
        uart_send(8'h93);
        uart_send(8'h76);
        uart_send(8'h87);
        
        repeat (100) @(posedge clk);
	end
	
	$stop;
end

task uart_send;
    input [7:0] data;
    $display("%d\n", data);
    begin
        din = 0;
        repeat (8) @(posedge clk);
        din = data[0];
        repeat (8) @(posedge clk);
        din = data[1];
        repeat (8) @(posedge clk);
        din = data[2];
        repeat (8) @(posedge clk);
        din = data[3];
        repeat (8) @(posedge clk);
        din = data[4];
        repeat (8) @(posedge clk);
        din = data[5];
        repeat (8) @(posedge clk);
        din = data[6];
        repeat (8) @(posedge clk);
        din = data[7];
        repeat (8) @(posedge clk);
        din = ~^data[7:0];
        repeat (8) @(posedge clk);
        din = 1;
        repeat (8) @(posedge clk);
        din = 1;
        repeat (8) @(posedge clk);
	end
endtask

always
	#2.5 clk <= ~clk;

adc_uart_rx iDUT(
	.clk(clk),
	.rst_n(rst_n),
	.start_rx(start_rx),
	.din(din),
	.is_dout_valid(is_dout_valid),
	.adc_dout0(adc_dout0),
	.adc_dout1(adc_dout1),
	.adc_dout2(adc_dout2),
	.adc_dout3(adc_dout3),
    .counter_data_valid(counter_data_valid),
    .counter_data_corrupt(counter_data_corrupt),
    .counter_data_timeout(counter_data_timeout)
);

endmodule