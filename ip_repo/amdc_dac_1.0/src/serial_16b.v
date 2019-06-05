`timescale 1ns / 1ps

// ----------
// serial_16b
// ----------
// 
// Nathan Petersen, 5/24/2019
//
// This module transmits a 16-bit word onto the 'DIN'
// line via the serial protocol. To use, set 'data'
// to the desired 16-bit output and assert 'start'
//
// 'done' will go high once the word has been sent.
//
module serial_16b(clk, rst_n, data, start, done, sclk_div, SYNC, SCLK, DIN);

input clk, rst_n;

input [15:0] data;
input start;
input done;

input [31:0] sclk_div;

output wire SYNC;		// SYNC signal going to DAC IC
output wire SCLK;		// SCLK signal going to DAC IC
output wire DIN;		// DIN  signal going to DAC IC


// *****************************
// *****************************
//         SCLK generation
// *****************************
// *****************************


reg [31:0] sclk_counter;
reg sclk_enabled;
reg my_sclk;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n) begin
		sclk_counter <= 32'b0;
		my_sclk = 1'b0;
	end

	else if (!sclk_enabled) begin
		sclk_counter <= 32'b0;
	end

	else if (sclk_counter >= (sclk_div - 32'd1)) begin
		sclk_counter <= 32'b0;
		my_sclk <= !my_sclk;
	end
	
	else begin
		sclk_counter <= sclk_counter + 32'd1;
	end

end

assign SCLK = my_sclk;

// Find rising edge of SCLK
wire SCLK_rise;
wire SCLK_rise2;
wire SCLK_fall;
assign SCLK_rise  = (my_sclk == 1'b0) & (sclk_counter == sclk_div - 32'd1);
assign SCLK_rise2 = (my_sclk == 1'b0) & (sclk_counter == sclk_div - 32'd2);
assign SCLK_fall  = (my_sclk == 1'b1) & (sclk_counter == sclk_div - 32'd1);


// *************************
// *************************
//       SPI bit counter
// *************************
// *************************

reg [4:0] spi_bit_counter;
reg inc_spi_bit_counter;
reg reset_spi_bit_counter;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		spi_bit_counter <= 5'b0;
	else if (reset_spi_bit_counter)
		spi_bit_counter <= 5'b0;
	else if (inc_spi_bit_counter)
		spi_bit_counter <= spi_bit_counter + 5'd1;
	else
		spi_bit_counter <= spi_bit_counter;
end


// *************************
// *************************
//     SPI data shift reg
// *************************
// *************************

reg [16:0] spi_shift_reg;
reg spi_shift_reg_shift;
reg spi_shift_reg_load;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		spi_shift_reg <= 16'b0;
	else if (spi_shift_reg_load)
		spi_shift_reg <= {1'b0, data[15:0]};
	else if (spi_shift_reg_shift)
		spi_shift_reg <= {spi_shift_reg[15:0], 1'b0};
end

// Shift out data MSB first
assign DIN = done ? 1'b0 : spi_shift_reg[16];


// *************************
// *************************
//     Set/Reset flop
//    for `sclk_enabled` output
// *************************
// *************************

reg set_sclk_enabled;
reg clr_sclk_enabled;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		sclk_enabled <= 0;
	else if (set_sclk_enabled)
		sclk_enabled <= 1;
	else if (clr_sclk_enabled)
		sclk_enabled <= 0;
end


// *************************
// *************************
//     Set/Reset flop
//    for `done` output
// *************************
// *************************

reg my_done;
reg set_done;
reg clr_done;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		my_done <= 0;
	else if (set_done)
		my_done <= 1;
	else if (clr_done)
		my_done <= 0;
end

assign done = my_done;


// *************************
// *************************
//     Set/Reset flop
//    for `SYNC` output
// *************************
// *************************

reg my_sync;
reg assert_sync;
reg deassert_sync;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		my_sync <= 1;
	else if (assert_sync)
		my_sync <= 0;
	else if (deassert_sync)
		my_sync <= 1;
end

assign SYNC = my_sync;


// *****************************
// *****************************
//       SPI State Machine
// *****************************
// *****************************

`define SM_SPI_IDLE	(1'b0)
`define SM_SPI_BITS	(1'b1)

// Infer state registers
reg state;
reg next_state;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		state <= `SM_SPI_IDLE;
	else
		state <= next_state;
end

always @(*) begin
	// Set default outputs:
	next_state = state;
	inc_spi_bit_counter = 0;
	reset_spi_bit_counter = 0;
	spi_shift_reg_shift = 0;
	spi_shift_reg_load = 0;
	set_done = 0;
	clr_done = 0;
	assert_sync = 0;
	deassert_sync = 0;
	set_sclk_enabled = 0;
	clr_sclk_enabled = 0;
	
	case (state)
		`SM_SPI_IDLE: begin
			if (start) begin
				next_state = `SM_SPI_BITS;
				clr_done = 1;
				set_sclk_enabled = 1;
				spi_shift_reg_load = 1;
				assert_sync = 1;
			end
		end
		
		
		`SM_SPI_BITS: begin
			if (SCLK_rise) begin
				inc_spi_bit_counter = 1;
				spi_shift_reg_shift = 1;
			end
			
			if (spi_bit_counter == 5'd16 & SCLK_rise2) begin
				// Done shifting bits!
				clr_sclk_enabled = 1;
				set_done = 1;
				deassert_sync = 1;
				reset_spi_bit_counter = 1;
				next_state = `SM_SPI_IDLE;
			end
		end
	endcase
end

endmodule