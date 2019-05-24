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

`define DAC_CTRL_WTM (4'b1001)

module drv_dac128s085(clk, rst_n, dac1, dac2, dac3, dac4, dac5, dac6, dac7, dac8, SYNC, SCLK, DIN);

input clk, rst_n;

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
	.SYNC(SYNC),
	.SCLK(SCLK),
	.DIN(DIN)
);

// *************************
// *************************
//      Serial Data Out
// *************************
// *************************

wire [11:0] curr_dac;
reg load_serial_data_out;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		serial_data_out <= 16'b0;
	else if (load_serial_data_out)
		serial_data_out <= {`DAC_CTRL_WTM, curr_dac};
end

// *************************
// *************************
//       State Machine
//       Delay Counter
// *************************
// *************************

reg [15:0] sm_delay_counter;
reg reset_sm_delay_counter;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		sm_delay_counter <= 16'b0;
	else if (reset_sm_delay_counter)
		sm_delay_counter <= 16'b0;
	else
		sm_delay_counter <= sm_delay_counter + 16'd1;
end 


// *****************************
// *****************************
//         State Machine
// *****************************
// *****************************

`define SM_LOAD_DAC1	(4'h0)
`define SM_LOAD_DAC2	(4'h1)
`define SM_LOAD_DAC3	(4'h2)
`define SM_LOAD_DAC4	(4'h3)
`define SM_LOAD_DAC5	(4'h4)
`define SM_LOAD_DAC6	(4'h5)
`define SM_LOAD_DAC7	(4'h6)
`define SM_LOAD_DAC8	(4'h7)
`define SM_SERIAL_START	(4'h8)
`define SM_SERIAL_WAIT	(4'h9)
`define SM_DELAY		(4'hA)


// Infer state registers
reg [3:0] state;
reg [3:0] next_state;

reg [3:0] state_after_delay;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n) begin
		state <= `SM_DELAY;
		state_after_delay <= `SM_LOAD_DAC1;
	end else
		state <= next_state;
end

always @(*) begin
	// Set default outputs:
	next_state = state;
	reset_sm_delay_counter = 0;
	serial_start = 0;
	load_serial_data_out = 0;
	
	case (state)
		`SM_LOAD_DAC1: begin
			load_serial_data_out = 1;
			state_after_delay = `SM_LOAD_DAC2;
			next_state = `SM_SERIAL_START;
		end
		
		`SM_LOAD_DAC2: begin
			load_serial_data_out = 1;
			state_after_delay = `SM_LOAD_DAC3;
			next_state = `SM_SERIAL_START;
		end
		
		`SM_LOAD_DAC3: begin
			load_serial_data_out = 1;
			state_after_delay = `SM_LOAD_DAC4;
			next_state = `SM_SERIAL_START;
		end
		
		`SM_LOAD_DAC4: begin
			load_serial_data_out = 1;
			state_after_delay = `SM_LOAD_DAC5;
			next_state = `SM_SERIAL_START;
		end
		
		`SM_LOAD_DAC5: begin
			load_serial_data_out = 1;
			state_after_delay = `SM_LOAD_DAC6;
			next_state = `SM_SERIAL_START;
		end
		
		`SM_LOAD_DAC6: begin
			load_serial_data_out = 1;
			state_after_delay = `SM_LOAD_DAC7;
			next_state = `SM_SERIAL_START;
		end
		
		`SM_LOAD_DAC7: begin
			load_serial_data_out = 1;
			state_after_delay = `SM_LOAD_DAC8;
			next_state = `SM_SERIAL_START;
		end
		
		`SM_LOAD_DAC8: begin
			load_serial_data_out = 1;
			state_after_delay = `SM_LOAD_DAC1;
			next_state = `SM_SERIAL_START;
		end
		
		
		
		`SM_SERIAL_START: begin
			serial_start = 1;
			next_state = `SM_SERIAL_WAIT;		
		end
		
		`SM_SERIAL_WAIT: begin
			if (serial_done) begin
				reset_sm_delay_counter = 1;
				next_state = `SM_DELAY;
			end
		end
		
		`SM_DELAY: begin
			if (sm_delay_counter >= 16'd100) begin
				next_state = state_after_delay;
			end
		end
	endcase
end

assign curr_dac = (state == `SM_LOAD_DAC1) ? dac1 :
					(state == `SM_LOAD_DAC2) ? dac2 :
					(state == `SM_LOAD_DAC3) ? dac3 :
					(state == `SM_LOAD_DAC4) ? dac4 :
					(state == `SM_LOAD_DAC5) ? dac5 :
					(state == `SM_LOAD_DAC6) ? dac6 :
					(state == `SM_LOAD_DAC7) ? dac7 : dac8;

endmodule
