`timescale 1ns / 1ps

module ild1420_rx(clk, rst_n, distance, error, fresh, din);

input clk, rst_n;
input din;

output [15:0] distance;
output [1:0] error;
output fresh;


// ==============
// Double flop "din" for meta-stability concerns!
// ... so, use *_ff2 for actual input
//
// din => din_ff1 => din_ff2
//
// NOTE: din_ff3 is used to detect falling edges
//
// ==============

reg din_ff1, din_ff2, din_ff3;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n) begin
		din_ff1 = 1'b0;
		din_ff2 = 1'b0;
		din_ff3 = 1'b0;
	end
	
	else begin
		// Flop din
		din_ff1 <= din;
		din_ff2 <= din_ff1;
		din_ff3 <= din_ff2;
	end
end


// ===========================
// Falling Edge Detector (din)
// ===========================

wire din_fall;
assign din_fall = (~din_ff2 && din_ff3) ? 1'b1 : 1'b0;


// ==============
// Deadtime Timer
// ==============

reg [12:0] dt_timer;
reg rst_dt_timer;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		dt_timer <= 13'b0;
	else if (rst_dt_timer)
		dt_timer <= 13'b0;
	else
		dt_timer <= dt_timer + 13'd1;
end

reg seen_dt;
reg rst_seen_dt;
reg set_seen_dt;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		seen_dt <= 1'b0;
	else if (rst_seen_dt)
		seen_dt <= 1'b0;
	else if (set_seen_dt)
		seen_dt <= 1'b1;
end

always @(posedge clk) begin
	// Ensure we are aligned to the UART data
	// by making sure that enough dead-time
	// elapsed between samples (4kHz rate)
	//
	// One transmission of data takes:
	// tx_length = (30 bits * 1 / 921600 baudrate * 1e9))
	// tx_length = 32555 ns
	// 
	// We want to ensure we have seen AT LEAST tx_length of dead-time,
	// so round up to 40k ns
	//
	// At clk = 5ns, this is 40000 / 5 = 8000 cycles
	if (~seen_dt & dt_timer > 13'd8000)
		set_seen_dt = 1;
	else
		rst_seen_dt = 0;
end



// ==============
// Baud Period Timer
// ==============

reg [7:0] baud_timer;
reg rst_baud_timer;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		baud_timer <= 8'b0;
	else if (rst_baud_timer)
		baud_timer <= 8'b0;
	else
		baud_timer <= baud_timer + 8'd1;
end


// ==============
// Fresh Timer
// ==============

reg [15:0] fresh_timer;
reg rst_fresh_timer;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		fresh_timer <= 16'b0;
	else if (rst_fresh_timer)
		fresh_timer <= 16'b0;
	else
		fresh_timer <= fresh_timer + 16'd1;
end


// ===========
// Bit counter
// ===========

reg [4:0] bit_counter;
reg inc_bit_counter;
reg rst_bit_counter;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		bit_counter <= 5'b0;
	else if (rst_bit_counter)
		bit_counter <= 5'b0;
	else if (inc_bit_counter)
		bit_counter <= bit_counter + 5'd1;
	else
		bit_counter <= bit_counter;
end


// ============
// Set / Rst FF for "fresh"
// ============

reg my_fresh;
reg rst_fresh;
reg set_fresh;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		my_fresh <= 0;
	else if (rst_fresh)
		my_fresh <= 0;
	else if (set_fresh)
		my_fresh <= 1;
end

assign fresh = my_fresh;


// =================
// UART RX Shift Reg
// =================

reg [29:0] shift_reg;
reg shift_reg_shift;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		shift_reg <= 30'b0;
	else if (shift_reg_shift)
		// Shift data in MSB to LSB progression
		shift_reg <= {din_ff2, shift_reg[29:1]};
end

wire byte_headers_valid;
assign byte_headers_valid = (~shift_reg[7] & ~shift_reg[8] & ~shift_reg[18] & shift_reg[17] & shift_reg[28]) ? 1'b1 : 1'b0;


// ================
// Output registers
// ================

reg [15:0] my_distance;
reg [1:0] my_error;
reg load_output_regs;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n) begin
		my_distance <= 16'b0;
		my_error <= 2'b0;
	end
	
	else if (load_output_regs) begin
		my_distance <= {shift_reg[23:20], shift_reg[16:11], shift_reg[6:1]};
		my_error <= shift_reg[26:25];
	end
end

assign distance = my_distance;
assign error = my_error;


// =============
// State Machine
// =============

`define SM_DT             (2'h0)
`define SM_WAIT_HALF_BAUD (2'h1)
`define SM_WAIT_FULL_BAUD (2'h2)
`define SM_END_OF_RX      (2'h3)

reg [1:0] state;
reg [1:0] next_state;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		state <= `SM_DT;
	else
		state <= next_state;
end

// "Continuous" hardware -- always active
always @(*) begin
	// Set default outputs
	next_state = state;
	
	load_output_regs = 0;
	shift_reg_shift = 0;
	set_fresh = 0;
	inc_bit_counter = 0;
	rst_bit_counter = 0;
	rst_fresh_timer = 0;
	rst_baud_timer = 0;
	rst_dt_timer = 0;
	
	case (state)
		`SM_DT: begin
			if (din_fall) begin
				// Make sure we have seen enough dead-time
				if (seen_dt) begin
					rst_baud_timer = 1;
					rst_bit_counter = 1;
					next_state = `SM_WAIT_HALF_BAUD;
				end
				
				else begin
					rst_dt_timer = 1;
				end
			end
		end
		
		`SM_WAIT_HALF_BAUD: begin
			// Wait for half of a baud period
			//
			// Means timer gets to timer_max = (((1 / baud_rate)/2) * 1e9 / clk_ns)
			// 
			// If clk_ns = 5ns and baud_rate 921600,
			// then timer_max = 108.5 cycles			
			//
			// Because we had to flop DIN a few times for meta-stability,
			// AND we needed another clock cycle to find falling edges,
			// we should not wait quite as long here to start the parsing
			// sequence. Should wait 3 cycles less.
			//
			// 108.5 - 3 = 105 cycles
			if (baud_timer > 8'd105) begin
				shift_reg_shift = 1;
				rst_baud_timer = 1;
				inc_bit_counter = 1;
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
			//
			// = 217 cycles
			if (baud_timer > 8'd217) begin
				shift_reg_shift = 1;
				rst_baud_timer = 1;
				inc_bit_counter = 1;
								
				if (bit_counter >= 5'd29) begin
					next_state = `SM_END_OF_RX;
				end
			end
		end
		
		`SM_END_OF_RX: begin
			if (byte_headers_valid) begin
				load_output_regs = 1;
				set_fresh = 1;
				rst_fresh_timer = 1;
			end
			
			next_state = `SM_DT;
		end
	endcase
end

always @(posedge clk) begin
	// Since we expect data at 4kHz (or at 250us centers),
	// let's indicate stale data after 300us
	//
	// 300e-6 * 1e9 / 5 = 60000 cycles
	//
	if (fresh & (fresh_timer > 16'd60000))
		rst_fresh = 1;
	else
		rst_fresh = 0;
end

endmodule