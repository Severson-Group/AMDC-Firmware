`timescale 1ns / 1ps

// *******
//
// DAC Interface Driver for Texas Instruments DAC128S085 Chip
//  
// Nathan Petersen, May 24, 2019
//
// *******
//
// Notes:
// - Signal clk expected at 200MHz
// - IC expects 16 bits per channel, even though DAC is 12 bits
// - Continually send data bits in loop:
//   DAC1 -> DAC2 -> ... -> DAC8 -> DAC1 -> ...
//
// *******

`define SM_LOAD_DAC1		(4'h0)
`define SM_LOAD_DAC2		(4'h1)
`define SM_LOAD_DAC3		(4'h2)
`define SM_LOAD_DAC4		(4'h3)
`define SM_LOAD_DAC5		(4'h4)
`define SM_LOAD_DAC6		(4'h5)
`define SM_LOAD_DAC7		(4'h6)
`define SM_LOAD_DAC8		(4'h7)
`define SM_SERIAL_START		(4'h8)
`define SM_SERIAL_WAIT		(4'h9)
`define SM_SAMPLE_DELAY		(4'hA)
`define SM_SET_WTM_MODE		(4'hB)

module drv_dac128s085(clk, rst_n, dac1, dac2, dac3, dac4, dac5, dac6, dac7, dac8, sclk_div, sample_delay, SYNC, SCLK, DIN);

input clk, rst_n;

input [31:0] sclk_div;
input [31:0] sample_delay;

input [11:0] dac1;		// value on DAC1 (0V to 4.096V)
input [11:0] dac2;		// value on DAC2 (0V to 4.096V)
input [11:0] dac3;		// value on DAC3 (0V to 4.096V)
input [11:0] dac4;		// value on DAC4 (0V to 4.096V)
input [11:0] dac5;		// value on DAC5 (0V to 4.096V)
input [11:0] dac6;		// value on DAC6 (0V to 4.096V)
input [11:0] dac7;		// value on DAC7 (0V to 4.096V)
input [11:0] dac8;		// value on DAC8 (0V to 4.096V)

output wire SYNC;		// SYNC signal going to DAC IC
output wire SCLK;		// SCLK signal going to DAC IC
output wire DIN;		// DIN  signal going to DAC IC

// Signals for serial line driver
reg serial_start;
wire serial_done;
reg [15:0] serial_data_out;

// Instantiate serial line driver
serial_16b serial(
	.clk(clk),
	.rst_n(rst_n),
	.data(serial_data_out),
	.start(serial_start),
	.done(serial_done),
	.sclk_div(sclk_div),
	.SYNC(SYNC),
	.SCLK(SCLK),
	.DIN(DIN)
);

// *************************
// *************************
//      Serial Data Out
// *************************
// *************************

reg load_serial_data_out;
reg [15:0] data_out;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		serial_data_out <= 16'b0;
	else if (load_serial_data_out)
		serial_data_out <= data_out;
end

// *************************
// *************************
//       State Machine
//       Delay Counter
// *************************
// *************************

reg [31:0] sm_delay_counter;
reg reset_sm_delay_counter;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		sm_delay_counter <= 32'b0;
	else if (reset_sm_delay_counter)
		sm_delay_counter <= 32'b0;
	else
		sm_delay_counter <= sm_delay_counter + 32'd1;
end 


// *****************************
// *****************************
//         State Machine
// *****************************
// *****************************

reg load_state_after_transmission;
reg [3:0] state_after_transmission;
reg [4:0] my_state_after_trans;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		state_after_transmission <= 4'b0;
	else if (load_state_after_transmission)
		state_after_transmission <= my_state_after_trans;
end

// Infer SM state registers
reg [3:0] state;
reg [3:0] next_state;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		state <= `SM_SET_WTM_MODE;
	else
		state <= next_state;
end

always @(*) begin
	// Set default outputs:
	next_state = state;
	reset_sm_delay_counter = 0;
	serial_start = 0;
	load_serial_data_out = 0;
	load_state_after_transmission = 0;
	data_out = 16'b0;
	
	case (state)
		`SM_LOAD_DAC1: begin
			load_serial_data_out = 1;
			data_out = {1'b0, 3'd0, dac1};
			
			my_state_after_trans = `SM_LOAD_DAC2;
			load_state_after_transmission = 1;
			
			next_state = `SM_SERIAL_START;
		end
		
		`SM_LOAD_DAC2: begin
			load_serial_data_out = 1;
			data_out = {1'b0, 3'd1, dac2};

			my_state_after_trans = `SM_LOAD_DAC3;
			load_state_after_transmission = 1;
			
			next_state = `SM_SERIAL_START;
		end
		
		`SM_LOAD_DAC3: begin
			load_serial_data_out = 1;
			data_out = {1'b0, 3'd2, dac3};
			
			my_state_after_trans = `SM_LOAD_DAC4;
			load_state_after_transmission = 1;
			
			next_state = `SM_SERIAL_START;
		end
		
		`SM_LOAD_DAC4: begin
			load_serial_data_out = 1;
			data_out = {1'b0, 3'd3, dac4};
			
			my_state_after_trans = `SM_LOAD_DAC5;
			load_state_after_transmission = 1;
			
			next_state = `SM_SERIAL_START;
		end
		
		`SM_LOAD_DAC5: begin
			load_serial_data_out = 1;
			data_out = {1'b0, 3'd4, dac5};
			
			my_state_after_trans = `SM_LOAD_DAC6;
			load_state_after_transmission = 1;
			
			next_state = `SM_SERIAL_START;
		end
		
		`SM_LOAD_DAC6: begin
			load_serial_data_out = 1;
			data_out = {1'b0, 3'd5, dac6};
			
			my_state_after_trans = `SM_LOAD_DAC7;
			load_state_after_transmission = 1;
			
			next_state = `SM_SERIAL_START;
		end
		
		`SM_LOAD_DAC7: begin
			load_serial_data_out = 1;
			data_out = {1'b0, 3'd6, dac7};
			
			my_state_after_trans = `SM_LOAD_DAC8;
			load_state_after_transmission = 1;
			
			next_state = `SM_SERIAL_START;
		end
		
		`SM_LOAD_DAC8: begin
			load_serial_data_out = 1;
			data_out = {1'b0, 3'd7, dac8};
			
			my_state_after_trans = `SM_LOAD_DAC1;
			load_state_after_transmission = 1;
			
			next_state = `SM_SERIAL_START;
		end
		
		
		
		`SM_SERIAL_START: begin
			serial_start = 1;
			next_state = `SM_SERIAL_WAIT;
		end
		
		`SM_SERIAL_WAIT: begin
			if (serial_done) begin
				reset_sm_delay_counter = 1;
				next_state = `SM_SAMPLE_DELAY;
			end
		end
		
		`SM_SAMPLE_DELAY: begin
			if (sm_delay_counter >= sample_delay)
				next_state = state_after_transmission;
		end
		
		`SM_SET_WTM_MODE: begin
			load_serial_data_out = 1;
			data_out = {4'b1001, 12'b0};
			
			my_state_after_trans = `SM_LOAD_DAC1;
			load_state_after_transmission = 1;
			
			next_state = `SM_SERIAL_START;
		end
	endcase
end

endmodule
