`timescale 1 ns / 1 ps

module adc_uart_rx(
    input wire clk,
    input wire rst_n,
    
    // Flag which is triggered for 1 clock cycle when a transmission is requested from the AMDS
    input wire start_rx,
    
    // The data line coming from the AMDS
    input wire din,
    
    output wire [3:0] is_dout_valid, // is_dout_valid[0] == 1 implies that adc_dout0 is valid
    output reg adc_uart_done,
    output reg [15:0] adc_dout0,
    output reg [15:0] adc_dout1,
    output reg [15:0] adc_dout2,
    output reg [15:0] adc_dout3,
    
    output reg [15:0] counter_data_valid,
    output reg [15:0] counter_data_corrupt,
    output reg [15:0] counter_data_timeout
);

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

// =======================
// S/R flops: is_doutN_valid
// =======================

reg is_dout0_valid, is_dout1_valid, is_dout2_valid, is_dout3_valid;

reg assert_data_valid, clr_all_data_valid;

always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        is_dout0_valid <= 1'b0;
    else if (clr_all_data_valid)
        is_dout0_valid <= 1'b0;
    else if (assert_data_valid & packet_counter == 0)
        is_dout0_valid <= 1'b1;
end

always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        is_dout1_valid <= 1'b0;
    else if (clr_all_data_valid)
        is_dout1_valid <= 1'b0;
    else if (assert_data_valid & packet_counter == 1)
        is_dout1_valid <= 1'b1;
end

always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        is_dout2_valid <= 1'b0;
    else if (clr_all_data_valid)
        is_dout2_valid <= 1'b0;
    else if (assert_data_valid & packet_counter == 2)
        is_dout2_valid <= 1'b1;
end

always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        is_dout3_valid <= 1'b0;
    else if (clr_all_data_valid)
        is_dout3_valid <= 1'b0;
    else if (assert_data_valid & packet_counter == 3)
        is_dout3_valid <= 1'b1;
end

// Concatenate the individual valid registers into output bus
assign is_dout_valid = {is_dout3_valid, is_dout2_valid, is_dout1_valid, is_dout0_valid};


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
// Done
// ==============

reg assert_done, deassert_done;

always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        // Start as done, so that the trigger does not freeze
        adc_uart_done <= 1'b1;
    else if (deassert_done)
        adc_uart_done <= 1'b0;
    else if (assert_done)
        adc_uart_done <= 1'b1;
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

    assert_done = 0;
    deassert_done = 0;
    
    assert_data_valid = 0;
    clr_all_data_valid = 0;
    
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
                
                // Tell user that the all data regs are no longer valid
                clr_all_data_valid = 1;

                deassert_done = 1;
            
                rst_packet_counter = 1;
                next_state = `SM_WAIT_FOR_HEADER;
            end
        end
        
        `SM_WAIT_FOR_HEADER: begin
            if (uart_is_byte_valid) begin
                // Check that the new header is valid:
                // * expect MSByte to be 0x9
                // * expect packet number in LSByte
                if ((uart_data_byte[7:4] == 4'h9) & (uart_data_byte[3:0] == packet_counter)) begin
                    inc_counter_data_valid = 1;
                    next_state = `SM_WAIT_FOR_DATA_MSB;
                    uart_start_rx = 1; // start the receiver again for the first byte of actual data
                end
                else begin
                    // Something broke... back to beginning!
                    next_state = `SM_IDLE;
                    // assert done even if header mismatch, so this sensor doesn't freeze the timing manager
                    assert_done = 1;
                end
            end
            
            else if (uart_is_rx_error) begin
                // Something broke... back to beginning!
                next_state = `SM_IDLE;

                // assert done even if corrupt/timeout error, so this sensor doesn't freeze the timing manager
                assert_done = 1;
                
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

                // assert done even if corrupt/timeout error, so this sensor doesn't freeze the timing manager
                assert_done = 1;
                
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

                // assert done even if corrupt/timeout error, so this sensor doesn't freeze the timing manager
                assert_done = 1;
                
                if (uart_is_data_corrupt)
                    inc_counter_data_corrupt = 1;
                else
                    inc_counter_data_timeout = 1;
            end
        end
        
        `SM_DONE: begin
            if (packet_counter == 4'd3) begin
                // Done! Successfully captured all 4 data packets
                next_state = `SM_IDLE;
                assert_done = 1;

                // don't forget to assert that the final packet is valid too!
                assert_data_valid = 1;
            end
            else if (packet_counter >= 4'd0 & packet_counter <= 4'd2) begin
                // assert that this non-final packet is valid
                assert_data_valid = 1;

                // Still more data to rx
                inc_packet_counter = 1;
                uart_start_rx = 1;
                next_state = `SM_WAIT_FOR_HEADER;
            end
            else begin
                // Packet counter did something weird, abort all data
                clr_all_data_valid = 1;
                assert_done = 1;
                next_state = `SM_IDLE;
            end
        end
    endcase
end
    
endmodule