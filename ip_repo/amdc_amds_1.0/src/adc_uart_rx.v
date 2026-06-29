`timescale 1 ns / 1 ps

module adc_uart_rx(
    input wire clk,
    input wire rst_n,
    
    input wire start_rx,
    
    // The data line coming from the AMDS
    input wire din,
    input wire [11:0] is_dout_enabled,
    
    output reg [11:0] is_dout_valid, // is_dout_valid[0] == 1 implies that adc_dout0 is valid
    output reg adc_uart_done,
    output reg assert_done,
    output wire [15:0] adc_dout0,
    output wire [15:0] adc_dout1,
    output wire [15:0] adc_dout2,
    output wire [15:0] adc_dout3,
    output wire [15:0] adc_dout4,
    output wire [15:0] adc_dout5,
    output wire [15:0] adc_dout6,
    output wire [15:0] adc_dout7,
    output wire [15:0] adc_dout8,
    output wire [15:0] adc_dout9,
    output wire [15:0] adc_dout10,
    output wire [15:0] adc_dout11,
    
    output reg [15:0] counter_bytes_valid,
    output reg [15:0] counter_bytes_corrupt,
    output reg [15:0] counter_bytes_timed_out
);

wire[15:0] counter_bytes_valid_next;
wire[15:0] counter_bytes_corrupt_next;
wire[15:0] counter_bytes_timed_out_next;

reg[3:0] sensor_index;
wire[3:0] sensor_index_next;
reg MSB;
reg[15:0] timer;
wire[15:0] timer_next;
reg finalize;

reg[15:0] adc_data[11:0];
assign adc_dout0 = adc_data[0];
assign adc_dout1 = adc_data[1];
assign adc_dout2 = adc_data[2];
assign adc_dout3 = adc_data[3];
assign adc_dout4 = adc_data[4];
assign adc_dout5 = adc_data[5];
assign adc_dout6 = adc_data[6];
assign adc_dout7 = adc_data[7];
assign adc_dout8 = adc_data[8];
assign adc_dout9 = adc_data[9];
assign adc_dout10 = adc_data[10];
assign adc_dout11 = adc_data[11];

assign sensor_index_next = sensor_index + 1;
assign timer_next = timer + 1;

assign counter_bytes_valid_next = counter_bytes_valid + 1;
assign counter_bytes_corrupt_next = counter_bytes_corrupt + 1;
assign counter_bytes_timed_out_next = counter_bytes_timed_out + 1;


	// input wire clk,
	// input wire rst_n,
	
	// // UART data input (not in our clock domain!)
	// // 
	// // UART settings
	// // > Bit Rate (MBit/s): 20
	// // > Bits per Frame:    8
	// // > Stop bits:         2
	// // > Parity bit:        Odd
	// // > Significant bit:   LSB first
	// input wire din,
	
	// // Asserted when we saw a start bit and have receieved all data and it's valid
	// output reg is_byte_valid,

	// // Asserted when we saw a start bit and received all data, but failed the parity check
	// output reg is_byte_corrupt,
	
	// // Holds the contents of what we received over the UART line
	// output wire [7:0] dout
wire valid;
wire corrupt;
wire[7:0] ephemeral_data;
wire read_complete;
reg should_be_reading;
assign read_complete = (valid | corrupt) & should_be_reading;
wire uart_rst_n;
reg trigger_uart_rst_n;
assign uart_rst_n = rst_n & trigger_uart_rst_n;
uart_rx byte_reader(
    .clk(clk),
    .rst_n(uart_rst_n),
    .din(din),
    .is_byte_valid(valid),
    .is_byte_corrupt(corrupt),
    .dout(ephemeral_data)
);

// on byte{
//     if valid,
//         fetch sensor index
//         if msb
//             set some bits
//             set MSB to false
//         otherwise,
//             set other bits
//             increment sensor index
//             Set MSB to true
        
        
//     Otherwise,
//         Give up. There is no recovery.
// reset uart.
// }

always @(posedge clk, negedge rst_n) begin
    if(!rst_n) begin
        timer <= 0;
        MSB <= 1;
        sensor_index <= 0;
        is_dout_valid <= 0;
        should_be_reading <= 0;
        finalize <= 0;
        counter_bytes_corrupt <= 0;
        counter_bytes_timed_out <= 0;
        counter_bytes_valid <= 0;
        trigger_uart_rst_n <= 1;
        adc_uart_done <= 0;
        assert_done <= 0;
    end else if (read_complete & trigger_uart_rst_n) begin
        timer <= 0;
        if (valid == 1) begin
            if (MSB == 1) begin
                MSB <= 0;
                adc_data[sensor_index][15:8] <= ephemeral_data;
            end else begin
                MSB <= 1;
                adc_data[sensor_index][7:0] <= ephemeral_data;
                is_dout_valid[sensor_index] <= 1;
                sensor_index <= sensor_index_next;
                if (sensor_index == 11) begin
                    finalize <= 1;
                end
            end
            counter_bytes_valid <= counter_bytes_valid_next;
        end else begin
            finalize <= 1;
            counter_bytes_corrupt <= counter_bytes_corrupt_next;
        end 
        trigger_uart_rst_n <= 0;
    end else if (finalize == 1) begin
        finalize <= 0;
        assert_done <= 1;
        adc_uart_done <= 1;
        MSB <= 1;
        should_be_reading <= 0;
        trigger_uart_rst_n <= 1;
    end else if (!should_be_reading) begin
        sensor_index <= 0;
        assert_done <= 0;
        timer <= 0;
        if (start_rx) begin
            should_be_reading <= 1;
            adc_uart_done <= 0;
        end
    end else begin
        timer <= timer_next;
        trigger_uart_rst_n <= 1;
        if (timer >= 2000) begin
            finalize <= 1;
            counter_bytes_timed_out <= counter_bytes_timed_out_next;
        end
    end
end
endmodule