`timescale 1 ns / 1 ps

module adc_uart_rx(
    input wire clk,
    input wire rst_n,
    
    // Flag which is triggered for 1 clock cycle when a transmission is requested from the AMDS
    input wire start_rx,
    
    // The data line coming from the AMDS
    input wire din,
    input wire [11:0] is_dout_enabled,
    
    output wire [11:0] is_dout_valid, // is_dout_valid[0] == 1 implies that adc_dout0 is valid
    output reg adc_uart_done,
    output reg assert_done,
    output reg [15:0] adc_dout0,
    output reg [15:0] adc_dout1,
    output reg [15:0] adc_dout2,
    output reg [15:0] adc_dout3,
    output reg [15:0] adc_dout4,
    output reg [15:0] adc_dout5,
    output reg [15:0] adc_dout6,
    output reg [15:0] adc_dout7,
    output reg [15:0] adc_dout8,
    output reg [15:0] adc_dout9,
    output reg [15:0] adc_dout10,
    output reg [15:0] adc_dout11,
    
    output reg [15:0] counter_bytes_valid,
    output reg [15:0] counter_bytes_corrupt,
    output reg [15:0] counter_bytes_timed_out
);

// ==============
// Packet Valid and Current Packet
// ==============

reg rst_packet_valid;
reg set_packet_valid;
reg rst_current_packet;
reg load_current_packet;

reg [11:0] packet_valid;
reg [3:0] current_packet;

always @(posedge clk, negedge rst_n) begin
    if (!rst_n) 
        current_packet <= 15;
    else if (rst_current_packet) 
        current_packet <= 15;
    else if (load_current_packet)
        current_packet <= uart_data_byte[3:0];
end

always @(posedge clk, negedge rst_n) begin
    if (!rst_n)
        packet_valid <= 12'b0;
    else if (rst_packet_valid)
        packet_valid <= 12'b0;
    else if (set_packet_valid)
        packet_valid[current_packet] <= 1'b1;
end

// =======================
// S/R flops: is_doutN_valid
// =======================

reg is_dout0_valid, is_dout1_valid, is_dout2_valid, is_dout3_valid;

reg is_dout4_valid, is_dout5_valid, is_dout6_valid, is_dout7_valid;
reg is_dout8_valid, is_dout9_valid, is_dout10_valid, is_dout11_valid;

reg assert_data_valid, clr_all_data_valid;

always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        is_dout0_valid <= 1'b0;
    else if (clr_all_data_valid)
        is_dout0_valid <= 1'b0;
    else if (assert_data_valid & (current_packet == 0) & is_dout_enabled[0])
        is_dout0_valid <= 1'b1;
end

always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        is_dout1_valid <= 1'b0;
    else if (clr_all_data_valid)
        is_dout1_valid <= 1'b0;
    else if (assert_data_valid & (current_packet == 1) & is_dout_enabled[1])
        is_dout1_valid <= 1'b1;
end

always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        is_dout2_valid <= 1'b0;
    else if (clr_all_data_valid)
        is_dout2_valid <= 1'b0;
    else if (assert_data_valid & (current_packet == 2) & is_dout_enabled[2])
        is_dout2_valid <= 1'b1;
end

always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        is_dout3_valid <= 1'b0;
    else if (clr_all_data_valid)
        is_dout3_valid <= 1'b0;
    else if (assert_data_valid & (current_packet == 3) & is_dout_enabled[3])
        is_dout3_valid <= 1'b1;
end

always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        is_dout4_valid <= 1'b0;
    else if (clr_all_data_valid)
        is_dout4_valid <= 1'b0;
    else if (assert_data_valid & (current_packet == 4) & is_dout_enabled[4])
        is_dout4_valid <= 1'b1;
end

always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        is_dout5_valid <= 1'b0;
    else if (clr_all_data_valid)
        is_dout5_valid <= 1'b0;
    else if (assert_data_valid & (current_packet == 5) & is_dout_enabled[5])
        is_dout5_valid <= 1'b1;
end

always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        is_dout6_valid <= 1'b0;
    else if (clr_all_data_valid)
        is_dout6_valid <= 1'b0;
    else if (assert_data_valid & (current_packet == 6) & is_dout_enabled[6])
        is_dout6_valid <= 1'b1;
end

always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        is_dout7_valid <= 1'b0;
    else if (clr_all_data_valid)
        is_dout7_valid <= 1'b0;
    else if (assert_data_valid & (current_packet == 7) & is_dout_enabled[7])
        is_dout7_valid <= 1'b1;
end

always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        is_dout8_valid <= 1'b0;
    else if (clr_all_data_valid)
        is_dout8_valid <= 1'b0;
    else if (assert_data_valid & (current_packet == 8) & is_dout_enabled[8])
        is_dout8_valid <= 1'b1;
end

always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        is_dout9_valid <= 1'b0;
    else if (clr_all_data_valid)
        is_dout9_valid <= 1'b0;
    else if (assert_data_valid & (current_packet == 9) & is_dout_enabled[9])
        is_dout9_valid <= 1'b1;
end

always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        is_dout10_valid <= 1'b0;
    else if (clr_all_data_valid)
        is_dout10_valid <= 1'b0;
    else if (assert_data_valid & (current_packet == 10) & is_dout_enabled[10])
        is_dout10_valid <= 1'b1;
end

always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        is_dout11_valid <= 1'b0;
    else if (clr_all_data_valid)
        is_dout11_valid <= 1'b0;
    else if (assert_data_valid & (current_packet == 11) & is_dout_enabled[11])
        is_dout11_valid <= 1'b1;
end

// Concatenate the individual valid registers into output bus
assign is_dout_valid = {
    is_dout11_valid,
    is_dout10_valid,
    is_dout9_valid,
    is_dout8_valid,
    is_dout7_valid,
    is_dout6_valid,
    is_dout5_valid,
    is_dout4_valid,
    is_dout3_valid,
    is_dout2_valid,
    is_dout1_valid,
    is_dout0_valid
};


// ===================
// Valid Bytes Counter
// ===================

reg inc_counter_bytes_valid;
always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        counter_bytes_valid <= 16'b0;
    else if (inc_counter_bytes_valid)
        counter_bytes_valid <= counter_bytes_valid + 1;
end

// =====================
// Corrupt Bytes Counter
// =====================

reg inc_counter_bytes_corrupt;
always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        counter_bytes_corrupt <= 16'b0;
    else if (inc_counter_bytes_corrupt)
        counter_bytes_corrupt <= counter_bytes_corrupt + 1;
end

// =======================
// Timed-out Bytes Counter
// =======================

reg inc_counter_bytes_timed_out;
always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        counter_bytes_timed_out <= 16'b0;
    else if (inc_counter_bytes_timed_out)
        counter_bytes_timed_out <= counter_bytes_timed_out + 1;
end

// ==============
// Done
// ==============

reg deassert_done;

always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        // Start as done, so that the trigger does not freeze
        adc_uart_done <= 1'b1;
    else if (deassert_done)
        adc_uart_done <= 1'b0;
    else if (assert_done)
        // assert_done is declared above as a module output, 
        // because it also needs to set a done flop in the parent driver
        adc_uart_done <= 1'b1;
end

// ==============
// UART RX module
// ==============

wire uart_is_byte_valid;
wire uart_is_byte_corrupt;
wire uart_byte_timed_out;

reg uart_start_rx;
wire [7:0] uart_data_byte;

uart_rx iUART_RX(
    .clk(clk),
    .rst_n(rst_n),
    .din(din),
    .start_rx(uart_start_rx),
    .is_byte_valid(uart_is_byte_valid),
    .is_byte_corrupt(uart_is_byte_corrupt),
    .byte_timed_out(uart_byte_timed_out),
    .dout(uart_data_byte)
);

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
        adc_dout4 <= 16'b0;
        adc_dout5 <= 16'b0;
        adc_dout6 <= 16'b0;
        adc_dout7 <= 16'b0;
        adc_dout8 <= 16'b0;
        adc_dout9 <= 16'b0;
        adc_dout10 <= 16'b0;
        adc_dout11 <= 16'b0;
    end

    else if (load_doutN_LSB) begin
        case (current_packet)
            4'd0:  adc_dout0[7:0]  <= uart_data_byte;
            4'd1:  adc_dout1[7:0]  <= uart_data_byte;
            4'd2:  adc_dout2[7:0]  <= uart_data_byte;
            4'd3:  adc_dout3[7:0]  <= uart_data_byte;
            4'd4:  adc_dout4[7:0]  <= uart_data_byte;
            4'd5:  adc_dout5[7:0]  <= uart_data_byte;
            4'd6:  adc_dout6[7:0]  <= uart_data_byte;
            4'd7:  adc_dout7[7:0]  <= uart_data_byte;
            4'd8:  adc_dout8[7:0]  <= uart_data_byte;
            4'd9:  adc_dout9[7:0]  <= uart_data_byte;
            4'd10: adc_dout10[7:0] <= uart_data_byte;
            4'd11: adc_dout11[7:0] <= uart_data_byte;
        endcase
    end
    
    else if (load_doutN_MSB) begin
        case (current_packet)
            4'd0:  adc_dout0[15:8]  <= uart_data_byte;
            4'd1:  adc_dout1[15:8]  <= uart_data_byte;
            4'd2:  adc_dout2[15:8]  <= uart_data_byte;
            4'd3:  adc_dout3[15:8]  <= uart_data_byte;
            4'd4:  adc_dout4[15:8]  <= uart_data_byte;
            4'd5:  adc_dout5[15:8]  <= uart_data_byte;
            4'd6:  adc_dout6[15:8]  <= uart_data_byte;
            4'd7:  adc_dout7[15:8]  <= uart_data_byte;
            4'd8:  adc_dout8[15:8]  <= uart_data_byte;
            4'd9:  adc_dout9[15:8]  <= uart_data_byte;
            4'd10: adc_dout10[15:8] <= uart_data_byte;
            4'd11: adc_dout11[15:8] <= uart_data_byte;
        endcase
    end
end

// =====================
// State Machine Helpers
// =====================

// These are needed to help the state machine remember if the header and MSB were all good
// The state machine still has to go through every state (HEADER -> MSB -> LSB) for all four
// packets, even if (for example) the header was not what was expected (x81 instead of x91).
// But if we did have a problem with a header or MSB, that packet should not be marked valid
// at the end.
reg was_header_good, was_MSB_good, was_LSB_good; // flops
reg assert_header_good, assert_MSB_good, assert_LSB_good, clr_sm_helpers; // SM outputs

always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        was_header_good = 1'b0;
    else if (clr_sm_helpers)
        was_header_good = 1'b0;
    else if (assert_header_good)
        was_header_good = 1'b1;
end

always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        was_MSB_good = 1'b0;
    else if (clr_sm_helpers)
        was_MSB_good = 1'b0;
    else if (assert_MSB_good)
        was_MSB_good = 1'b1;
end

always @(posedge clk, negedge rst_n) begin
    if (~rst_n)
        was_LSB_good = 1'b0;
    else if (clr_sm_helpers)
        was_LSB_good = 1'b0;
    else if (assert_LSB_good)
        was_LSB_good = 1'b1;
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
    
    rst_packet_valid = 0;
    set_packet_valid = 0;
    rst_current_packet = 0;
    load_current_packet = 0;

    assert_done = 0;
    deassert_done = 0;
    
    assert_data_valid = 0;
    clr_all_data_valid = 0;

    assert_header_good = 0;
    assert_MSB_good = 0;
    assert_LSB_good = 0;
    clr_sm_helpers = 0;
    
    load_doutN_LSB = 0;
    load_doutN_MSB = 0;
    
    inc_counter_bytes_valid = 0;
    inc_counter_bytes_corrupt = 0;
    inc_counter_bytes_timed_out = 0;
    
    case (state)
        `SM_IDLE: begin
            if (|is_dout_enabled) begin
                if (start_rx) begin
                    // We have a trigger and at least one channel is enabled
                    clr_sm_helpers = 1;
                    
                    // Incoming UART transmission, so start UART RX module
                    uart_start_rx = 1;
                    
                    // Tell user that the all data regs are no longer valid
                    clr_all_data_valid = 1;
                    
                    deassert_done = 1;
            
                    rst_packet_valid = 1;
                    rst_current_packet = 1;
                    next_state = `SM_WAIT_FOR_HEADER;
                end
            end
            else begin
                // Trigger received, but the line is disabled. 
                // Immediately signal "done" so the AXI wrapper doesn't wait/timeout.
                assert_done = 1;
                next_state = `SM_IDLE;
            end
        end
        
        `SM_WAIT_FOR_HEADER: begin
            if (uart_is_byte_valid) begin
                // Check that the new header is valid:
                // * expect more-significant nibble to be 0x9
                // * expect packet number in less-significant nibble
                if (uart_data_byte[7:4] == 4'h9) begin
                    // Ideally we make it to here: parity check passed and header byte was what we expected it to be
                    assert_header_good = 1;
                    load_current_packet = 1;
                    inc_counter_bytes_valid = 1;
                end
                else begin
                    // Uh oh... parity check passed, but header was not what we expected
                    // We will NOT assert that the header was good, it will remain in its sad '0' state
                    // However, the AMDS is still sending more bytes (certainly data MSB and LSB, maybe even more packets),
                    // so we will keep going through the rest of this SM
                end
                next_state = `SM_WAIT_FOR_DATA_MSB;
                uart_start_rx = 1; // start the receiver again for the first byte of actual data
            end
            else if (uart_is_byte_corrupt) begin
                // Uh oh... parity check for header failed, increment the corrupt counter
                // again, we still need to proceed through the full SM
                inc_counter_bytes_corrupt = 1;
                next_state = `SM_WAIT_FOR_DATA_MSB;
                uart_start_rx = 1; // start the receiver again so that we keep getting the UART receiver results
            end
            else if (uart_byte_timed_out) begin
                // Uh oh... never received start bit for header byte
                // In the event of timeout, we really should just give up 
                // and return to IDLE... there's probably no data coming anyway
                inc_counter_bytes_timed_out = 1;
                assert_done = 1;
                next_state = `SM_IDLE;
            end
        end
        
        `SM_WAIT_FOR_DATA_MSB: begin
            if (uart_is_byte_valid) begin
                // Hooray! MSB passed the parity check! We will capture the data and assert everything here was valid!
                inc_counter_bytes_valid = 1;
                load_doutN_MSB = 1;
                assert_MSB_good = 1;
                next_state = `SM_WAIT_FOR_DATA_LSB;
                uart_start_rx = 1;
            end
            else if (uart_is_byte_corrupt) begin
                // Uh oh... parity check for MSB failed, increment the corrupt counter
                // do NOT assert the MSB was good
                // again, we still need to proceed through the full SM
                inc_counter_bytes_corrupt = 1;
                next_state = `SM_WAIT_FOR_DATA_LSB;
                uart_start_rx = 1; // start the receiver again so that we keep getting the UART receiver results
            end
            else if (uart_byte_timed_out) begin
                // Uh oh... never received start bit for MSB
                // In the event of timeout, we really should just give up 
                // and return to IDLE... there's probably no data coming anyway
                inc_counter_bytes_timed_out = 1;
                assert_done = 1;
                next_state = `SM_IDLE;
            end
        end
        
        `SM_WAIT_FOR_DATA_LSB: begin
            if (uart_is_byte_valid) begin
                // Hooray! LSB passed the parity check! We will capture the data and assert everything here was valid!
                inc_counter_bytes_valid = 1;
                load_doutN_LSB = 1;
                assert_LSB_good = 1;
                next_state = `SM_DONE;
            end
            else if (uart_is_byte_corrupt) begin
                // Uh oh... parity check for LSB failed, increment the corrupt counter
                // again, we still need to proceed through the full SM
                inc_counter_bytes_corrupt = 1;
                next_state = `SM_DONE;
            end
            else if (uart_byte_timed_out) begin
                // Uh oh... never received start bit for LSB
                // In the event of timeout, we really should just give up 
                // and return to IDLE... there's probably no data coming anyway
                inc_counter_bytes_timed_out = 1;
                assert_done = 1;
                next_state = `SM_IDLE;
            end
        end
        
        `SM_DONE: begin
            // The DONE state is reached every time a full packet (HEADER -> MSB -> LSB) is complete
            // This means that we will go back to WAIT_HEADER if we are expecting more packets,
            // or back to IDLE if the packet counter is done

            if (was_header_good & was_MSB_good & was_LSB_good) begin
                // If all three bytes of the packet were good, we can assert that this packet is valid
                // This signal is ANDed with the value of the packet counter to make sure the *correct*
                // packet is marked valid 
                assert_data_valid = 1;
                set_packet_valid = 1;
            end

            if (&((packet_valid | (1 << current_packet)) | ~is_dout_enabled)) begin
                next_state = `SM_IDLE;
                assert_done = 1;
                clr_sm_helpers = 1;
            end
            
            else begin
                // If there are more enabled channels, there is still more data to rx
                uart_start_rx = 1;
                next_state = `SM_WAIT_FOR_HEADER;
                clr_sm_helpers = 1;
            end
        end
    endcase
end
    
endmodule
