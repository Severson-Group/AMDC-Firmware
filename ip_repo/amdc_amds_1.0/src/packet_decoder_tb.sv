`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 06/22/2026 10:53:06 AM
// Design Name: 
// Module Name: packet_decoder_tb
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module packet_decoder_tb();


    reg [7:0] currently_broadcasting;
    reg examine_line_x = 0;
	
	reg clk = 0;
	always #1 clk = ~clk;
	reg rst_n = 1'b1;
	reg [1:0] uart_data = 2'b11;
	reg [11:0] is_dout_enabled = 12'b111111111111;
	wire adc_uart_done;
	wire assert_done;
	reg start_rx = 0;
	 wire[15:0] adc_data[11:0];
   /* input wire clk,
    input wire rst_n,
    
    input wire start_rx,
    
    // The data line coming from the AMDS
    input wire din,
    input wire [11:0] is_dout_enabled,
    
    output reg [11:0] is_dout_valid, // is_dout_valid[0] == 1 implies that adc_dout0 is valid
    output reg adc_uart_done,
    output reg assert_done,
    output wire [15:0] adc_dout0,
    output wire [15:0] adc_dout1,
    output wire [15:0] adc_dout2,
    output wire [15:0] adc_dout3,
    output wire [15:0] adc_dout4,
    output wire [15:0] adc_dout5,
    output wire [15:0] adc_dout6,
    output wire [15:0] adc_dout7,
    output wire [15:0] adc_dout8,
    output wire [15:0] adc_dout9,
    output wire [15:0] adc_dout10,
    output wire [15:0] adc_dout11,
    
    output reg [15:0] counter_bytes_valid,
    output reg [15:0] counter_bytes_corrupt,
    output reg [15:0] counter_bytes_timed_out
    */
    adc_uart_rx packet_decoder(
        .rst_n(rst_n),
        .clk(clk),
        .din(uart_data[examine_line_x]),
        .is_dout_enabled(is_dout_enabled),
        .adc_uart_done(adc_uart_done),
        .assert_done(assert_done),
        .start_rx(start_rx)
    );
    assign adc_data = packet_decoder.adc_data;
    initial begin 
    rst_n <= 1;
    #2;
    rst_n <= 0;
    #2;
    rst_n <= 1;
    #2;
    
    auto_start_rx();
    send_byte(8'b10101010, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b10101010, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b10101010, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b10101010, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b10101010, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b10101010, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b10101010, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b10101010, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b10101010, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b10101010, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b10101010, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b10101010, 0);
    send_byte(8'b11111111, 0);
    
    // TRANSMISSION COMPLETE
    #20;
    auto_start_rx();
    send_byte(8'b11111111, 0);
    
    send_byte(8'b10101010, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b10101010, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b10101010, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b10101010, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b10101010, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b10101010, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b10101010, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b10101010, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b10101010, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b10101010, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b10101010, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b10101010, 0);
    
    $finish;
    end
    
    task automatic auto_start_rx(); 
        #2;
        start_rx <= 1;
        #2;
        start_rx <= 0;
    endtask
    
    task automatic send_byte(input reg[7:0] data, input bit use_invalid_parity);
    currently_broadcasting <= data;
    // start bit
    uart_data[examine_line_x] <= 1'b0;
    #18;
    // data bits
    for(int i = 0; i < 8; i = i + 1) begin
        uart_data[examine_line_x] <= data[i];
        #18;
    end
    // parity bit (odd parity)
    uart_data[examine_line_x] <= (!(^data)) ^ use_invalid_parity;
    #18;
    // end transmission.
    uart_data[examine_line_x] <= 1'b1;
    #2;
endtask 




endmodule

