`timescale 1ns / 1ps

// Encoder Interface
//
// Nathan Petersen, 11/29/2018
//
// Given encoder signals A and B
// this module will count rotational steps
// and sum them into a binary counter register.
//
// Z is used to provide single revolution position via `position` output
// `position` ranges between 0 and (2 ^ pulses_per_rev_bits) - 1
//
module encoder(clk, rst_n, A, B, Z, counter, position, pulses_per_rev_bits);

input A, B, Z;
input clk;
input rst_n;

input [31:0] pulses_per_rev_bits;

output wire [31:0] counter;
output wire [31:0] position;

// State machine signals that indicate
// when steps increment or decrement
// ...HIGH for one clock...
reg inc_step;
reg dec_step;

// *****************************
// Double flop inputs for meta-stability concerns
// ... so, use *_ff2 for actual input
//
// A => A_ff1 => A_ff2
//
// *****************************

reg A_ff1, A_ff2;
reg B_ff1, B_ff2;
reg Z_ff1, Z_ff2;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n) begin
		A_ff1 = 0;
		A_ff2 = 0;
		B_ff1 = 0;
		B_ff2 = 0;
		Z_ff1 = 0;
		Z_ff2 = 0;
	end
	
	else begin
		// Flop A
		A_ff1 <= A;
		A_ff2 <= A_ff1;
		
		// Flop B
		B_ff1 <= B;
		B_ff2 <= B_ff1;
		
		// Flop Z
		Z_ff1 <= Z;
		Z_ff2 <= Z_ff1;
	end
end

// *****************************
// Concatenate filtered A and B inputs together for easy processing
// *****************************
wire [1:0] newAB;
assign newAB = {A_ff2, B_ff2};

// *****************************
// Create 32-bit counter which stores steps
// *****************************
reg [31:0] my_counter;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		my_counter <= 32'b0;
	
	else if (inc_step)
		my_counter <= my_counter + 1;
		
	else if (dec_step)
		my_counter <= my_counter - 1;
	
	else
	   	my_counter <= my_counter;
end

assign counter = my_counter;

// *****************************
//         State Machine
// *****************************

`define SM_00	(2'b00)
`define SM_01	(2'b01)
`define SM_10	(2'b10)
`define SM_11	(2'b11)

// Infer state registers
reg [1:0] state;
reg [1:0] next_state;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		state <= `SM_00;
	else
		state <= next_state;
end

always @(*) begin
	// Set default outputs
	next_state = state;
	dec_step = 0;
	inc_step = 0;
	
	case (state)
		`SM_00: begin
			if (newAB == 2'b01) begin
				dec_step = 1;
				next_state = `SM_01;
			end

			if (newAB == 2'b10) begin
				inc_step = 1;
				next_state = `SM_10;
			end
		end
		
		`SM_01: begin
			if (newAB == 2'b00) begin
				inc_step = 1;
				next_state = `SM_00;
			end

			if (newAB == 2'b11) begin
				dec_step = 1;
				next_state = `SM_11;
			end
		end
		
		`SM_10: begin
			if (newAB == 2'b00) begin
				dec_step = 1;
				next_state = `SM_00;
			end

			if (newAB == 2'b11) begin
				inc_step = 1;
				next_state = `SM_11;
			end
		end
		
		`SM_11: begin
			if (newAB == 2'b01) begin
				inc_step = 1;
				next_state = `SM_01;
			end

			if (newAB == 2'b10) begin
				dec_step = 1;
				next_state = `SM_10;
			end
		end
	endcase
end




// *****************************
// Create position output.
//
// NOTE: # bits configured from C code
//       to match encoder user has attached to AMDC.
//
//       Typically ~12-bit (2^12 = 4096 pulses per rev).
//       
//       Stored in `pulses_per_rev_bits` input signal.
//
//       Max pulses per rev: 2^32
// *****************************

wire [31:0] MAX_POS;
assign MAX_POS = (32'd1 << pulses_per_rev_bits) - 32'd1;

// Find rising edge of Z
wire z_rise;
reg z_delay;
always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		z_delay <= 0;
	else
		z_delay <= Z_ff2;
end
assign z_rise = (Z_ff2 && ~z_delay);


reg [31:0] my_pos;
reg know_pos;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n) begin
		my_pos <= 32'hFFFFFFFF;
		know_pos = 0;
	end
	
	else if (z_rise) begin
		know_pos = 1;
		my_pos <= 32'b0;
	end
	
	else if (inc_step)
		if (my_pos == MAX_POS)
			my_pos <= 32'b0;
		else
			my_pos <= my_pos + 32'd1;
			
	else if (dec_step)
		if (my_pos == 32'b0)
			my_pos <= MAX_POS;
		else
			my_pos <= my_pos - 32'd1;

	else
	   	my_pos <= my_pos;
end

assign position = know_pos ? my_pos : 32'hFFFFFFFF;

endmodule
