`timescale 1 ns / 1 ps

module uart_rx(
	input wire clk,
	input wire rst_n,
	
	// UART data input (not in our clock domain!)
	// 
	// UART settings
	// > Bit Rate (MBit/s): 25
	// > Bits per Frame:    8
	// > Stop bits:         2
	// > Parity bit:        Odd
	// > Significant bit:   LSB first
	input wire din,
	
	// Flag which indicates when to start looking for new UART data
	// i.e. this should set some time before the start bit occurs
	input wire start_rx,
	
	// Asserted when we saw a start bit and have receieved all data
	output reg is_byte_valid,

	// Asserted when we saw a start bit and received all data, but failed the parity check
	output reg is_byte_corrupt,
	
	// Asserted when the user has started a rx, but we never saw a start bit!
	output reg byte_timed_out,
	
	// Holds the contents of what we received over the UART line
	output wire [7:0] dout
);

// ==============
// "din" has already been double-flopped for meta-stability in the AXI driver file!
//
// HOWEVER... all of the state machines above this UART receiver add in several clock
// cycles of delay. Without extra flopping of the data line in this module, the start bit
// will fall several clock cyles before this module's SM is in the WAIT_START_BIT_STATE
// Therefore, we will add some extra flopping in this module to line things up again
// 
// NOTE: The last ff is used to detect falling edges
// ==============

reg [4:0] din_flopper;

always @(posedge clk, negedge rst_n) begin
	if (~rst_n)
		din_flopper <= 1'b0;
	else
		din_flopper <= {din_flopper[3:0], din};
end

// ===========================
// Falling Edge Detector (din)
// ===========================

wire din_fall;
assign din_fall = (~din_flopper[3] & din_flopper[4]);

// =================
// UART RX Shift Reg
// =================

reg [8:0] shift_reg;
reg shift_reg_shift;
reg reset_reg_shift;

always @(posedge clk, negedge rst_n) begin
	if (~rst_n)
		shift_reg <= 9'b0;
	else if (reset_reg_shift)
		shift_reg <= 9'b0;
	else if (shift_reg_shift)
		shift_reg <= {din_flopper[3], shift_reg[8:1]};
end

// Extract only the data bits from the shift reg
// The shift reg also holds the parity bit!
assign dout[7:0] = shift_reg[7:0];

// =======================
// S/R flop: is_byte_valid
// =======================

reg deassert_is_byte_valid;
reg assert_is_byte_valid;

always @(posedge clk, negedge rst_n) begin
	if (~rst_n)
		is_byte_valid <= 1'b0;
	else if (deassert_is_byte_valid)
		is_byte_valid <= 1'b0;
	else if (assert_is_byte_valid)
		is_byte_valid <= 1'b1;
	else
		is_byte_valid <= is_byte_valid;
end

// =======================
// S/R flop: is_byte_corrupt
// =======================

reg deassert_is_byte_corrupt;
reg assert_is_byte_corrupt;

always @(posedge clk, negedge rst_n) begin
	if (~rst_n)
		is_byte_corrupt <= 1'b0;
	else if (deassert_is_byte_corrupt)
		is_byte_corrupt <= 1'b0;
	else if (assert_is_byte_corrupt)
		is_byte_corrupt <= 1'b1;
	else
		is_byte_corrupt <= is_byte_corrupt;
end

// =======================
// S/R flop: byte_timed_out
// =======================

reg deassert_byte_timed_out;
reg assert_byte_timed_out;

always @(posedge clk, negedge rst_n) begin
	if (~rst_n)
		byte_timed_out <= 1'b0;
	else if (deassert_byte_timed_out)
		byte_timed_out <= 1'b0;
	else if (assert_byte_timed_out)
		byte_timed_out <= 1'b1;
	else
		byte_timed_out <= byte_timed_out;
end

// ==============
// Baud Period Timer
// ==============

reg [3:0] baud_timer;
reg rst_baud_timer;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		baud_timer <= 4'b0;
	else if (rst_baud_timer)
		baud_timer <= 4'b0;
	else
		baud_timer <= baud_timer + 4'd1;
end

// ===========
// Bit counter
// ===========

reg [3:0] bit_counter;
reg inc_bit_counter;
reg rst_bit_counter;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		bit_counter <= 4'b0;
	else if (rst_bit_counter)
		bit_counter <= 4'b0;
	else if (inc_bit_counter)
		bit_counter <= bit_counter + 4'd1;
	else
		bit_counter <= bit_counter;
end

// ==================
// Byte Timeout Timer
// ==================

// Wait for a max of 2.5us for the start bit
// after adc_uart_rx tells us to expect it
//
// 2.5us = 2500ns = 500 clock cycles
//
// Let's have max of 512, so 9 bit.

reg [8:0] byte_timeout_timer;
reg reset_byte_timeout_timer;
always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		byte_timeout_timer <= 9'b0;
	else if (reset_byte_timeout_timer)
		byte_timeout_timer <= 9'b0;
	else
		byte_timeout_timer <= byte_timeout_timer + 1;
end

// Detect when timer = max value (i.e., about 2.5us)
wire max_byte_timeout_timer;
assign max_byte_timeout_timer = &byte_timeout_timer;

// =============
// State Machine
// =============

`define SM_IDLE           (3'h0)
`define SM_WAIT_START_BIT (3'h1)
`define SM_WAIT_HALF_BAUD (3'h2)
`define SM_WAIT_FULL_BAUD (3'h3)
`define SM_END_OF_RX      (3'h4)

reg [2:0] state;
reg [2:0] next_state;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		state <= `SM_IDLE;
	else
		state <= next_state;
end

always @(*) begin
	// Set default outputs
	next_state = state;
	
	reset_reg_shift = 0;
	shift_reg_shift = 0;
	inc_bit_counter = 0;
	rst_bit_counter = 0;
	rst_baud_timer = 0;
	reset_byte_timeout_timer = 0;
	
	deassert_is_byte_valid = 0;
	assert_is_byte_valid = 0;
	deassert_byte_timed_out = 0;
	assert_byte_timed_out = 0;
	deassert_is_byte_corrupt = 0;
	assert_is_byte_corrupt = 0;
	
	case (state)
		`SM_IDLE: begin
			if (start_rx) begin
			    reset_reg_shift = 1;
				rst_bit_counter = 1;
				reset_byte_timeout_timer = 1;
				deassert_is_byte_valid = 1;
				deassert_byte_timed_out = 1;
				deassert_is_byte_corrupt = 1;
				next_state = `SM_WAIT_START_BIT;
			end
		end	
	
		`SM_WAIT_START_BIT: begin
			if (din_fall) begin
				// Falling DIN means start-bit, here comes a packet!				
				rst_baud_timer = 1;
				next_state = `SM_WAIT_HALF_BAUD;
			end
			
			else if (max_byte_timeout_timer) begin
				// TIMEOUT! Abort...
				next_state = `SM_IDLE;
				assert_byte_timed_out = 1;
			end
		end
		
		`SM_WAIT_HALF_BAUD: begin
			// Wait for half of a baud period
			//
			// Means timer gets to timer_max = (((1 / baud_rate)/2) * 1e9 / clk_ns)
			// 
			// If clk_ns = 5ns and baud_rate 25M,
			// then timer_max = 4 cycles			
			//
			// Because we detected the DIN falling edge 1 clock cycle late, wait 1 less cycle
			// 4 - 1 = 3 cycle
			if (baud_timer >= 4'd3) begin
				rst_baud_timer = 1;
				next_state = `SM_WAIT_FULL_BAUD;
			end
		end

		`SM_WAIT_FULL_BAUD: begin
			// Wait for full baud period
			//
			// In the half baud wait state, we accounted for the delay
			// because of our flip flopping of DIN, so here we want 
			// to wait for EXACTLY one baud period.
			//
			// timer_max = ((1 / baud_rate) * 1e9 / clk_ns)
			//           = 8 cycles
			if (baud_timer >= 4'd7) begin
				shift_reg_shift = 1;
				rst_baud_timer = 1;
				inc_bit_counter = 1;
								
				if (bit_counter >= 4'd8) begin
					next_state = `SM_END_OF_RX;
				end
			end
		end
		
		`SM_END_OF_RX: begin
			if (/*ODD parity:*/^shift_reg[8:0]) begin
				// Packet looks valid!
				assert_is_byte_valid = 1;
			end
			else begin
				// Data is corrupt!
				assert_is_byte_corrupt = 1;
			end
			
			next_state = `SM_IDLE;
		end
	endcase
end

endmodule