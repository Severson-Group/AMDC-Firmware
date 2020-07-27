// *******
//
// ADC Interface Driver for Analog Devices LTC2320-14 Chip
// 
// Originally created by
// Nathan Petersen, September 12, 2018
//
// *******
//
// Notes:
// - SCK needs to be 100MHz to achieve 1.5Msps rate.
// - Negative edge of SCK starts shift of data out onto SDO
//
// - SDO data bits come in MSB first
// - Latch SDO bit right before falling edge of SDO
//
// - Wait 450ns after asserting CNV_n before starting SCK (90 cycles at 200MHz)

// Assume clk = 200MHz, period = 5ns
`define CYCLES_TO_ASSERT_CNV	(7'd6)  // 30ns
`define CYCLES_TO_WAIT_SAMPLING	(7'd90) // 450ns
`define CYCLES_TO_HANG			(7'd200) // 1000ns

// Set SCK divisor from system clock (200MHz)
`define SCK_DIV2  (2'b00) // 100   MHz
`define SCK_DIV4  (2'b01) //  50   MHz
`define SCK_DIV8  (2'b10) //  25   MHz
`define SCK_DIV16 (2'b11) //  12.5 MHz

module drv_ltc2320(
	clk, rst_n,
	CNV_n, SCK, SDO, CLKOUT,
	data_valid,
	clkdiv,
	data1, data2, data3, data4,
	data5, data6, data7, data8
);

// System signals
input clk, rst_n;

input CLKOUT;

input wire [1:0] clkdiv;

// ADC signals
output reg CNV_n;
output SCK;
input [7:0] SDO;

output reg data_valid;
output wire [14:0] data1;
output wire [14:0] data2;
output wire [14:0] data3;
output wire [14:0] data4;
output wire [14:0] data5;
output wire [14:0] data6;
output wire [14:0] data7;
output wire [14:0] data8;

reg [15:0] data1reg;
reg [15:0] data2reg;
reg [15:0] data3reg;
reg [15:0] data4reg;
reg [15:0] data5reg;
reg [15:0] data6reg;
reg [15:0] data7reg;
reg [15:0] data8reg;

// SM outputs
reg incr_bit_counter;
reg reset_bit_counter;
reg reset_delay_counter;
reg reset_sck_div;
reg shift_sdo;
reg sck_enabled;
reg deassert_cnv_n;
reg assert_cnv_n;
reg deassert_data_valid;
reg assert_data_valid;


// *****************************
// *****************************
//      CNV_n set/reset flop
// *****************************
// *****************************
always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		CNV_n <= 1'b0;
	else if (deassert_cnv_n)
		CNV_n <= 1'b0;
	else if (assert_cnv_n)
		CNV_n <= 1'b1;
	else
		CNV_n <= CNV_n;
end


// *****************************
// *****************************
//    data_valid set/reset flop
// *****************************
// *****************************
always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		data_valid <= 1'b0;
	else if (deassert_data_valid)
		data_valid <= 1'b0;
	else if (assert_data_valid)
		data_valid <= 1'b1;
	else
		data_valid <= data_valid;
end


// *****************************
// *****************************
//         SCK generation
// *****************************
// *****************************

reg [3:0] sck_div;

/**
 * 00: +8 ... /2  ... 100MHz
 * 01: +4 ... /4  ... 50MHz
 * 10: +2 ... /8  ... 25MHz
 * 11: +1 ... /16 ... 12.5MHz
 */

// Divide clk to form SCK
always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		sck_div <= 4'b0;
	else if (reset_sck_div)
		sck_div <= 4'b0;
	else if (clkdiv == `SCK_DIV2)
		sck_div <= sck_div + 8;
	else if (clkdiv == `SCK_DIV4)
		sck_div <= sck_div + 4;
	else if (clkdiv == `SCK_DIV8)
		sck_div <= sck_div + 2;
	else if (clkdiv == `SCK_DIV16)
		sck_div <= sck_div + 1;
end

assign SCK = (sck_enabled) ? sck_div[3] : 0;


// *****************************
// *****************************
//         DATA register
// *****************************
// *****************************

always @(posedge clk, negedge rst_n) begin
	if (!rst_n) begin
		data1reg <= 16'b0;
		data2reg <= 16'b0;
		data3reg <= 16'b0;
		data4reg <= 16'b0;
		data5reg <= 16'b0;
		data6reg <= 16'b0;
		data7reg <= 16'b0;
		data8reg <= 16'b0;
	end

	else if (shift_sdo) begin
		data1reg <= {data1reg[14:0], SDO[0]}; // shift in data on LSB side
		data2reg <= {data2reg[14:0], SDO[1]}; // shift in data on LSB side
		data3reg <= {data3reg[14:0], SDO[2]}; // shift in data on LSB side
		data4reg <= {data4reg[14:0], SDO[3]}; // shift in data on LSB side
		data5reg <= {data5reg[14:0], SDO[4]}; // shift in data on LSB side
		data6reg <= {data6reg[14:0], SDO[5]}; // shift in data on LSB side
		data7reg <= {data7reg[14:0], SDO[6]}; // shift in data on LSB side
		data8reg <= {data8reg[14:0], SDO[7]}; // shift in data on LSB side
	end
end

assign data1 = data1reg[15:1];
assign data2 = data2reg[15:1];
assign data3 = data3reg[15:1];
assign data4 = data4reg[15:1];
assign data5 = data5reg[15:1];
assign data6 = data6reg[15:1];
assign data7 = data7reg[15:1];
assign data8 = data8reg[15:1];


// *****************************
// *****************************
//         Bit Counter
// *****************************
// *****************************

// 5-bit counter: counts 16 bits

reg [4:0] bit_counter;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		bit_counter <= 5'b0;
	else if (reset_bit_counter)
		bit_counter <= 5'b0;
	else if (incr_bit_counter)
		bit_counter <= bit_counter + 1;
	else
		bit_counter <= bit_counter;
end


// *****************************
// *****************************
//         Delay Counter
// *****************************
// *****************************

// 7-bit counter: up to 128
// Need to count max 450ns, so at 200MHz clk => 90 cycles (< 128)

reg [6:0] delay_counter;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		delay_counter <= 7'b0;
	else if (reset_delay_counter)
		delay_counter <= 7'b0;
	else
		delay_counter <= delay_counter + 1;
end


// *****************************
// *****************************
//         State Machine
// *****************************
// *****************************

`define SM_CNV				(3'b000)
`define SM_WAIT_CNV			(3'b001)
`define SM_WAIT_SAMPLE		(3'b010)
`define SM_RECV				(3'b011)
`define SM_HANG				(3'b100)

// Infer state registers
reg [2:0] state;
reg [2:0] next_state;

always @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		state <= `SM_CNV;
	else
		state <= next_state;
end

always @(*) begin
	// Set default outputs:
	next_state = state;
	incr_bit_counter = 0;
	reset_bit_counter = 0;
	reset_delay_counter = 0;
	reset_sck_div = 0;
	shift_sdo = 0;
	sck_enabled = 0;
	deassert_cnv_n = 0;
	assert_cnv_n = 0;
	deassert_data_valid = 0;
	assert_data_valid = 0;
	
	case (state)
		`SM_CNV: begin
			// Start conversion!
			assert_cnv_n = 1;
			reset_delay_counter = 1;
			
			next_state = `SM_WAIT_CNV;
		end
		
		`SM_WAIT_CNV: begin
			// Waits for minimum CNV_n pulse width
			if (delay_counter >= `CYCLES_TO_ASSERT_CNV) begin
				reset_delay_counter = 1;
				next_state = `SM_WAIT_SAMPLE;
			end
		end
		
		`SM_WAIT_SAMPLE: begin
			// Waits for conversion to happen
			deassert_cnv_n = 1;
		
			if (delay_counter >= `CYCLES_TO_WAIT_SAMPLING) begin
				// Conversion is done
				reset_bit_counter = 1;
				reset_sck_div = 1;
				deassert_data_valid = 1;
				next_state = `SM_RECV;
			end
		end
		
		`SM_RECV: begin
			// Gets data bits from ADC via SDO line
			sck_enabled = 1;
		
			// Trying to identify clk cycle right before falling edge of SCK
			// and since SCK is MSB of sck_div, look for when counter going to roll over:
			shift_sdo = (clkdiv == `SCK_DIV2) ? (sck_div == 4'b1000) : // +8
						(clkdiv == `SCK_DIV4) ? (sck_div == 4'b1100) : // +4
						(clkdiv == `SCK_DIV8) ? (sck_div == 4'b1110) : // +2
					  /* clkdiv == `SCK_DIV16*/ (sck_div == 4'b1111);  // +1

			incr_bit_counter = shift_sdo;
			
			// After 16 bits, we're done
			if (bit_counter >= 5'd16) begin
				reset_delay_counter = 1;
				assert_data_valid = 1;
				next_state = `SM_HANG;
			end
		end
		
		`SM_HANG: begin
			// Wait for a bit of dead time between conversions
			if (delay_counter >= `CYCLES_TO_HANG) begin
				next_state = `SM_CNV;
			end
		end
	endcase
end

endmodule