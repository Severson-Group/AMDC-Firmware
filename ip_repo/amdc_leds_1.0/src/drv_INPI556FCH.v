`timescale 1ns / 1ps

// =============================
//
// Serial LED Driver
// 
// Designed for IN-PI556FCH
// http://www.inolux-corp.com/datasheet/SMDLED/Addressable%20LED/IN-PI556FCH.pdf
//  
// Nathan Petersen, March 13, 2020
//
// =============================
//
// Note: clk expected at 200MHz
//
// =============================

// State Machine states
`define SM_IDLE		(3'h0)
`define SM_START	(3'h1)
`define SM_WAIT		(3'h2)

module drv_INPI1556FCH(clk, rst_n, color, start, dout, done);

input clk, rst_n;

// Holds requested color of LED
input [23:0] color;

// Assert HIGH to trigger transmission
input start;

// Raw signal to LED devices
output wire dout;

// HIGH when line is idle
output wire done;



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


// =========
// Shift Reg
// =========

reg [23:0] shift_reg;
reg shift_reg_shift;
reg shift_reg_load;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		shift_reg <= 24'b0;
	else if (shift_reg_load)
		shift_reg <= color[23:0];
	else if (shift_reg_shift)
		shift_reg <= {shift_reg[22:0], 1'b0};
end

wire code;
assign code = shift_reg[23]; // MSB of color


// ===========
// Bit Counter
// ===========

reg [4:0] counter;
reg inc_counter;
reg rst_counter;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		counter <= 5'b0;
	else if (rst_counter)
		counter <= 5'b0;
	else if (inc_counter)
		counter <= counter + 5'd1;
	else
		counter <= counter;
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


reg linedrv_start;
wire linedrv_done;

always @(*) begin
	// Set default outputs:
	next_state = state;
	
	shift_reg_shift = 0;
	shift_reg_load = 0;
	inc_counter = 0;
	rst_counter = 0;
	set_done = 0;
	clr_done = 0;
	linedrv_start = 0;
		
	case (state)
		`SM_IDLE: begin
			if (start == 1'b1) begin
				clr_done = 1;
				shift_reg_load = 1;
				rst_counter = 1;
				next_state = `SM_START;
			end
		end
		
		`SM_START: begin
			linedrv_start = 1;
			next_state = `SM_WAIT;
		end
		
		`SM_WAIT: begin
			if (linedrv_done) begin
				if (counter >= 5'd23) begin
					set_done = 1;
					next_state = `SM_IDLE;	
				end
				
				else begin
					shift_reg_shift = 1;
					inc_counter = 1;
					next_state = `SM_START;
				end
			end
		end
	endcase
end


// ===========
// Line Driver
// ===========

line_driver linedrv(
	.clk(clk),
	.rst_n(rst_n),
	.code(code),
	.start(linedrv_start),
	.done(linedrv_done),
	.dout(dout)
);

endmodule