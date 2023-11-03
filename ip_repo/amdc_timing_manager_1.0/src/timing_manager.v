module timing_manager(
                        // INPUTS
                        clk, rst_n,
                        trigger,
                        user_ratio,
                        // DONE SIGNALS
                        adc_done, encoder_done,
                        eddy_0_done, eddy_1_done,
                        eddy_2_done, eddy_3_done,
                        // PWM signals
                        pwm_carrier_low, pwm_carrier_high,
                        // OUTPUTS
                        all_done, sched_isr);
    
    ////////////
    // INPUTS //
    ////////////
    input clk, rst_n;
    input wire pwm_carrier_low, pwm_carrier_high;
    input wire [15:0] user_ratio;
    input wire adc_done;
    input wire encoder_done;
    input wire eddy_0_done, eddy_1_done, eddy_2_done, eddy_3_done;
    input wire trigger;
    // input wire gpio0_done, gpio1_done, gpio2_done, gpio3_done
    //eddy_done, encoder_done, amds_done;
    
    /////////////
    // OUTPUTS //
    /////////////
    //output reg flush_duty;
    output reg sched_isr;
    output wire all_done;
    
    // Internal signals
    reg [15:0] count;   // Holds the count to generate the interrupt based
                        // on the user ratio
    
    // Selecting the done
    // Based on the enable, choose the desired done signals to get
    // the acquisition time per each of those sensors
    // (e.g. if adc and encoder were chosen, use adc_done and encoder_done
    // to figure out the acqusition time
    
    ////////////////////////////////////////////////////////////////// 
    // Generating the acquisition time per sensor. Once the trigger //
    // is sent and the user decides what sensors to enable, then    //
    // the time can start being recorded. It is stopped once a done //
    // signal is recieved from a sensor, and done once all are done //
    //////////////////////////////////////////////////////////////////
    
    
    // All done will be based on when all of the selected ones are done.
    // all_done is high when something is enabled and its corresponding
    // done signal is high
    // TRIAL:
    // all_done = ((!en_adc || en_adc && adc_done) &&    // if adc is enable, based on done - if not enable, doesn't matter
    //             (!en_eddy || en_eddy && eddy_done) &&
    //             (!en_encoder || en_encoder && encoder_done)
    //            )               
    assign all_done = adc_done;
    
    //////////////////////////////////////////////////////////////////
    // Logic to generate interrupt based on PWM carrier. This       //
    // allows the scheduler to run synchronized to the PWM carrier. //
    // The user can define a ratio that determines when the         //
    // interrupt is generated.                                      //
    //////////////////////////////////////////////////////////////////
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) begin
            count <= 0;
            sched_isr <= 0;
        end
        else if (count == user_ratio) begin
            count <= 0;
            sched_isr <= 1;
        end
        else if (trigger) begin
            count <= count + 1;
            sched_isr <= 0;
        end
    end
    
    // Logic to decide which sensors to use
    // Possibly: 4-bit number, a one represents choosing the sensor?
    // e.g. 0111 would be sensors 1-3, 1001 would be sensors 1 and 4, numbering each of them
    // so the user can put in the text like "enable encoder adc" and those could be 1 and 2,
    // so the number that would be sent here would be 0011??
    

endmodule

`default_nettype wire