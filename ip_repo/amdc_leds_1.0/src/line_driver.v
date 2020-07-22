`timescale 1ns / 1ps

// =============================
//
// Submodule used to drive logic "1", "0", and "RESET" signals
// to data line of IN-PI556FCH.
//
// Nathan Petersen, March 13, 2020
//
// =============================
//
// Note: clk expected at 200MHz
//
// =============================

// "code" definition:
// 0: logic "0"
// 1: logic "1"

// Assert "start" (active high) to start sending code on line. Once started,
// "done" will go logic LOW when line is being driven to some code,
// and back to HIGH when line is free (i.e. code is sent).

// Cycles for 0L / 0H, etc
// NOTE: clock is at 200MHz
`define CYCLES_0H	(32'd60)  // 0.3us at 5ns clk = 300 / 5 = 60
`define CYCLES_0L	(32'd180) // 0.9us at 5ns clk = 900 / 5 = 180
`define CYCLES_1H	(32'd120) // 0.6us at 5ns clk = 600 / 5 = 120
`define CYCLES_1L	(32'd120) // 0.6us at 5ns clk = 600 / 5 = 120

// State Machine states
`define SM_IDLE		(3'h0)
`define SM_PARSE	(3'h1)
`define SM_0H		(3'h2)
`define SM_1H		(3'h3)
`define SM_0L		(3'h4)
`define SM_1L		(3'h5)

module line_driver(clk, rst_n, code, start, done, dout);

input clk, rst_n;
input start;
input code;

output wire done;
output wire dout;

// State machine below is used to control "line_dout"
reg line_dout;
assign dout = line_dout;


// =============
// Set / Reset FF
// for "done"
// =============

reg set_done;
reg clr_done;
reg my_done;
assign done = my_done;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		my_done <= 1;
	else if (set_done)
		my_done <= 1;
	else if (clr_done)
		my_done <= 0;
end


// =============
// Counter
// =============

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



// =============
// State Machine
// =============

// Infer SM state registers
reg [2:0] state;
reg [2:0] next_state;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		state <= `SM_IDLE;
	else
		state <= next_state;
end

always @(*) begin
	// Set default outputs:
	next_state = state;
	
	reset_sm_delay_counter = 0;
	set_done = 0;
	clr_done = 0;
	
	line_dout = 0;	
	
	case (state)
		`SM_IDLE: begin
			if (start == 1'b1) begin
				clr_done = 1;
				next_state = `SM_PARSE;
			end
		end
		
		`SM_PARSE: begin
			reset_sm_delay_counter = 1;
		
			if (code == 1'b0) begin
				next_state = `SM_0H;
			end
			
			else if (code == 1'b1) begin
				next_state = `SM_1H;
			end
			
			else begin
				next_state = `SM_IDLE;	
			end
		end
		
		`SM_0H: begin
			line_dout = 1;
			
			if (sm_delay_counter >= `CYCLES_0H) begin
				reset_sm_delay_counter = 1;
				next_state = `SM_0L;
			end
		end
		
		`SM_1H: begin
			line_dout = 1;
			
			if (sm_delay_counter >= `CYCLES_1H) begin
				reset_sm_delay_counter = 1;
				next_state = `SM_1L;
			end
		end
		
		`SM_0L: begin
			line_dout = 0;
			
			if (sm_delay_counter >= `CYCLES_0L) begin
				set_done = 1;
				next_state = `SM_IDLE;
			end
		end
		
		`SM_1L: begin
			line_dout = 0;
			
			if (sm_delay_counter >= `CYCLES_1L) begin
				set_done = 1;
				next_state = `SM_IDLE;
			end
		end
	endcase
end


endmodule