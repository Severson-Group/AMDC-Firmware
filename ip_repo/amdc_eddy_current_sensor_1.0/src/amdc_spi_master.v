`default_nettype none 

module amdc_spi_master(
    // INPUTS
    clk, rst_n, 
    start, 
    miso_x, miso_y, 
    sclk_cnt,
    
    // OUTPUTS
    sclk, cnv, 
    sensor_data_x, sensor_data_y, 
    done
    );

    ///////////////////////////////////////////////
    //
    //  SPI Driver for AD4011 ADC in Kaman Eddy Current Sensor
    //
    ///////////////////////////////////////////

    /////////////////////////
    // PARAMETERS
    //////////////////////
    localparam AXI_CLK_FREQ = 8'd200; // MEGAHERTZ
    localparam AXI_CLK_PERIOD = 8'd5; // NANOSECONDS

    // For 10MHz, SCLK needs to toggle every 50ns, so every 10 AXI CLK periods (50 / AXI_CLK_PERIOD)
    // This is moved to an input that is configurable by the C driver (default is 10)
    //localparam sclk_cnt = 8'd10;

    // We need to give the ADC 320ns to handle conversion, or 64 AXI CLK periods (320 / AXI_CLK_PERIOD)
    localparam cnv_cnt = 8'd64;
    

    ///////////////////////
    // INPUTS
    /////////////////////
    input wire clk, rst_n;
    input wire start;
    input wire miso_x, miso_y;
    input wire [7:0] sclk_cnt;


    ///////////////////////
    // OUTPUTS
    /////////////////////
    output reg sclk;
    output reg cnv;
    output reg [17:0] sensor_data_x, sensor_data_y;
    output reg done;


    ///////////////////////////////////////////
    // Internal regs and control signals
    ////////////////////////////////////////
    reg [7:0] sclk_div;
    reg miso_x_1, miso_x_2;
    reg miso_y_1, miso_y_2;
    reg [4:0] bit_cnt;
    reg [7:0] cnv_div;

    reg clr_cnv;
    wire cnv_cmplt;
    reg clr_sclk;
    reg set_done, clr_done;



    // CNV TIMER
    //   According to the AD4011 ADC's datasheet, the CoNVersion phase is initiated by setting and holding high the 'cnv' line for as much as 320ns
    //   This is done during the CNV state, and because the ADC does not send a signal when the CoNVersion is complete, we have to have a timer for this state
    //   and create our own SM input for when we want to move from the CNV to RX state
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n)
            cnv_div <= 8'b0;
        else if(clr_cnv)                    // From SM: in all states except CNV, we should not run up cnv_div 
            cnv_div <= 8'b0;
        else                                // Else keep running up cnv_div
            cnv_div <= cnv_div + 1;
    end

    assign cnv_cmplt = (cnv_div == cnv_cnt); // SM input



    // SCLK GENERATION
    //   Kaman has the AD4011 ADC connected to VIO = 3.3V, therefore the minimum SCLK period is 9.8ns, so I'll use 10ns (5ns low/5ns high) or SLCK_FREQ = 100MHz
    //   If our AXI_CLK_FREQ is 200MHz (period of 5ns), we can just flip SCLK on every rising edge of the AXI CLK
    //   In the future, we might slow the AXI_CLK_FREQ to 100MHz (period of 10ns), so that would mean our SCLK freq would be capped at a period of 20ns (10ns low/10ns high), or SLCK_FREQ = 50MHz
    //
    //   But actaully, nevermind all that because we are using the diff/single transceivers, which have a bottleneck of 10MHz (period 100ns, 50ns low/50ns high)
    //   So instead, we will toggle SCLK every 10 AXI CLK cycles
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n)
            sclk <= 1'b0;
        else if(clr_sclk)
            sclk <= 1'b0;
        else if(sclk_div == sclk_cnt)       // Toggle SCLK if the appropriate number of AXI clock cycles have passed
            sclk <= ~sclk;
    end

    // SCLK divider, uses the SCLK_cnt parameter defined above, which is based on the AXI CLK frequency
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n)
            sclk_div <= 8'b0;
        else if(clr_sclk)                   // From SM: in states where SCLK should not toggle (all except RX), we should not run sclk_div 
            sclk_div <= 8'b0;
        else if (sclk_div == sclk_cnt)      // If the appropriate number of AXI clock cycles have passed, we will toggle sclk (meaning we should also reset the sclk_div)
            sclk_div <= 8'b0;
        else                                // Else keep running up sclk_div
            sclk_div <= sclk_div + 1;
    end



    // DATA READ-IN
    //   This includes double-flopping both miso_x and miso_y lines to account for crossing clock domains,
    //   as well as shifting these flopped values into our result registers
    
    // Double-flop
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) begin
            miso_x_1 <= 1'b0;
            miso_x_2 <= 1'b0;
            miso_y_1 <= 1'b0;
            miso_y_2 <= 1'b0;
        end
        else begin
            miso_x_1 <= miso_x;
            miso_x_2 <= miso_x_1;
            miso_y_1 <= miso_y;
            miso_y_2 <= miso_y_1;
        end
    end

    // SCLK falling edge detector
    reg sclk_1;
    wire sclk_fall;

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n)
            sclk_1 <= 1'b0;
        else
            sclk_1 <= sclk;
    end

    assign sclk_fall = (sclk_1 & ~sclk);

    // Shift registers
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) begin
            sensor_data_x = 18'b0;
            sensor_data_y = 18'b0;
        end
        else if(start) begin
            sensor_data_x = 18'b0;
            sensor_data_y = 18'b0;
        end
        else if(sclk_fall) begin
            sensor_data_x = {sensor_data_x[16:0], miso_x_2};
            sensor_data_y = {sensor_data_y[16:0], miso_y_2};
        end
    end



    // BIT COUNTER
    //   Counts that 18 bits have been shifted in (done18), completing the RX state
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n)
            bit_cnt <= 5'b0;
        else if(start)
            bit_cnt <= 5'b0;
        else if(sclk_fall)
            bit_cnt = bit_cnt + 1;
    end

    wire done18;
    assign done18 = (bit_cnt == 5'b10010); // Input for SM



    // DONE FF
    //   Set and cleared by SM
    //   'done' goes back out of the eddy current IP block signaling that a whole CONVERT/RECIEVE cycle has completed and the data is valid
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n)
            done <= 1'b0;
        else if(clr_done)
            done <= 1'b0;
        else if(set_done)
            done <= 1'b1;
    end

    

    ////////////////////////////////////////
    //
    //  STATE MACHINE LOGIC
    //
    ////////////////////////////////////
    
    reg [1:0] state, nxt_state;

    localparam IDLE = 2'b00;
    localparam CNV = 2'b01;
    localparam RX = 2'b10;
    // There is no need for a HOLD/WAIT state between the completion of RX and the beginning of a new CoNVersion
    //    This is because our 'start' signal that kicks off the process is synced to our PWM carrier, running at a relatively slow 100kHz
    //    so after RX completes, we will hang out in idle for a while before the next PWM_high or PWM_low kicks off another CoNVersion


    // NEXT STATE
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) begin
            state <= IDLE;
        end
        else begin
            state <= nxt_state;
        end
    end
  
  
    // STATE TRANSITIONS (input/outputs)
    // SM Inputs:
    //    start
    //    cnv_cmplt
    //    done18
    //   
    // SM Outputs:
    //    cnv      - the cnv line to the ADC
    //    clr_cnv  - reset cnv_div 
    //    clr_sclk - hold sclk low when not in RX state, and reset sclk_div
    //    clr_done - clr 'done' when we begin a new CNV/RX cycle
    //    set_done - set 'done' when RX is completed, meaning data is valid
    always @(*) begin
  
        // default nxt_state and outputs
        nxt_state = IDLE;
        cnv = 1'b0;
        clr_cnv = 1'b1;
        clr_sclk = 1'b1;
        clr_done = 1'b0;
        set_done = 1'b0;
      
        case(state)
            IDLE: begin
                if(start) begin
                    nxt_state = CNV;
                    clr_cnv = 1'b1;
                    clr_sclk = 1'b1;
                    clr_done = 1'b1;
                    set_done = 1'b0;
                end
                else begin
                    nxt_state = IDLE;
                    clr_cnv = 1'b1;
                    clr_sclk = 1'b1;
                    clr_done = 1'b0;
                    set_done = 1'b0;
                end
            end 
            CNV: begin
                cnv = 1'b1;
                if(cnv_cmplt) begin
                    nxt_state = RX;
                    clr_cnv = 1'b1;
                    clr_sclk = 1'b0;
                    clr_done = 1'b0;
                    set_done = 1'b0;
                end
                else begin
                    nxt_state = CNV;
                    clr_cnv = 1'b0;
                    clr_sclk = 1'b1;
                    clr_done = 1'b0;
                    set_done = 1'b0;
                end
            end
            RX: begin
                if(done18) begin
                    nxt_state = IDLE;
                    clr_cnv = 1'b1;
                    clr_sclk = 1'b1;
                    clr_done = 1'b0;
                    set_done = 1'b1;
                end
                else begin
                    nxt_state = RX;
                    clr_cnv = 1'b1;
                    clr_sclk = 1'b0;
                    clr_done = 1'b0;
                    set_done = 1'b0;
                end
            end
            default:
                begin
                    nxt_state = IDLE;
                    cnv = 0;
                    clr_cnv = 1'b1;
                    clr_sclk = 1'b1;
                    clr_done = 1'b1;
                    set_done = 1'b0;
                end
        endcase
    end

endmodule

`default_nettype wire