    // Rising edge of cnv triggers conversion
    // Previous conversion data is available to read after the  
    // cnv rising edge
    // cnv must be high for TQUIET1 after cnv is brought high before bringing low
    // must wait TQUIET2 after last SCLK low until cnv is brought high
    // capturing on falling edge is best

    // TQUIET 1 is 190 ns min
    // TQUIET 2 is 60 ns min

module amdc_spi_master(clk, rst_n, trig, miso_x, miso_y, sclk, cnv, sensor_data_x, sensor_data_y, data_ready);

    input clk, rst_n, trig;
    input miso_x, miso_y;

    output reg [17:0] sensor_data_x, sensor_data_y;
    output reg sclk, cnv;
    output reg data_ready;

    localparam CNV = 2'b00;
    localparam RX = 2'b01;
    localparam HOLD = 2'b10;

    localparam TQUIET_1 = 6'h36;  // Minimum CNV hold time
    localparam TQUIET_2 = 6'h36;  // Minimum time between last SCLK and new conversion
    localparam LAST_BIT = 5'd18;


    reg [3:0] clk_div, clk_div_clr;
    reg [4:0] bit_cntr;
    reg [5:0] cnv_cntr;

    reg [1:0] state, next_state;

    reg clk_div_f;
    reg cnv_cntr_f, cnv_cntr_clr;
    reg shift_f;

    // Conversion wait time counter
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) cnv_cntr <= 6'h00;
        else if(trig) begin
          if (cnv_cntr_clr) cnv_cntr <= 6'h00;
          else if (~&cnv_cntr && cnv_cntr_f) cnv_cntr <= cnv_cntr + 1'b1;
        end
    end

    // Clock divider counter
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) clk_div <= 4'h0;
        else if(trig) begin
          if(clk_div_clr) clk_div <= 4'h0;
          else if(clk_div_f) clk_div <= clk_div + 1'b1;
        end
    end

    // Bit counter
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) bit_cntr <= 5'h00;
        else if(trig) begin
          if (clk_div_clr) bit_cntr <= 5'h00;
          else if (shift_f) bit_cntr <= bit_cntr + 1'b1;   
        end
    end

    // Sensor data shifter
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) begin
            sensor_data_x <= 18'h00000;
            sensor_data_y <= 18'h00000;
        end
        else if(trig) begin
          if (shift_f) begin
              sensor_data_x <= {sensor_data_x[16:0], miso_x};
              sensor_data_y <= {sensor_data_y[16:0], miso_y};
          end
        end
    end

    // SCLK FF
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) sclk <= 1'b0;
        else if(trig) begin
          if (clk_div_f) sclk <= clk_div >= 4'h8 ? 1'b1 : 1'b0;
        end
    end

    // State FF
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) state <= CNV;
        else if(trig) begin
          state <= next_state;
        end
    end
    
    always @(*) begin
        cnv = 1'b0;
        cnv_cntr_f = 1'b0;
        cnv_cntr_clr = 1'b0;

        clk_div_f = 1'b0;
        clk_div_clr = 1'b0;
        shift_f = 1'b0;

        next_state = CNV;
        data_ready = 1'b0;

        case(state)
            CNV: begin
                // CNV is held high while data is ready to be read
                cnv = 1'b1;
                data_ready = 1'b1;

                // CNV counter is running until TQUIET1 time has passed
                cnv_cntr_f = 1'b1;

                if(cnv_cntr >= TQUIET_1) begin
                    // Reset cnv_cntr and begin receiving data
                    cnv_cntr_clr = 1'b1;
                    next_state = RX;
                end
            end
            RX: begin
                // SCLK start low and begins running
                clk_div_f = 1'b1;

                // Shift MSB left on falling edge of SCLK
                if(~|clk_div) shift_f = 1'b1;

                // Move to HOLD if the LSB was shifted in
                if(bit_cntr > LAST_BIT) begin
                    // Clear divider for next receive
                    clk_div_clr = 1'b1;
                    next_state = HOLD;
                end
                else next_state = RX;
            end
            HOLD: begin
                // CNV counter is running until TQUIET2 time has passed
                cnv_cntr_f = 1'b1;

                if(cnv_cntr < TQUIET_2) next_state = HOLD;
                else cnv_cntr_clr = 1'b1;
            end
            default: next_state = CNV;
        endcase
    end

endmodule