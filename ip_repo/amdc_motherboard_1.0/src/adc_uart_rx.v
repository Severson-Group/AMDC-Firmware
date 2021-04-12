`timescale 1 ns / 1 ps

module adc_uart_rx(
	input wire clk,
	input wire rst_n,
	
	// Flag which is triggered for 1 clock cycle when a transmission is requested from the motherboard
	input wire start_rx,
	
	// The data line coming from the motherboard
	input wire din,
	
	output reg is_dout_valid,
	output reg [15:0] adc_dout0,
	output reg [15:0] adc_dout1,
	output reg [15:0] adc_dout2,
	output reg [15:0] adc_dout3,
	
    output reg [15:0] counter_data_valid,
    output reg [15:0] counter_data_corrupt,
    output reg [15:0] counter_data_timeout
);


// =======================
// S/R flop: is_dout_valid
// =======================

reg deassert_data_valid;
reg assert_data_valid;

always @(posedge clk, negedge rst_n) begin
	if (~rst_n)
		is_dout_valid <= 1'b0;
	else if (deassert_data_valid)
		is_dout_valid <= 1'b0;
	else if (assert_data_valid)
		is_dout_valid <= 1'b1;
	else
		is_dout_valid <= is_dout_valid;
end

// ==============
// Packet Counter
// ==============

reg rst_packet_counter;
reg inc_packet_counter;

reg [3:0] packet_counter;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		packet_counter <= 4'b0;
	else if (rst_packet_counter)
		packet_counter <= 4'b0;
	else if (inc_packet_counter)
		packet_counter <= packet_counter + 1;
	else
		packet_counter <= packet_counter;
end

// ==============
// Data Valid Counter
// ==============

reg inc_counter_data_valid;
always @(posedge clk, negedge rst_n) begin
	if (~rst_n)
		counter_data_valid <= 16'b0;
	else if (inc_counter_data_valid)
		counter_data_valid <= counter_data_valid + 1;
end

// ==============
// Data Corrupt Counter
// ==============

reg inc_counter_data_corrupt;
always @(posedge clk, negedge rst_n) begin
	if (~rst_n)
		counter_data_corrupt <= 16'b0;
	else if (inc_counter_data_corrupt)
		counter_data_corrupt <= counter_data_corrupt + 1;
end

// ==============
// Data Timeout Counter
// ==============

reg inc_counter_data_timeout;
always @(posedge clk, negedge rst_n) begin
	if (~rst_n)
		counter_data_timeout <= 16'b0;
	else if (inc_counter_data_timeout)
		counter_data_timeout <= counter_data_timeout + 1;
end


// ==============
// UART RX module
// ==============

wire uart_is_byte_valid;
wire uart_is_data_corrupt;
wire uart_is_rx_timeout;

reg uart_start_rx;
wire [7:0] uart_data_byte;

uart_rx iUART_RX(
	.clk(clk),
	.rst_n(rst_n),
	.din(din),
	.start_rx(uart_start_rx),
	.is_byte_valid(uart_is_byte_valid),
	.is_data_corrupt(uart_is_data_corrupt),
	.is_rx_timeout(uart_is_rx_timeout),
	.dout(uart_data_byte)
);

wire uart_is_rx_error;
assign uart_is_rx_error = uart_is_data_corrupt | uart_is_rx_timeout;

// ===================
// Loading Output Data
// ===================

reg load_doutN_LSB;
reg load_doutN_MSB;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n) begin
		adc_dout0 <= 16'b0;
		adc_dout1 <= 16'b0;
		adc_dout2 <= 16'b0;
		adc_dout3 <= 16'b0;
	end

	else if (load_doutN_LSB) begin
		case (packet_counter)
			4'd0: begin
				adc_dout0[7:0] <= uart_data_byte;
			end
			4'd1: begin
				adc_dout1[7:0] <= uart_data_byte;
			end
			4'd2: begin
				adc_dout2[7:0] <= uart_data_byte;
			end
			4'd3: begin
				adc_dout3[7:0] <= uart_data_byte;
			end
		endcase
	end
	
	else if (load_doutN_MSB) begin
		case (packet_counter)
			4'd0: begin
				adc_dout0[15:8] <= uart_data_byte;
			end
			4'd1: begin
				adc_dout1[15:8] <= uart_data_byte;
			end
			4'd2: begin
				adc_dout2[15:8] <= uart_data_byte;
			end
			4'd3: begin
				adc_dout3[15:8] <= uart_data_byte;
			end
		endcase
	end
end

// =============
// State Machine
// =============

`define SM_IDLE              (3'h0)
`define SM_WAIT_FOR_HEADER   (3'h1)
`define SM_WAIT_FOR_DATA_MSB (3'h2)
`define SM_WAIT_FOR_DATA_LSB (3'h3)
`define SM_DONE              (3'h4)

// Infer state registers
reg [2:0] state;
reg [2:0] next_state;

always @(posedge clk, negedge rst_n) begin
	if (~rst_n)
		state <= `SM_IDLE;
	else
		state <= next_state;
end

always @(*) begin
	// Set default outputs
	next_state = state;

	uart_start_rx = 0;
	
	rst_packet_counter = 0;
	inc_packet_counter = 0;
	
	assert_data_valid = 0;
	deassert_data_valid = 0;
	
	load_doutN_LSB = 0;
	load_doutN_MSB = 0;
	
	inc_counter_data_valid = 0;
	inc_counter_data_corrupt = 0;
	inc_counter_data_timeout = 0;
	
	case (state)
		`SM_IDLE: begin
			if (start_rx) begin
				// Incoming UART transmission, so start UART RX module
				uart_start_rx = 1;
				
				// Tell user that the data is no longer valid
				deassert_data_valid = 1;
			
				rst_packet_counter = 1;
				next_state = `SM_WAIT_FOR_HEADER;
			end
		end
		
		`SM_WAIT_FOR_HEADER: begin
			if (uart_is_byte_valid) begin
				// Check that the new header is valid:
				// * expect MSB byte to be 0x9
				// * expect packet number in LSB byte
				if ((uart_data_byte[7:4] == 4'h9) & (uart_data_byte[3:0] == packet_counter)) begin
					// Yay! Valid header!
					inc_counter_data_valid = 1;
					
					next_state = `SM_WAIT_FOR_DATA_MSB;
					uart_start_rx = 1;
				end
				else begin
					// Something broke... back to beginning!
					next_state = `SM_IDLE;
				end
			end
			
			else if (uart_is_rx_error) begin
				// Something broke... back to beginning!
				next_state = `SM_IDLE;
				
				if (uart_is_data_corrupt)
				    inc_counter_data_corrupt = 1;
				else
				    inc_counter_data_timeout = 1;
			end
		end
		
		`SM_WAIT_FOR_DATA_MSB: begin
			if (uart_is_byte_valid) begin
			    inc_counter_data_valid = 1;
				load_doutN_MSB = 1;
				next_state = `SM_WAIT_FOR_DATA_LSB;
				uart_start_rx = 1;
			end
			
			else if (uart_is_rx_error) begin
				// Something broke... back to beginning!
				next_state = `SM_IDLE;
				
				if (uart_is_data_corrupt)
				    inc_counter_data_corrupt = 1;
				else
				    inc_counter_data_timeout = 1;
			end
		end
		
		`SM_WAIT_FOR_DATA_LSB: begin
			if (uart_is_byte_valid) begin
			    inc_counter_data_valid = 1;
				load_doutN_LSB = 1;
				next_state = `SM_DONE;
			end
			
			else if (uart_is_rx_error) begin
				// Something broke... back to beginning!
				next_state = `SM_IDLE;
				
				if (uart_is_data_corrupt)
				    inc_counter_data_corrupt = 1;
				else
				    inc_counter_data_timeout = 1;
			end
		end
		
		`SM_DONE: begin
			if (packet_counter >= 4'd3) begin
				// Done! Got all data now
				// Tell user that the data is now valid
				assert_data_valid = 1;
				next_state = `SM_IDLE;
			end
			else begin
				// Still more data to rx
				inc_packet_counter = 1;
				uart_start_rx = 1;
				next_state = `SM_WAIT_FOR_HEADER;
			end
		end
	endcase
end
	
endmodule