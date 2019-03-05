`timescale 1ns / 1ps

// Encoder Interface
//
// Nathan Petersen, 11/29/2018
//
// Given encoder signals A and B
// this module will count rotational steps
// and sum them into a binary counter register.
//
// NOTES:
// - Output `counter` is 32-bit binary counter value
//   i.e. it is not 2's complement format for negative values
//
module encoder(clk, rst_n, A, B, counter);

input A, B;
input clk;
input rst_n;

output wire [31:0] counter;

// *****************************
// Double flop inputs for meta-stability concerns
// ... so, use *_ff2 for actual input
//
// A => A_ff1 => A_ff2
//
// *****************************

reg A_ff1, A_ff2;
reg B_ff1, B_ff2;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n) begin
		A_ff1 = 0;
		A_ff2 = 0;
		B_ff1 = 0;
		B_ff2 = 0;
	end
	
	else begin
		// Flop A
		A_ff1 <= A;
		A_ff2 <= A_ff1;
		
		// Flop B
		B_ff1 <= B;
		B_ff2 <= B_ff1;
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
reg inc_counter;
reg dec_counter;
reg [31:0] my_counter;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		my_counter <= 32'b0;
//		inc_counter <= 0;
//		dec_counter <= 0;
	
	else if (inc_counter)
		my_counter <= my_counter + 1;
	
	else if (dec_counter)
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
	dec_counter = 0;
	inc_counter = 0;
	
	case (state)
		`SM_00: begin
			if (newAB == 2'b01) begin
				dec_counter = 1;
				next_state = `SM_01;
			end

			if (newAB == 2'b10) begin
				inc_counter = 1;
				next_state = `SM_10;
			end
		end
		
		`SM_01: begin
			if (newAB == 2'b00) begin
				inc_counter = 1;
				next_state = `SM_00;
			end

			if (newAB == 2'b11) begin
				dec_counter = 1;
				next_state = `SM_11;
			end
		end
		
		`SM_10: begin
			if (newAB == 2'b00) begin
				dec_counter = 1;
				next_state = `SM_00;
			end

			if (newAB == 2'b11) begin
				inc_counter = 1;
				next_state = `SM_11;
			end
		end
		
		`SM_11: begin
			if (newAB == 2'b01) begin
				inc_counter = 1;
				next_state = `SM_01;
			end

			if (newAB == 2'b10) begin
				dec_counter = 1;
				next_state = `SM_10;
			end
		end
	endcase
end

endmodule
