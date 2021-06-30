`timescale 1ns / 1ps

module ild1420_tx(clk, rst_n, distance, error, start, dout, done);

input clk, rst_n;
input [15:0] distance;
input [1:0] error;
input start;

output dout;
output done;


// ============
// Set / Rst FF (latches value)
// ============

reg my_done;
reg rst_done;
reg set_done;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		my_done <= 1;
	else if (set_done)
		my_done <= 1;
	else if (rst_done)
		my_done <= 0;
end

assign done = my_done;


// =============
// Timer (basically just a counter)
// =============

reg [7:0] timer;
reg rst_timer;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		timer <= 8'b0;
	else if (rst_timer)
		timer <= 8'b0;
	else
		timer <= timer + 8'd1;
end



// ===========
// Bit counter
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


// ==============
// UART Shift Reg
// ==============

reg [29:0] shift_reg;
reg shift_reg_shift;
reg shift_reg_load;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		shift_reg <= 30'b0;
	else if (shift_reg_load)
		shift_reg <= {
		1'b1, 2'b10, error, distance[15:12], 1'b0,
		1'b1, 2'b01, distance[11:6], 1'b0,
		1'b1, 2'b00, distance[5:0], 1'b0
		};
	else if (shift_reg_shift)
		shift_reg <= {1'b0, shift_reg[29:1]};
end

// Shift out data MSB first
assign dout = shift_reg[0];


// =============
// State Machine
// =============

`define SM_IDLE	(2'h0)
`define SM_TX	(2'h1)

reg [1:0] state;
reg [1:0] next_state;

// "Discrete" hardware -- depends on clk
always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		state <= `SM_IDLE;
	else
		state <= next_state;
end

// "Continuous" hardware -- always active
always @(*) begin
	// Set default outputs
	next_state = state;
	inc_counter = 0;
	rst_counter = 0;
	shift_reg_shift = 0;
	shift_reg_load = 0;
	rst_done = 0;
	set_done = 0;
	rst_timer = 0;
	
	case (state)
		`SM_IDLE: begin
			if (start) begin
				next_state = `SM_TX;
				rst_counter = 1;
				shift_reg_load = 1;
				rst_done = 1;
				rst_timer = 1;
			end
		end
		
		`SM_TX: begin
			// Wait for one "baud"
			// Means timer gets to timer_max = (1 / baud_rate * 1e9 / clk_ns)
			// 
			// If clk_ns = 5ns and baud_rate 921600,
			// then timer_max = 217 cycles
			if (timer > 8'd217) begin
				shift_reg_shift = 1;
				inc_counter = 1;
				rst_timer = 1;
				
				if (counter >= 5'd29) begin
					next_state = `SM_IDLE;
					set_done = 1;
				end
			end
		end
	endcase
end

endmodule