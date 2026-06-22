`timescale 1 ns / 1 ps

// Author: Harley Peterson
// Date: 5/28/2026
// This file READS individual BYTES from UART, then exposes them as an 8-bit register.

module uart_rx(
	input wire clk,
	input wire rst_n,
	
	// UART data input (not in our clock domain!)
	// 
	// UART settings
	// > Bit Rate (MBit/s): 20
	// > Bits per Frame:    8
	// > Stop bits:         2
	// > Parity bit:        Odd
	// > Significant bit:   LSB first
	input wire din,
	
	// Asserted when we saw a start bit and have receieved all data and it's valid
	output reg is_byte_valid,

	// Asserted when we saw a start bit and received all data, but failed the parity check
	output reg is_byte_corrupt,
	
	// Holds the contents of what we received over the UART line
	output wire [7:0] dout
);

/*

First, identify the start bit.
This is easy, always @ falling edge,
UNLESS currently receiving data.

Then, wait 5 clock cycles to offset read location for accuracy.
Wait another 10 clock cycles to fast foward to the first data bit.

All of our packets are 9 bits long, 8 data 1 parity.
On every tenth cycle, shift data into buffer.

After 9 bits are read, check for parity. Timeouts don't exist anymore.

*/

// flag to prevent logic execution until start bit has been seen.
reg currently_reading_data;
// The baud timer counts down until the next bit.
reg [3:0] baud_timer;
// This register tells the code to reset the countdown.
wire reset_baud_timer;
// This register tells the code to start a 1.5 baud countdown
reg initial_baud_timer;
// The baud clock is a clock signal operating at the baud rate.
wire baud_clock;
// the internal clock only runs when we are receiving data.
wire internal_clock;
// RX'd data + parity bit.
reg [8:0] shift_reg;
// This counts to 9, one increment for each bit read.
reg [3:0] counter_bits_recieved;
// Signals that we have received 9 bits and should finalise the data.
wire transmission_complete;
// Always contains the current validity of the shift register.
// HIGH is VALID.
wire shift_register_validity;




assign internal_clock = currently_reading_data & clk;
// 10->6 (inclusive) are baud_clock HIGH
// 5 ->1 (inclusive) are baud_clock LOW
assign baud_clock = (baud_timer > 4'd5) & currently_reading_data;
// Extract only the data bits from the shift reg
// The shift reg also holds the parity bit!
assign dout[7:0] = shift_reg[7:0];
// 10 bits in a (byte + parity + start bit), transmission complete when counter == 10.
assign transmission_complete = counter_bits_recieved == 4'd10;
// Odd parity, so XOR TRUE.
assign shift_register_validity = ^shift_reg[8:0];


always @(negedge rst_n, posedge clk) begin
    initial_baud_timer <= 1'b0;
    if (!rst_n) begin
        currently_reading_data <= 1'b0;
        is_byte_corrupt <= 1'b0;
        is_byte_valid <= 1'b0;
    end else if (transmission_complete) begin
        is_byte_valid <= shift_register_validity;
        is_byte_corrupt <= !shift_register_validity;
    end else begin
    // Do nothing if we are already within a transmission.
    if ((!currently_reading_data) & (!din)) begin
        initial_baud_timer <= 1'b1;
        currently_reading_data <= 1'b1;
        // Reset validity/corrupt flags.
        is_byte_corrupt <= 1'b0;
        is_byte_valid <= 1'b0;
    end
    end
end

// Continously count down while receiving data.
always @(posedge internal_clock, negedge rst_n, posedge initial_baud_timer) begin
    if (!rst_n)
        baud_timer <= 4'b0000;
    else if (initial_baud_timer)
        // 15 = 5 clock cycle delay for data integrity
        //    + 9 clock cycle delay due to start bit.
        baud_timer <= 4'd12;
    else if (baud_timer > 1) begin
        baud_timer <= baud_timer - 3'd1;
    end else begin
        baud_timer <= 4'd9;
    end
end
 
// Read data from the input.
always @(posedge baud_clock, negedge rst_n) begin
    if (!rst_n) begin
        shift_reg <= 9'b000000000;
        counter_bits_recieved <= 4'b0000;
    end else if (baud_clock) begin
        shift_reg <= {din, shift_reg[8:1]};
        counter_bits_recieved = counter_bits_recieved + 4'd1;
    end 
    
end

endmodule