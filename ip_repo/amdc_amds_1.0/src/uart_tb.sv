`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 06/22/2026 10:08:05 AM
// Design Name: 
// Module Name: uart_tb
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


module uart_tb();



reg [7:0] currently_broadcasting;
    reg examine_line_x = 0;
	
	reg clk = 0;
	always #1 clk = ~clk;
	reg rst_n = 1'b1;
	reg [1:0] uart_data = 2'b11;
	wire is_byte_valid;
	wire is_byte_corrupt;
	wire [8:0] rx_data;







    
    uart_rx bit_receiver(
    .clk(clk),
    .rst_n(rst_n),
    .din(uart_data[examine_line_x]),
    .is_byte_valid(is_byte_valid),
    .is_byte_corrupt(is_byte_corrupt),
    .dout(rx_data)
    );
    
    assign rx_data[8] = bit_receiver.shift_reg[8];
    
    initial begin 
    rst_n <= 1;
    #2;
    rst_n <= 0;
    #2;
    rst_n <= 1;
    #2;
    
    send_byte(8'b10101010, 0);
    
    $finish;
    end
    
    
    
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
    #18;
    // Test to make sure the data was received, and that the uart module
    // either saw it as valid, or invalid. If both are low, no data was seen.
    assert (is_byte_valid != is_byte_corrupt) else begin
        $error("Data cannot be both valid & invalid.");
        $stop;
    end
    // Ensure that the parity was validated correctly.
    assert (is_byte_corrupt == use_invalid_parity) else begin 
        $error("Valid data failed parity check or vice versa.");
        $stop;
    end
endtask 




endmodule
