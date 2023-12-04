module timing_manager(
                        // INPUTS
                        clk, rst_n,
                        event_qualifier,
                        user_ratio,
                        en_bits,
                        // DONE SIGNALS
                        adc_done, encoder_done,
                        eddy_0_done, eddy_1_done,
                        eddy_2_done, eddy_3_done,
                        // PWM signals
                        pwm_carrier_low, pwm_carrier_high,
                        // OUTPUTS
                        sched_isr,
                        // Enable signals
                        en_eddy_0, en_eddy_1,
                        en_eddy_2, en_eddy_3,
                        en_adc, en_encoder,
                        // Time
                        adc_time, encoder_time,
                        eddy0_time, eddy1_time,
                        eddy2_time, eddy3_time,
                        trigger
                        );
    
    ////////////
    // INPUTS //
    ////////////
    input clk, rst_n;
    input wire pwm_carrier_low, pwm_carrier_high;
    input wire [15:0] user_ratio;
    input wire [7:0] en_bits;
    input wire adc_done;
    input wire encoder_done;
    input wire eddy_0_done, eddy_1_done, eddy_2_done, eddy_3_done;
    input wire event_qualifier;
    
    /////////////
    // OUTPUTS //
    /////////////
    output reg sched_isr;
    output wire en_eddy_0, en_eddy_1, en_eddy_2, en_eddy_3;
    output wire en_adc, en_encoder;
    output reg trigger;
    output reg [15:0] adc_time, encoder_time, eddy0_time, eddy1_time, eddy2_time, eddy3_time;
    
    //////////////////////
    // Internal signals //
    //////////////////////

    // Holds the count to generate the interrupt based on the user ratio
    reg [15:0] count;
    // Signifies when all the sensors are done
    wire all_done;
    // Counts FPGA clock cycles for each sensor
    reg [15:0] count_time;
    // Determines when the count should be started based on when the sensors begin
    reg start_count; 
    
    //////////////////////////////////////////////////////////////////
    // Logic to generate interrupt based on PWM carrier. This       //
    // allows the scheduler to run synchronized to the PWM carrier. //
    // The user can define a ratio that determines when the         //
    // interrupt is generated.                                      //
    //////////////////////////////////////////////////////////////////
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) begin
            count <= 0;
            trigger <= 0;
        end
        else if (count == user_ratio) begin
            count <= 0;
            trigger <= 1;
        end
        else if (event_qualifier) begin
            count <= count + 1;
            trigger <= 0;
        end
    end

    //////////////////////////////////////////////////////////////////
    // Logic that decides which sensors are being used. This takes  //
    // in an 8-bit number that has the bits set to whichever        //
    // sensor should to be enabled (0x00 by default) for timing.    //
    // BITS:                                                        //
    // en_bits[0]: Eddy current sensor - GPIO0                      //
    // en_bits[1]: Eddy current sensor - GPIO1                      //
    // en_bits[2]: Eddy current sensor - GPIO2                      //
    // en_bits[3]: Eddy current sensor - GPIO3                      //
    // en_bits[4]: Encoder                                          //
    // en_bits[5]: ADC                                              //
    //////////////////////////////////////////////////////////////////
    assign en_eddy_0 =  en_bits[0];
    assign en_eddy_1 =  en_bits[1];
    assign en_eddy_2 =  en_bits[2];
    assign en_eddy_3 =  en_bits[3];
    assign en_encoder = en_bits[4];
    assign en_adc =     en_bits[5];

    //////////////////////////////////////////////////////////////////
    // The signal all_done will be asserted when all of the enabled //
    // sensors are done with their respective conversions, sending  //
    // the signal the acquisition time is complete for all sensors. //
    // Each sensor goes high either if it is not enabled or if it   //
    // is enabled and the done signal has been recieved.            //
    //////////////////////////////////////////////////////////////////
    assign all_done = (
                        (!en_eddy_0 || en_eddy_0 && eddy_0_done) &&
                        (!en_eddy_1 || en_eddy_1 && eddy_1_done) &&
                        (!en_eddy_2 || en_eddy_2 && eddy_2_done) &&
                        (!en_eddy_3 || en_eddy_3 && eddy_3_done) &&
                        (!en_encoder || en_encoder && encoder_done) &&
                        (!en_adc || en_adc && adc_done)
                        );

    //////////////////////////////////////////////////////////////////
    // Send an interrupt to the PS once all of the sensors are done //
    // with their conversion/acquisition. This will trigger a       //
    // FreeRTOS task on the C side in the timing manager driver     //
    //////////////////////////////////////////////////////////////////
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) begin
            sched_isr <= 0;
        end
        else if (all_done) begin
            sched_isr <= 1;
        end
        else begin
            sched_isr <= 0; // ISR only called when all sensors are done
        end
    end

    ////////////////////////////////////////////////////////////////// 
    // Generating the acquisition time per sensor. Once the trigger //
    // is sent and the user decides what sensors to enable, then    //
    // the time can start being recorded. It is stopped once a done //
    // signal is recieved from a sensor, and done once all are done //
    //////////////////////////////////////////////////////////////////

    // Start counting upon trigger being asserted, stop once all
    // sensors are done.
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) start_count <= 0;
        else if (trigger) start_count <= 1;
        else if (all_done) start_count <= 0;
    end

    // Count the time when start_count is asserted, otherwise
    // the time should be reset to 0.
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) count_time <= 0;
        else if (start_count) count_time <= count_time + 1;
        else count_time <= 0;
    end

    // Get ADC time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) adc_time <= 0;
        else if (adc_done) adc_time <= count_time;
    end

    // Get encoder time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) encoder_time <= 0;
        else if (encoder_done) encoder_time <= count_time;
    end

    // Get eddy current sensor 0 time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) eddy0_time <= 0;
        else if (eddy_0_done) eddy0_time <= count_time;
    end

    // Get eddy current sensor 1 time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) eddy1_time <= 0;
        else if (eddy_1_done) eddy1_time <= count_time;
    end

    // Get eddy current sensor 2 time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) eddy2_time <= 0;
        else if (eddy_2_done) eddy2_time <= count_time;
    end

    // Get eddy current sensor 3 time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) eddy3_time <= 0;
        else if (eddy_3_done) eddy3_time <= count_time;
    end

endmodule

`default_nettype wire