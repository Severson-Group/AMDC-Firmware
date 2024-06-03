`default_nettype none 

module amdc_spi_master(
    // INPUTS
    clk, rst_n, 
    trigger, 
    miso_x, miso_y, 
    sclk_cnt,
    shift_index,
    
    // OUTPUTS
    sclk, cnv, 
    sensor_data_x, sensor_data_y, 
    done,
    debug
    );

    //////////////////////////////////////////////////////////////////////
    //
    //  SPI Driver for AD4011 ADC in Kaman Eddy Current Sensor Box
    //
    ///////////////////////////////////////////////////////////////////

    /////////////////////////
    // PARAMETERS
    //////////////////////

    // FPGA_CLK_FREQ = 200 MEGAHERTZ
    // FPGA_CLK_PERIOD = 5 NANOSECONDS

    // For 5 MHz, SCLK needs to toggle every 100ns, so every 20 FPGA CLK periods (100 / FPGA_CLK_PERIOD)

    // We need to give the ADC 320ns to handle conversion, or 64 FPGA CLK periods (320 / FPGA_CLK_PERIOD)
    localparam cnv_cnt = 8'd64;
    

    ///////////////////////
    // INPUTS
    /////////////////////
    input wire clk, rst_n;
    input wire trigger;
    input wire miso_x, miso_y;
    input wire [7:0] sclk_cnt;
    input wire [7:0] shift_index;


    ///////////////////////
    // OUTPUTS
    /////////////////////
    output reg sclk;
    output reg cnv;
    output reg [17:0] sensor_data_x, sensor_data_y;
    output reg done;
    output wire [2:0] debug;


    ///////////////////////////////////////////
    // Internal regs and control signals
    ////////////////////////////////////////
    reg [7:0] sclk_div;
    reg miso_x_1, miso_x_2;
    reg miso_y_1, miso_y_2;
    reg [4:0] sclk_fall_cnt, shift_cnt;
    reg [7:0] cnv_div;

    reg clr_cnv;
    wire cnv_cmplt;
    reg clr_sclk;
    reg set_done, clr_done;
    
    // "trigger" (input) vs "start" (SM output)
    //   "trigger" is a module input that TRIES to start a new conversion/recieve transaction with the PWM settings requested by the user.
    //   "start" is the ACTUAL start signal. If a new trigger comes in and tries to start a new transaction BEFORE the previous
    //   transaction has completed, it will be ignored.
    reg start;



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
    //   If our FPGA_CLK_FREQ is 200MHz (period of 5ns), we can just flip SCLK on every rising edge of the FPGA CLK
    //   In the future, we might slow the FPGA_CLK_FREQ to 100MHz (period of 10ns), so that would mean our SCLK freq would be capped at a period of 20ns (10ns low/10ns high), or SLCK_FREQ = 50MHz
    //
    //   But actaully, nevermind all that because we are using the diff/single transceivers, which have a bottleneck of 10 MHz (period 100ns, 50ns low/50ns high)
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n)
            sclk <= 1'b0;
        else if(clr_sclk)
            sclk <= 1'b0;
        else if(sclk_div == sclk_cnt)       // Toggle SCLK if the appropriate number of FPGA clock cycles have passed
            sclk <= ~sclk;
    end

    // SCLK divider, uses the SCLK_cnt parameter defined above, which is based on the FPGA CLK frequency
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n)
            sclk_div <= 8'b0;
        else if(clr_sclk)                   // From SM: in states where SCLK should not toggle (all except RX), we should not run sclk_div 
            sclk_div <= 8'b0;
        else if (sclk_div == sclk_cnt)      // If the appropriate number of FPGA clock cycles have passed, we will toggle sclk (meaning we should also reset the sclk_div)
            sclk_div <= 8'b0;
        else                                // Else keep running up sclk_div
            sclk_div <= sclk_div + 1;
    end

    // SCLK edge detector
    reg sclk_1;
    wire sclk_fall, sclk_rise;

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) 
            sclk_1 <= 1'b0;
        else
            sclk_1 <= sclk;
    end

    assign sclk_fall = (sclk_1 & ~sclk);
    assign sclk_rise = (~sclk_1 & sclk);


    // SCLK FALL COUNTER
    //   Counts that 18 SCLK falls have occured (sclk_fall_18), completing the RX state
    //   However, because of the propogation delay, it is possible that not all 18 bits coming on 
    //   MISO have actually been shifted. Therefore, we need the WAIT state.
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n)
            sclk_fall_cnt <= 5'b0;
        else if(start)
            sclk_fall_cnt <= 5'b0;
        else if(sclk_fall)
            sclk_fall_cnt <= sclk_fall_cnt + 1;
    end

    wire sclk_fall_18;
    assign sclk_fall_18 = (sclk_fall_cnt == 5'd18); // Input for SM



    // SHIFT delayer
    //   Why is this needed? The Kaman adapter board's filtering may introduce a significant propogation delay 
    //  into the system. On the FPGA side, the SCLK signal being generated will rise (sclk_rise), which is when we 
    //  would like to sample the MISO line. However, this rise may take a while to propogate through the adapter
    //  board and then the valid data on the MISO lines may take a while to propogate back. This delay depends on 
    //  the RC filters used on the Kaman adapter board.
    //
    //  To account for this propogation delay, we need to delay our sclk_rise "shift" signal by the total round-trip 
    //  propogation delay. The shift signal delay is implemented in the FPGA below used a "shift" signal shift register. 
    //  The flip-flop we care about in this shift register is selected by "shift_index"; the C code driver (eddy_current_sensor.c) 
    //  function "eddy_current_sensor_set_timing()" allows the user to specify both the desired SCLK frequency, as well as the 
    //  Kaman board's one-way propogation delay, and will set "shift_index" to the correct value.
    wire shift;
    reg [255:0] shift_delay;

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n)
            shift_delay <= 256'b0;
        else if(start)
            shift_delay <= 256'b0;
        else
            shift_delay <= {shift_delay[254:0], sclk_rise};
    end

    assign shift = shift_delay[shift_index];


    // SHIFT COUNTER
    //   Counts that 18 bits have been actaully been shifted in, completing the WAIT state
    //   Then we have valid data and can assert 'done'
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n)
            shift_cnt <= 5'b0;
        else if(start)
            shift_cnt <= 5'b0;
        else if(shift)
            shift_cnt <= shift_cnt + 1;
    end

    reg shift_18; // Input for SM

    always @(posedge clk, negedge rst_n) begin
        if(!rst_n)
            shift_18 <= 1'b0;
        else if(start)
            shift_18 <= 1'b0;
        else if(shift_cnt == 5'd18)
            shift_18 <= 1'b1;
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

    // Shift registers
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) begin
            sensor_data_x <= 18'b0;
            sensor_data_y <= 18'b0;
        end
        else if(start) begin
            sensor_data_x <= 18'b0;
            sensor_data_y <= 18'b0;
        end
        else if(shift) begin
            sensor_data_x <= {sensor_data_x[16:0], miso_x_2};
            sensor_data_y <= {sensor_data_y[16:0], miso_y_2};
        end
    end



    // DONE FF
    //   Set and cleared by SM
    //   'done' goes back out of the eddy current IP block signaling that a whole 
    //   CONVERT/RECIEVE cycle has completed and the data is valid
    // Assert 'done' signal by default to prevent trigger signal in higher level
    // code from hanging
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n)
            done <= 1'b1;
        else if(clr_done)
            done <= 1'b0;
        else if(set_done)
            done <= 1'b1;
    end

    

    ////////////////////////////////////////
    //
    //  STATE MACHINE
    //
    ////////////////////////////////////
    
    reg [1:0] state, nxt_state;

    localparam IDLE = 2'b00;
    localparam CNV = 2'b01;
    localparam RX = 2'b10;
    localparam WAIT = 2'b11;
    // There is no need for an additional QUIET state between the completion of WAIT and the beginning of a new CoNVersion
    //    This is because our 'start' signal that kicks off the process is synced to our PWM carrier, running at a relatively 
    //    slow frequency so after RX/WAIT complete, we will hang out in idle for a while before the next PWM_high or PWM_low kicks 
    //    off another CoNVersion


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
    //    trigger        - Synced with the user-requested PWM triggers
    //    cnv_cmplt      - The appropriate amount of time has elapsed in the CNV state
    //    sclk_fall_18   - SCLK has fallen 18 times, so we can stop toggling it (RX -> WAIT)
    //    shift_18       - All 18 data bits on MISO have been shifted into our shift registers, so we are done (WAIT -> IDLE)
    //   
    // SM Outputs:
    //    start    - output when trigger is received while in the IDLE state to start a new transaction
    //    cnv      - the cnv line to the ADC
    //    clr_cnv  - reset cnv_div 
    //    clr_sclk - hold sclk low when not in RX state, and reset sclk_div
    //    clr_done - clr 'done' when we begin a new CNV/RX cycle
    //    set_done - set 'done' when RX is completed, meaning data is valid
    always @(*) begin
  
        // default nxt_state and outputs
        start = 1'b0;
        nxt_state = IDLE;
        cnv = 1'b0;
        clr_cnv = 1'b1;
        clr_sclk = 1'b1;
        clr_done = 1'b0;
        set_done = 1'b0;
      
        case(state)
            IDLE: begin
                if(trigger) begin
                    nxt_state = CNV;
                    clr_done = 1'b1;
                    start = 1'b1;
                end
            end 
            CNV: begin
                if(cnv_cmplt) begin
                    nxt_state = RX;
                    cnv = 1'b1;
                    clr_sclk = 1'b0;
                end
                else begin
                    nxt_state = CNV;
                    cnv = 1'b1;
                    clr_cnv = 1'b0;
                end
            end
            RX: begin
                // If the timing is fast enough, we can skip the WAIT state
                if(shift_18 & sclk_fall_18) begin 
                    nxt_state = IDLE;
                    set_done = 1'b1;
                end
                // However, if we are done toggling SCLK and not all bits have been shifted, we must WAIT
                else if(sclk_fall_18) begin 
                    nxt_state = WAIT;
                end
                else begin
                    nxt_state = RX;
                    clr_sclk = 1'b0;
                end
            end
            WAIT: begin
                if(shift_18) begin
                    nxt_state = IDLE;
                    set_done = 1'b1;
                end
                else begin
                    nxt_state = WAIT;
                end
            end
            default:
                begin
                    nxt_state = IDLE;
                    cnv = 1'b0;
                    clr_cnv = 1'b1;
                    clr_sclk = 1'b1;
                    clr_done = 1'b1;
                    set_done = 1'b0;
                    start = 1'b0;
                end
        endcase
    end


    //////////////////////////////////////
    // DEBUG PORTS
    ////////////////////////////////////

    reg shift_debug;

    always@(posedge clk, negedge rst_n) begin
        if(!rst_n)
            shift_debug <= 1'b0;
        else if(shift)
            shift_debug <= ~shift_debug;
    end

    assign debug[0] = trigger;
    assign debug[1] = 1'b1;
    assign debug[2] = 1'b1;

endmodule

`default_nettype wire
