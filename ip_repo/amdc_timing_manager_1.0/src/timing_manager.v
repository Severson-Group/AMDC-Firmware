module timing_manager(
    // INPUTS
    clk, rst_n,
    do_auto_triggering,
    send_manual_trigger,
    event_qualifier,
    user_ratio,
    en_bits, reset_sched_isr,
    // DONE SIGNALS
    adc_done, encoder_done,
    amds_0_done, amds_1_done,
    amds_2_done, amds_3_done,
    eddy_0_done, eddy_1_done,
    eddy_2_done, eddy_3_done,
    // OUTPUTS
    sched_isr,
    // Enable signals
    en_amds_0, en_amds_1,
    en_amds_2, en_amds_3,
    en_eddy_0, en_eddy_1,
    en_eddy_2, en_eddy_3,
    en_adc, en_encoder,
    // Time
    adc_time, encoder_time,
    amds0_time, amds1_time,
    amds2_time, amds3_time,
    eddy0_time, eddy1_time,
    eddy2_time, eddy3_time,
    trigger, count_time
);
    
    ////////////
    // INPUTS //
    ////////////
    input clk, rst_n;
    input wire do_auto_triggering;
    input wire send_manual_trigger;
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
    output wire en_amds_0, en_amds_1, en_amds_2, en_amds_3;
    output wire en_eddy_0, en_eddy_1, en_eddy_2, en_eddy_3;
    output wire en_adc, en_encoder;
    output reg trigger;
    output reg [15:0] amds0_time, amds1_time, amds2_time, amds3_time;
    output reg [15:0] eddy0_time, eddy1_time, eddy2_time, eddy3_time;
    output reg [15:0] adc_time, encoder_time;
    
    //////////////////////
    // Internal signals //
    //////////////////////

    // Holds the count to generate the interrupt based on the user ratio
    reg [15:0] count;
    // Signifies when all the sensors are done
    wire all_done;
    // Counts FPGA clock cycles for each sensor
    output reg [15:0] count_time;
    // See if any are enabled for all_done to be triggered
    wire sensors_enabled;
    
    //////////////////////////////////////////////////////////////////
    // Logic to generate trigger based on PWM carrier. This         //
    // allows the scheduler to run synchronized to the PWM carrier. //
    // The user can define a ratio that determines when the         //
    // trigger is generated.                                        //
    //////////////////////////////////////////////////////////////////
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) begin
            count <= 0;
        end
	else if (count == user_ratio) begin
	    count <= 0;
	end
        else if (event_qualifier) begin
            count <= count + 1;
        end
    end

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

    reg manual_trigger_queued;
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n)
            manual_trigger_queued <= 0;
        else if (send_manual_trigger)
            manual_trigger_queued <= 1;
        else if (trigger)
            manual_trigger_queued <= 0;
    end

    //////////////////////////////////////////////////////////////////
    // Logic that decides which sensors are being used. This takes  //
    // in a 16-bit number that has the bits set to whichever        //
    // sensor should to be enabled (0x00 by default) for timing.    //
    // BITS:                                                        //
    // en_bits[0]: AMDS - GPIO0                                     //
    // en_bits[1]: AMDS - GPIO1                                     //
    // en_bits[2]: AMDS - GPIO2                                     //
    // en_bits[3]: AMDS - GPIO3                                     //
    // en_bits[4]: Eddy current sensor - GPIO0                      //
    // en_bits[5]: Eddy current sensor - GPIO1                      //
    // en_bits[6]: Eddy current sensor - GPIO2                      //
    // en_bits[7]: Eddy current sensor - GPIO3                      //
    // en_bits[8]: Encoder                                          //
    // en_bits[9]: ADC                                              //
    //////////////////////////////////////////////////////////////////
    assign en_amds_0 =  en_bits[0];
    assign en_amds_1 =  en_bits[1];
    assign en_amds_2 =  en_bits[2];
    assign en_amds_3 =  en_bits[3];
    assign en_eddy_0 =  en_bits[4];
    assign en_eddy_1 =  en_bits[5];
    assign en_eddy_2 =  en_bits[6];
    assign en_eddy_3 =  en_bits[7];
    assign en_encoder = en_bits[8];
    assign en_adc =     en_bits[9];

    //////////////////////////////////////////////////////////////////
    // The signal all_done will be asserted when all of the enabled //
    // sensors are done with their respective conversions, sending  //
    // the signal the acquisition time is complete for all sensors. //
    // Each sensor goes high either if it is not enabled or if it   //
    // is enabled and the done signal has been recieved. all_done   //
    // can also only be high if at least one sensor is enabled.     //
    //////////////////////////////////////////////////////////////////
    assign sensors_enabled = en_eddy_0 | en_eddy_1 | en_eddy_2 | en_eddy_3 |
                            en_amds_0 | en_amds_1 | en_amds_2 | en_amds_3 |
                            en_encoder | en_adc;
    
    assign all_done = ((!en_eddy_0 || eddy_0_done) &
                        (!en_eddy_1 || eddy_1_done) &
                        (!en_eddy_2 || eddy_2_done) &
                        (!en_eddy_3 || eddy_3_done) &
                        (!en_amds_0 || amds_0_done) &
                        (!en_amds_1 || amds_1_done) &
                        (!en_amds_2 || amds_2_done) &
                        (!en_amds_3 || amds_3_done) &
                        (!en_encoder || encoder_done) &
                        (!en_adc || adc_done)) &
                        sensors_enabled;

    //////////////////////////////////////////////////////////////////
    // Rising edge detection for all_done, which signifies when to    //
    // send an interrupt                                            //
    //////////////////////////////////////////////////////////////////
    reg all_done_ff;
    wire all_done_pe;
    always @(posedge clk) begin
        all_done_ff <= all_done;
    end
    assign all_done_pe = all_done & ~all_done_ff;

    //////////////////////////////////////////////////////////////////
    // Send an interrupt to the PS once all of the sensors are done //
    // with their conversion/acquisition. This will trigger a       //
    // task on the C side in the timing manager driver              //
    //////////////////////////////////////////////////////////////////
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n)
            sched_isr <= 0;
        else if (all_done_pe)
            sched_isr <= 1;
        else if (reset_sched_isr)
            sched_isr <= 0;
    end

    ////////////////////////////////////////////////////////////////// 
    // Generating the acquisition time per sensor. Once the trigger //
    // is sent and the user decides what sensors to enable, then    //
    // the time can start being recorded. It is stopped once a done //
    // signal is recieved from a sensor, and done once all are done //
    //////////////////////////////////////////////////////////////////

    reg adc_ff, encoder_ff, amds_0_ff, amds_1_ff, amds_2_ff, amds_3_ff, eddy_0_ff, eddy_1_ff, eddy_2_ff, eddy_3_ff;
    wire adc_pe, encoder_pe, amds_0_pe, amds_1_pe, amds_2_pe, amds_3_pe, eddy_0_pe, eddy_1_pe, eddy_2_pe, eddy_3_pe;
    
    // Detect a rising edge for each done signal to copy over at that point
    
    // ADC
    always @(posedge clk) begin
        adc_ff <= adc_done;
    end
    assign adc_pe = adc_done & ~adc_ff;        

    // Encoder
    always @(posedge clk) begin
        encoder_ff <= encoder_done;
    end
    assign encoder_pe = encoder_done & ~encoder_ff;
    
    // Eddy 0
    always @(posedge clk) begin
        eddy_0_ff <= eddy_0_done;
    end
    assign eddy_0_pe = eddy_0_done & ~eddy_0_ff;

    // Eddy 1
    always @(posedge clk) begin
        eddy_1_ff <= eddy_1_done;
    end
    assign eddy_1_pe = eddy_1_done & ~eddy_1_ff;

    // Eddy 2
    always @(posedge clk) begin
        eddy_2_ff <= eddy_2_done;
    end
    assign eddy_2_pe = eddy_2_done & ~eddy_2_ff;

    // Eddy 3
    always @(posedge clk) begin
        eddy_3_ff <= eddy_3_done;
    end
    assign eddy_3_pe = eddy_3_done & ~eddy_3_ff;

    // AMDS 0
    always @(posedge clk) begin
        amds_0_ff <= amds_0_done;
    end
    assign amds_0_pe = amds_0_done & ~amds_0_ff;

    // AMDS 1
    always @(posedge clk) begin
        amds_1_ff <= amds_1_done;
    end
    assign amds_1_pe = amds_1_done & ~amds_1_ff;

    // AMDS 2
    always @(posedge clk) begin
        amds_2_ff <= amds_2_done;
    end
    assign amds_2_pe = amds_2_done & ~amds_2_ff;

    // AMDS 3
    always @(posedge clk) begin
        amds_3_ff <= amds_3_done;
    end
    assign amds_3_pe = amds_3_done & ~amds_3_ff;
    

    // Count the time when start_count is asserted, otherwise
    // the time should be reset to 0.
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) count_time <= 0;
        else if (trigger) count_time <= 0;    // Restart upon trigger
        else count_time <= count_time + 1;
    end

    // Get ADC time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) adc_time <= 0;
        else if (adc_pe) adc_time <= count_time;
    end

    // Get encoder time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) encoder_time <= 0;
        else if (encoder_pe) encoder_time <= count_time;
    end

    // Get eddy current sensor 0 time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) eddy0_time <= 0;
        else if (eddy_0_pe) eddy0_time <= count_time;
    end

    // Get eddy current sensor 1 time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) eddy1_time <= 0;
        else if (eddy_1_pe) eddy1_time <= count_time;
    end

    // Get eddy current sensor 2 time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) eddy2_time <= 0;
        else if (eddy_2_pe) eddy2_time <= count_time;
    end

    // Get eddy current sensor 3 time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) eddy3_time <= 0;
        else if (eddy_3_pe) eddy3_time <= count_time;
    end

    // Get AMDS 0 time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) amds0_time <= 0;
        else if (amds_0_pe) amds0_time <= count_time;
    end

    // Get AMDS 1 time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) amds1_time <= 0;
        else if (amds_1_pe) amds1_time <= count_time;
    end

    // Get AMDS 2 time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) amds2_time <= 0;
        else if (amds_2_pe) amds2_time <= count_time;
    end

    // Get AMDS 3 time
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) amds3_time <= 0;
        else if (amds_3_pe) amds3_time <= count_time;
    end

endmodule

`default_nettype wire