module timing_manager(
    // INPUTS
    clk, rst_n,
    do_auto_triggering,
    send_manual_trigger,
    event_qualifier,
    user_ratio,
    en_bits, reset_sched_isr,
    sched_source_mode,
    // DONE SIGNALS
    adc_done, encoder_done,
    amds_0_done, amds_1_done,
    amds_2_done, amds_3_done,
    eddy_0_done, eddy_1_done,
    eddy_2_done, eddy_3_done,
    // OUTPUTS
    sched_isr,
    // Enable signals
    en_adc, en_encoder,
    en_amds_0, en_amds_1,
    en_amds_2, en_amds_3,
    en_eddy_0, en_eddy_1,
    en_eddy_2, en_eddy_3,
    // Time
    adc_time, encoder_time,
    amds_0_time, amds_1_time,
    amds_2_time, amds_3_time,
    eddy_0_time, eddy_1_time,
    eddy_2_time, eddy_3_time,
    trigger, sched_tick_time
);
    
    ////////////
    // INPUTS //
    ////////////
    input wire clk, rst_n;
    input wire do_auto_triggering;
    input wire send_manual_trigger;
    input wire sched_source_mode;
    input wire [15:0] user_ratio;
    input wire [15:0] en_bits;
    input wire adc_done;
    input wire encoder_done;
    input wire amds_0_done, amds_1_done, amds_2_done, amds_3_done;
    input wire eddy_0_done, eddy_1_done, eddy_2_done, eddy_3_done;
    input wire event_qualifier;
    input wire reset_sched_isr;
    
    /////////////
    // OUTPUTS //
    /////////////
    output reg sched_isr;
    output wire en_adc, en_encoder;
    output wire en_amds_0, en_amds_1, en_amds_2, en_amds_3;
    output wire en_eddy_0, en_eddy_1, en_eddy_2, en_eddy_3;
    output reg trigger;
    output reg [15:0] adc_time, encoder_time;
    output reg [15:0] amds_0_time, amds_1_time, amds_2_time, amds_3_time;
    output reg [15:0] eddy_0_time, eddy_1_time, eddy_2_time, eddy_3_time;
    output reg [31:0] sched_tick_time;
    
    //////////////////////
    // Internal signals //
    //////////////////////

    // Holds the count to generate the interrupt based on the user ratio
    reg [15:0] count;
    // Signifies when all the sensors are done
    wire all_done;
    // Counts FPGA clock cycles for each sensor
    reg [31:0] count_time;
    // Counts elapsed time for each interrupt cycle
    reg [31:0] count_tick_time;
    // See if any are enabled for all_done to be triggered
    wire sensors_enabled;
    
    //////////////////////////////////////////////////////////////////
    // Logic to generate trigger based on PWM carrier. This         //
    // allows the scheduler to run synchronized to the PWM carrier. //
    // The user can define a ratio that determines when the         //
    // trigger is generated.                                        //
    //////////////////////////////////////////////////////////////////
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n)
            count <= 0;
        else if (count == user_ratio)
            count <= 0;
        else if (event_qualifier)
            count <= count + 1;
    end

    reg manual_trigger_queued;
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n)
            trigger <= 0;
        else if (do_auto_triggering & (count == user_ratio) & all_done)
            // In auto triggering mode, send the next trigger if the count of PWM events has
            // reached the desired user ratio, and all enabled sensors are done sampling
            trigger <= 1;
        else if (manual_trigger_queued & event_qualifier & all_done)
            // If we are in manual trigger mode and the user has requested/queued a trigger,
            // wait until all sensors are done and the next qualified PWM peak/valley to trigger
            trigger <= 1;
        else
            trigger <= 0;
    end

    always @(posedge clk, negedge rst_n) begin
        if (!rst_n)
            manual_trigger_queued <= 0;
        else if (send_manual_trigger)
            manual_trigger_queued <= 1;
        else if (trigger)
            manual_trigger_queued <= 0;
    end

    // IMPORTANT:
    // Sensor order in FPGA verilog MUST BE KEPT IN AGREEMENT
    // with the order of the sensor_e enumeration in the driver code!!
    // See timing_manager.h for details!

    //////////////////////////////////////////////////////////////////
    // Logic that decides which sensors are being used. This takes  //
    // in a 16-bit number that has the bits set to whichever        //
    // sensor should to be enabled (0x00 by default) for timing.    //
    // BITS:                                                        //
    // en_bits[0]: ADC                                              //
    // en_bits[1]: Encoder                                          //
    // en_bits[2]: AMDS - GPIO0                                     //
    // en_bits[3]: AMDS - GPIO1                                     //
    // en_bits[4]: AMDS - GPIO2                                     //
    // en_bits[5]: AMDS - GPIO3                                     //
    // en_bits[6]: Eddy current sensor - GPIO0                      //
    // en_bits[7]: Eddy current sensor - GPIO1                      //
    // en_bits[8]: Eddy current sensor - GPIO2                      //
    // en_bits[9]: Eddy current sensor - GPIO3                      //
    //////////////////////////////////////////////////////////////////
    assign en_adc = en_bits[0];
    assign en_encoder = en_bits[1];
    assign en_amds_0 =  en_bits[2];
    assign en_amds_1 =  en_bits[3];
    assign en_amds_2 =  en_bits[4];
    assign en_amds_3 =  en_bits[5];
    assign en_eddy_0 =  en_bits[6];
    assign en_eddy_1 =  en_bits[7];
    assign en_eddy_2 =  en_bits[8];
    assign en_eddy_3 =  en_bits[9];


    //////////////////////////////////////////////////////////////////
    // The signal all_done will be asserted when all of the enabled //
    // sensors are done with their respective conversions, sending  //
    // the signal the acquisition time is complete for all sensors. //
    // Each sensor goes high either if it is not enabled or if it   //
    // is enabled and the done signal has been recieved. all_done   //
    // can also only be high if at least one sensor is enabled.     //
    //////////////////////////////////////////////////////////////////
    assign sensors_enabled = en_adc | en_encoder |
                            en_amds_0 | en_amds_1 | en_amds_2 | en_amds_3 |
                            en_eddy_0 | en_eddy_1 | en_eddy_2 | en_eddy_3;

    assign all_done = ((!en_adc || adc_done) &
                        (!en_encoder || encoder_done) &
                        (!en_amds_0 || amds_0_done) &
                        (!en_amds_1 || amds_1_done) &
                        (!en_amds_2 || amds_2_done) &
                        (!en_amds_3 || amds_3_done) &
                        (!en_eddy_0 || eddy_0_done) &
                        (!en_eddy_1 || eddy_1_done) &
                        (!en_eddy_2 || eddy_2_done) &
                        (!en_eddy_3 || eddy_3_done)) &
                        sensors_enabled;

    //////////////////////////////////////////////////////////////////
    // Rising edge detection for all_done, which signifies when to  //
    // send an interrupt                                            //
    //////////////////////////////////////////////////////////////////
    reg all_done_ff;
    wire all_done_pe;
    always @(posedge clk) begin
        all_done_ff <= all_done;
    end
    assign all_done_pe = all_done & ~all_done_ff;

    ///////////////////////////////////////////////////////////////////
    // Scheduler Interrupts:
    // Two different interrupts are used as a source of the scheduler
    // in the C code (sent to the processing system from the FPGA):
    // Mode = 0 (legacy mode):
    //  -   interrupt is asserted every 'trigger' signal, and is not
    //      synchronized to the sensor I/O - just the PWM carrier
    //  -   the user_ratio determines the control frequency
    //  -   *note* the actual trigger signal is not used as it is based
    //      on the all_done signal, is synchronized to the sensors.
    //      instead, this interrupt is based on the same condition as
    //      the trigger, just excluding all_done
    // Mode = 1 (timing manager):
    //  - if no sensors are enabled:
    //      -   the functionality is the same as mode 0, so based on
    //          a 'trigger'
    //  - if sensors are enabled:
    //      -   interrupt is synchronized with sensor I/O and asserted
    //          once all the enabled sensors have completed their
    //          acquisition/conversion cycle (e.g. on the rising edge
    //          of the all_done signal)
    //////////////////////////////////////////////////////////////////
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) begin
            sched_isr <= 0;
        end
        else if (~sched_source_mode & (count == user_ratio)) begin
            sched_isr <= 1;
        end
        else if (sched_source_mode & ~sensors_enabled & (count == user_ratio)) begin
            sched_isr <= 1;
        end
        else if (sched_source_mode & all_done_pe) begin
            sched_isr <= 1;
        end
        else if (reset_sched_isr) begin
            sched_isr <= 0;
        end
    end

    ////////////////////////////////////////////////////////////////// 
    // Generating the acquisition time per sensor. Once the trigger //
    // is sent and the user decides what sensors to enable, then    //
    // the time can start being recorded. It is stopped once a done //
    // signal is recieved from a sensor, and done once all are done //
    //////////////////////////////////////////////////////////////////

    reg adc_ff, encoder_ff, amds_0_ff, amds_1_ff, amds_2_ff, amds_3_ff, eddy_0_ff, eddy_1_ff, eddy_2_ff, eddy_3_ff;
    wire adc_pe, encoder_pe, amds_0_pe, amds_1_pe, amds_2_pe, amds_3_pe, eddy_0_pe, eddy_1_pe, eddy_2_pe, eddy_3_pe;
    
    // Detect a rising edge for each done signal to copy over time at that point

    always @(posedge clk) begin
        adc_ff <= adc_done;
        encoder_ff <= encoder_done;
        amds_0_ff <= amds_0_done;
        amds_1_ff <= amds_1_done;
        amds_2_ff <= amds_2_done;
        amds_3_ff <= amds_3_done;
        eddy_0_ff <= eddy_0_done;
        eddy_1_ff <= eddy_1_done;
        eddy_2_ff <= eddy_2_done;
        eddy_3_ff <= eddy_3_done;
    end

    assign adc_pe = adc_done & ~adc_ff;        
    assign encoder_pe = encoder_done & ~encoder_ff;
    assign amds_0_pe = amds_0_done & ~amds_0_ff;
    assign amds_1_pe = amds_1_done & ~amds_1_ff;
    assign amds_2_pe = amds_2_done & ~amds_2_ff;
    assign amds_3_pe = amds_3_done & ~amds_3_ff;
    assign eddy_0_pe = eddy_0_done & ~eddy_0_ff;
    assign eddy_1_pe = eddy_1_done & ~eddy_1_ff;
    assign eddy_2_pe = eddy_2_done & ~eddy_2_ff;
    assign eddy_3_pe = eddy_3_done & ~eddy_3_ff;
    

    // Count the time when trigger is asserted
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n)
            count_time <= 32'h0;
        else if (trigger)
            count_time <= 32'h0;    // Restart upon trigger
        else
            count_time <= count_time + 1;
    end
    
    // Count the elapsed time between each scheduler ISR call
    reg sched_isr_ff;
    wire sched_isr_pe;
    always @(posedge clk) begin
        sched_isr_ff <= sched_isr;
    end
    assign sched_isr_pe = sched_isr & ~sched_isr_ff;

    always @(posedge clk, negedge rst_n) begin
        if (!rst_n)
            count_tick_time <= 32'h0;
        else if (sched_isr_pe)
            count_tick_time <= 32'h0;   // restart upon ISR call
        else
            count_tick_time <= count_tick_time + 1;
    end
    
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n)
            sched_tick_time <= 32'h0;
        else if (sched_isr_pe)
            sched_tick_time <= count_tick_time;
    end

    // Get ADC time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) adc_time <= 0;
        else if (adc_pe) adc_time <= count_time[15:0];
    end

    // Get encoder time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) encoder_time <= 0;
        else if (encoder_pe) encoder_time <= count_time[15:0];
    end

    // Get AMDS 0 time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) amds_0_time <= 0;
        else if (amds_0_pe) amds_0_time <= count_time[15:0];
    end

    // Get AMDS 1 time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) amds_1_time <= 0;
        else if (amds_1_pe) amds_1_time <= count_time[15:0];
    end

    // Get AMDS 2 time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) amds_2_time <= 0;
        else if (amds_2_pe) amds_2_time <= count_time[15:0];
    end

    // Get AMDS 3 time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) amds_3_time <= 0;
        else if (amds_3_pe) amds_3_time <= count_time[15:0];
    end

    // Get eddy current sensor 0 time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) eddy_0_time <= 0;
        else if (eddy_0_pe) eddy_0_time <= count_time[15:0];
    end

    // Get eddy current sensor 1 time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) eddy_1_time <= 0;
        else if (eddy_1_pe) eddy_1_time <= count_time[15:0];
    end

    // Get eddy current sensor 2 time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) eddy_2_time <= 0;
        else if (eddy_2_pe) eddy_2_time <= count_time[15:0];
    end

    // Get eddy current sensor 3 time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) eddy_3_time <= 0;
        else if (eddy_3_pe) eddy_3_time <= count_time[15:0];
    end

endmodule

`default_nettype wire