`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: Eric Severson's E-Lev Laboratory
// Engineer: Harley Peterson
// 
// Create Date: 05/30/2026 04:36:45 PM
// Design Name: TestBench for UART modules
// Module Name: amdc_amds_uart_v1_0_tb
// Target Devices: PicoZed7030
// Description: This file is designed to test UART compliance within the AMDC UART receiver.
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module amdc_amds_uart_v1_0_tb();
    reg [7:0] currently_broadcasting;
    reg examine_line_x = 0;
	
	reg clk = 0;
	reg rst_n = 1'b1;
	reg [1:0] uart_data = 2'b11;
	wire is_byte_valid;
	wire is_byte_corrupt;
	wire shift_reg_valid;
	wire [8:0] rx_data;

    reg [11:0] is_dout_enabled = 12'b000000000000;
    // sync adc signal, used to reset data validity.
    reg sync_adc = 1'b0;
    
    wire [11:0] is_dout_valid; // is_dout_valid[0] == 1 implies that adc_dout0 is valid
    wire adc_uart_done;
    
    wire [15:0] adc_dout[11:0];
    
    wire [15:0] dout_x0;
    wire [15:0] dout_x1;
    wire [15:0] dout_x2;
    wire [15:0] dout_x3;
    wire [15:0] dout_x4;
    wire [15:0] dout_x5;
    wire [15:0] dout_x6;
    wire [15:0] dout_x7;
    wire [15:0] dout_x8;
    wire [15:0] dout_x9;
    wire [15:0] dout_x10;
    wire [15:0] dout_x11;
    
    wire [15:0] counter_bytes_valid;
    wire [15:0] counter_bytes_corrupt;
    wire [15:0] counter_bytes_timed_out;
    // Stores the state machine state of adc_uart_rx;
    wire [2:0] adc_state;
    
    // We want to stop validating when timeouts occur.
    reg stop_validating_data = 1'b0;
    // Keep track of what adc we *think* we are trying to read.
    integer selected_index;
    
    // Tell the AXI driver to poll the AMDS
    reg trigger_axi_read = 1'b0;
    // The signal has made it out of the FPGA!!!
    wire axi_driver_done;
    
    wire adc_uart0_done;
    wire adc_uart1_done;
    wire done_0;
    wire done_1;
    
    reg start_rx;
    
    
    uart_rx bit_receiver(
    .clk(clk),
    .rst_n(rst_n),
    .start_rx(start_rx),
    .din(uart_data[examine_line_x]),
    .is_byte_valid(is_byte_valid),
    .is_byte_corrupt(is_byte_corrupt),
    .dout(rx_data)
    );
    
    adc_uart_rx packet_decoder(
     .clk(clk),
     .rst_n(rst_n),
    // The data line coming from the AMDS
     .din(uart_data[examine_line_x]),
     .is_dout_enabled(is_dout_enabled),
    // sync adc signal, used to reset data validity.
     .start_rx(sync_adc),
     .is_dout_valid(is_dout_valid), // is_dout_valid[0] == 1 implies that adc_dout0 is valid
     .adc_uart_done(adc_uart_done),
     
     .adc_dout0(dout_x0),
     .adc_dout1(dout_x1),
     .adc_dout2(dout_x2),
     .adc_dout3(dout_x3),
     .adc_dout4(dout_x4),
     .adc_dout5(dout_x5),
     .adc_dout6(dout_x6),
     .adc_dout7(dout_x7),
     .adc_dout8(dout_x8),
     .adc_dout9(dout_x9),
     .adc_dout10(dout_x10),
     .adc_dout11(dout_x11),
    
    .counter_bytes_valid(counter_bytes_valid),
    .counter_bytes_corrupt(counter_bytes_corrupt),
    .counter_bytes_timed_out(counter_bytes_timed_out)
);

    amdc_amds_v1_0_S00_AXI axi_module(
        .S_AXI_ACLK(clk),
        .S_AXI_ARESETN(rst_n),
        .enable(1'b1),
        .amds_data(uart_data),
        .done(axi_driver_done),
        .trigger(trigger_axi_read)
        );
       
    
    assign rx_data = bit_receiver.shift_reg;
    assign adc_state = packet_decoder.state;
    assign shift_reg_valid = ^bit_receiver.shift_reg[8:0];
    assign selected_index = packet_decoder.current_packet;
    assign done_0 = axi_module.done_0;
    assign done_1 = axi_module.done_1;
    assign adc_uart0_done = axi_module.adc_uart0_done;
    assign adc_uart1_done = axi_module.adc_uart1_done;
    
    assign adc_dout[0] = dout_x0;
    assign adc_dout[1] = dout_x1;
    assign adc_dout[2] = dout_x2;
    assign adc_dout[3] = dout_x3;
    assign adc_dout[4] = dout_x4;
    assign adc_dout[5] = dout_x5;
    assign adc_dout[6] = dout_x6;
    assign adc_dout[7] = dout_x7;
    assign adc_dout[8] = dout_x8;
    assign adc_dout[9] = dout_x9;
    assign adc_dout[10] = dout_x10;
    assign adc_dout[11] = dout_x11;
    
    reg disabled_channel = 0;
    
initial begin
    automatic bit timeout_data = 0;
    automatic bit timeout_header = 0;
    automatic bit corrupt_data = 0;
    automatic bit corrupt_header = 0;
    assign_axi_enabled(12'b0);
    examine_line_x = ~examine_line_x;
    assign_axi_enabled(12'b0);
    examine_line_x = ~examine_line_x;
    start_rx <= 0;
    #2;
    rst_n <= 1'b0;
    #2;
    rst_n <= 1'b1;
    #2;
//    sync_adc <= 1'b1;
//    #5;
//    sync_adc <= 1'b0;
//    #5;
//    send_byte(12'b10000000, 0);
//    $finish;
    
    for (int i = 0; i < 5; i = i + 1) begin
    $display("      valid data");
        timeout_data = 0;
        timeout_header = 0;
        corrupt_data = 0;
        corrupt_header = 0;
        test_data_done_timing(timeout_data, timeout_header, corrupt_data, corrupt_header);
        #4;
    end
    //$stop;
    for (int i = 0; i < 5; i = i + 1) begin
    $display("      corrupt data");
         timeout_data = 0;
         timeout_header = 0;
         corrupt_data = 1;
         corrupt_header = 0;
         test_data_done_timing(timeout_data, timeout_header, corrupt_data, corrupt_header);
         #4;
     end
     //$stop;
     for (int i = 0; i < 5; i = i + 1) begin
     $display("     timeout data");
         timeout_data = 1;
         timeout_header = 0;
         corrupt_data = 0;
         corrupt_header = 0;
         test_data_done_timing(timeout_data, timeout_header, corrupt_data, corrupt_header);
         #4;
    end
    //$stop;
    for (int i = 0; i < 5; i = i + 1) begin
    $display("      corrupt header");
         timeout_data = 0;
         timeout_header = 0;
         corrupt_data = 0;
         corrupt_header = 1;
         test_data_done_timing(timeout_data, timeout_header, corrupt_data, corrupt_header);
         #4;
    end
    //$stop;
    for (int i = 0; i < 5; i = i + 1) begin
    $display("      timeout header");
         timeout_data = 0;
         timeout_header = 1;
         corrupt_data = 0;
         corrupt_header = 0;
         test_data_done_timing(timeout_data, timeout_header, corrupt_data, corrupt_header);
         #4;
    end
    #10;
    $finish;
end

always #1 clk = ~clk;

task automatic test_data_done_timing(input bit timeout_data, input bit timeout_header, input bit corrupt_data, input bit corrupt_header);
    reg [31:0] cards;
    reg [11:0] enabled;
    // Some sensors enabled
    cards = $random;
    $display("Random sensor cards");
    test_sensor_cards(cards[11:0],  timeout_data,timeout_header,corrupt_data,corrupt_header);
    // All sensors enabled.
    $display("All sensor cards");
    test_sensor_cards(12'b111111111111, timeout_data,timeout_header,corrupt_data,corrupt_header);
    // One sensor enabled.
    enabled = 12'b0;
    enabled[$random % 12] = 1'b1;
    $display("Singular sensor card");
    test_sensor_cards(enabled,  timeout_data,timeout_header,corrupt_data,corrupt_header);
    // No sensors enabled.
    $display("No sensor cards");
    test_sensor_cards(12'b000000000000, timeout_data,timeout_header,corrupt_data,corrupt_header);
endtask

task automatic test_sensor_cards(input reg[11:0] enabled_channels, input bit timeout_data, input bit timeout_header, input bit corrupt_data, input bit corrupt_header);
    reg [31:0] random1 = $random;// timeouts for foward pass
    reg [31:0] random2 = $random;// timeouts for backwards pass
    reg [31:0] random3 = $random;// corrupts for foward pass
    reg [31:0] random4 = $random;// corrupts for backwards pass
    reg random_timeout_data;
    reg random_timeout_header;
    reg random_corrupt_data;
    reg random_corrupt_header;
    is_dout_enabled <= enabled_channels;
    assign_axi_enabled(enabled_channels);
    #2;
    // foward pass.
    sync_adc = ~sync_adc;
    trigger_axi_read <= 1'b1;
    #2;
    sync_adc = ~sync_adc;
    trigger_axi_read <= 1'b0;
    #800;
    stop_validating_data = 1'b0;
    for (reg[4:0] i = 0; i < 12; i = i + 1) begin
        if (enabled_channels[i] | 1) begin
            disabled_channel = ~enabled_channels[i];
            assert ((!adc_uart_done) | stop_validating_data | (!enabled_channels[i])) else $error("Transmission marked as done before data transmission finished");
            assert ((!axi_driver_done) | stop_validating_data | (!enabled_channels[i])) else $error("Axi driver asserted done before transmission complete");
            random_timeout_data = random1[(i * 2)] & timeout_data;
            random_timeout_header = random1[(i * 2) + 1] & timeout_header;
            random_corrupt_data = random3[(i * 2)] & corrupt_data;
			random_corrupt_header = random3[(i * 2) + 1] & corrupt_header;
            if (random_timeout_data | random_timeout_header) begin
                stop_validating_data = 1'b1;
            end
            send_rand_packet(i, random_timeout_data, random_timeout_header, random_corrupt_data, random_corrupt_header);
            #4;
        end
    end
    if (corrupt_header) begin
        #4098; // corrupt headers cause packet timeouts
    end
    #20;
    assert (adc_uart_done) else begin
     if(corrupt_data) begin
            //$warning("Missing optimization: track valid headers and assert done when all are seen, even if data is invalid");
            #4098;
            assert (adc_uart_done) else $error("Transmission complete, but done was not asserted!");
     end else begin
     $error("Transmission complete, but done was not asserted!");
     $stop;
    end
    end
    //assert (axi_driver_done) else $error("Axi driver failed to assert done");
    // backward pass.
    sync_adc = ~sync_adc;
    trigger_axi_read <= 1'b1;
    #2;
    sync_adc = ~sync_adc;
    trigger_axi_read <= 1'b0;
    #800;
    stop_validating_data = 1'b0;
    for (reg[4:0] i = 12; i > 0; i = i - 1) begin
        if (enabled_channels[i - 1] | 1) begin
            disabled_channel = ~enabled_channels[i - 1];
            assert ((!adc_uart_done) | stop_validating_data | (!enabled_channels[i - 1])) else $error("Transmission marked as done before data transmission finished");
            assert ((!axi_driver_done) | stop_validating_data | (!enabled_channels[i - 1])) else $error("Axi driver asserted done before transmission complete");
            random_timeout_data = random2[(i * 2)] & timeout_data;
            random_timeout_header = random2[(i * 2) + 1] & timeout_header;
            random_corrupt_data = random3[(i * 2)] & corrupt_data;
			random_corrupt_header = random3[(i * 2) + 1] & corrupt_header;
            if (random_timeout_data | random_timeout_header) begin
                stop_validating_data = 1'b1;
            end
            send_rand_packet(i - 1, random_timeout_data, random_timeout_header, random_corrupt_data, random_corrupt_header);
            #4;
        end
    end
    if (corrupt_header) begin
        #4098; // corrupt headers cause packet timeouts
    end
    #20;
    assert (adc_uart_done) else begin
     if(corrupt_data) begin
            //$warning("Missing optimization: track valid headers and assert done when all are seen, even if data is invalid");
            #4098;
            assert (adc_uart_done) else $error("Transmission complete, but done was not asserted!");
     end else begin
     $error("Transmission complete, but done was not asserted!");
     $stop;
    end
    end
    //assert (axi_driver_done) else $error("Axi driver failed to assert done");
endtask

task automatic assign_axi_enabled(input reg[11:0] enabled_channels);
    // assign is_dout0_enabled = {slv_reg30[19:16], slv_reg30[11:8], slv_reg30[3:0]};
    // assign is_dout1_enabled = {slv_reg30[23:20], slv_reg30[15:12], slv_reg30[7:4]};
    if (examine_line_x == 0) begin
        axi_module.slv_reg30[19:16] = enabled_channels[11:8];
        axi_module.slv_reg30[11:8] = enabled_channels[7:4];
        axi_module.slv_reg30[3:0] = enabled_channels[3:0];
    end else begin
        axi_module.slv_reg30[23:20] = enabled_channels[11:8];
        axi_module.slv_reg30[15:12] = enabled_channels[7:4];
        axi_module.slv_reg30[7:4] = enabled_channels[3:0];
    end
endtask

task automatic send_rand_packet(input reg[4:0] channel, input bit timeout_data, input bit timeout_header, input bit corrupt_data, input bit corrupt_header);
    reg [7:0] header = 8'b10010000 + channel;
    reg [31:0] random = $random;
    reg msb_corrupt = random[31];
    reg msb_timeout = random[30];
    if (timeout_header) begin
        #4096; // I think this is long enough to trigger a timeout.
        assert (adc_uart_done) else $error("Header timed out, but done not asserted");
    end else
        send_byte(header, corrupt_header);
        #20;
    if (msb_timeout & timeout_data) begin
        #4096;
        assert (adc_uart_done) else $error("MSB timed out, but done not asserted");
    end else
        send_byte(random[15:8], msb_corrupt & corrupt_data);
        #20;
    if (!msb_timeout & timeout_data) begin
        #4096;
        assert (adc_uart_done) else $error("LSB timed out, but done not asserted");
    end else
        send_byte(random[7:0], (!msb_corrupt) & corrupt_data);
    
    #20;
    
    if (((timeout_data | timeout_header) | (corrupt_data | corrupt_header)) == 0) begin
        assert ((adc_dout[channel] == random[15:0]) | stop_validating_data | disabled_channel) else $error("Valid data received, but register remains unpopulated.");
        assert (is_dout_valid[channel] == 1'b1 | stop_validating_data | disabled_channel) else $error("Valid data marked as Invalid");
    end else begin 
        assert ((is_dout_valid[channel] == 1'b0) | (&(~is_dout_enabled))) else begin
            $error("Invalid data marked as Valid");
            $display("Channel was %0h", channel);
            $display("Timeout data %0b Timeout header %0b Corrupt data %0b Corrupt header %0b MSB corrupt %0b MSB timeout %0b", timeout_data, timeout_header, corrupt_data, corrupt_header, msb_corrupt, msb_timeout);
            $stop;
        end
    end
    
    if (timeout_data | timeout_header) begin
        assert (adc_uart_done) else $error("Data timed out, but done not asserted");
    end
    
    if ((!timeout_header) & (!corrupt_header)) begin
        assert ((packet_decoder.is_dout_valid[channel] == 1'b1) | stop_validating_data | disabled_channel) else begin
        if(corrupt_data) begin
         // $warning("Missing optimization: Track valid headers even though data is invalid");
        end else begin
         $error("Valid header not recognized.");
         $stop;
        end
        end
    end

endtask 

task automatic send_byte(input reg[7:0] data, input bit use_invalid_parity);
    start_rx <= 1;
    #2;
    start_rx <= 0;
    #20;
    currently_broadcasting <= data;
    // start bit
    uart_data[examine_line_x] <= 1'b0;
    #20;
    // data bits
    for(int i = 0; i < 8; i = i + 1) begin
        uart_data[examine_line_x] <= data[i];
        #20;
    end
    // parity bit (odd parity)
    uart_data[examine_line_x] <= (!(^data)) ^ use_invalid_parity;
    #20;
    // end transmission.
    uart_data[examine_line_x] <= 1'b1;
    #20;
    // Test to make sure the data was received, and that the uart module
    // either saw it as valid, or invalid. If both are low, no data was seen.
    assert ((is_byte_valid != is_byte_corrupt) | (&(~is_dout_enabled)) | stop_validating_data) else begin
    $error("Data cannot be both valid & invalid.");
    $stop;
    end
    // Ensure that the parity was validated correctly.
    assert (is_byte_corrupt == use_invalid_parity | (&(~is_dout_enabled)) | stop_validating_data) else begin 
    $error("Valid data failed parity check or vice versa.");
    $stop;
    end
endtask 

endmodule
