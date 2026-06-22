`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 06/22/2026 01:20:35 PM
// Design Name: 
// Module Name: axi_tb
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module axi_tb();
    reg [7:0] currently_broadcasting;
    reg examine_line_x = 0;
	
	reg clk = 0;
	always #1 clk = ~clk;
	reg rst_n = 1'b1;
	reg [1:0] uart_data = 2'b11;
	reg [11:0] is_dout_enabled = 12'b111111111111;
	 wire axi_driver_done;
	 reg trigger_axi_read = 0;
	 
	  wire[15:0] adc_data[23:0];

        amdc_amds_v1_0_S00_AXI axi_module(
        .S_AXI_ACLK(clk),
        .S_AXI_ARESETN(rst_n),
        .enable(1'b1),
        .amds_data(uart_data),
        .done(axi_driver_done),
        .trigger(trigger_axi_read)
        );
    
    assign adc_data[0] = axi_module.my_adc_data0;
    assign adc_data[1] = axi_module.my_adc_data1;
    assign adc_data[2] = axi_module.my_adc_data2;
    assign adc_data[3] = axi_module.my_adc_data3;
    assign adc_data[4] = axi_module.my_adc_data4;
    assign adc_data[5] = axi_module.my_adc_data5;
    assign adc_data[6] = axi_module.my_adc_data6;
    assign adc_data[7] = axi_module.my_adc_data7;
    assign adc_data[8] = axi_module.my_adc_data8;
    assign adc_data[9] = axi_module.my_adc_data9;
    assign adc_data[10] = axi_module.my_adc_data10;
    assign adc_data[11] = axi_module.my_adc_data11;
    assign adc_data[12] = axi_module.my_adc_data12;
    assign adc_data[13] = axi_module.my_adc_data13;
    assign adc_data[14] = axi_module.my_adc_data14;
    assign adc_data[15] = axi_module.my_adc_data15;
    assign adc_data[16] = axi_module.my_adc_data16;
    assign adc_data[17] = axi_module.my_adc_data17;
    assign adc_data[18] = axi_module.my_adc_data18;
    assign adc_data[19] = axi_module.my_adc_data19;
    assign adc_data[20] = axi_module.my_adc_data20;
    assign adc_data[21] = axi_module.my_adc_data21;
    assign adc_data[22] = axi_module.my_adc_data22;
    assign adc_data[23] = axi_module.my_adc_data23;
    
    initial begin 
    #2;
    rst_n <= 1;
    #2;
    rst_n <= 0;
    #2;
    rst_n <= 1;
    #18;
    assign_axi_enabled(is_dout_enabled);
    #3000;
    auto_start_rx();
    #3000;
    send_byte(8'b11111111, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b11111111, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b11111111, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b11111111, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b11111111, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b11111111, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b11111111, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b11111111, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b11111111, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b11111111, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b11111111, 0);
    send_byte(8'b11111111, 0);
    
    send_byte(8'b11111111, 0);
    send_byte(8'b11111111, 0);
    
    $finish;
    end
    
    task automatic auto_start_rx(); 
        #2;
        trigger_axi_read <= 1;
        #2;
        trigger_axi_read <= 0;
    endtask
    
    task automatic send_byte(input reg[7:0] data, input bit use_invalid_parity);
    currently_broadcasting <= data;
    // start bit
    uart_data[examine_line_x] <= 1'b0;
    #18;
    // data bits
    for(int i = 0; i < 8; i = i + 1) begin
        uart_data[examine_line_x] <= data[i];
        #18;
    end
    // parity bit (odd parity)
    uart_data[examine_line_x] <= (!(^data)) ^ use_invalid_parity;
    #18;
    // end transmission.
    uart_data[examine_line_x] <= 1'b1;
    #2;
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
endmodule
