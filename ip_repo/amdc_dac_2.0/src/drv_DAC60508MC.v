`timescale 1ns / 1ps

//////////////////////////////////////////////////////////
//
// DAC Interface Driver for Texas Instruments DAC60508MC
//
// Brandt Buchda, July 27, 2020
//
//////////////////////////////////////////////////////////
//
// NOTES:
//  Core Clock                       |     200MHz
//  SCLK                             |      25MHz
//  Maximum Register Update Clock    |  781.25KHz
//  Individual Register Update Clock |  781.25KHz to 3.05KHz
//
//////////////////////////////////////////////////////////

module drv_DAC60508MC(
    clk, rst_n, 
    SYNC_data, CONFIG_data, GAIN_data, TRIGGER_data, BRDCAST_data, 
    DAC0_data, DAC1_data, DAC2_data, DAC3_data, DAC4_data, DAC5_data, DAC6_data, DAC7_data,
    SYNC_w, CONFIG_w, GAIN_w, TRIGGER_w, BRDCAST_w,
    DAC0_w, DAC1_w, DAC2_w, DAC3_w, DAC4_w, DAC5_w, DAC6_w, DAC7_w, 
    CS, SCLK, MOSI
);
    /// CONSTANTS ///
    localparam IDLE = 4'h0;
    localparam SYNC = 4'h2;
    localparam CONFIG = 4'h3;
    localparam GAIN = 4'h4;
    localparam TRIGGER = 4'h5;
    localparam BRDCAST = 4'h6;
    localparam DAC0 = 4'h8;
    localparam DAC1 = 4'h9;
    localparam DAC2 = 4'hA;
    localparam DAC3 = 4'hB;
    localparam DAC4 = 4'hC;
    localparam DAC5 = 4'hD;
    localparam DAC6 = 4'hE;
    localparam DAC7 = 4'hF;

    localparam TX_START = 4'h1;
    localparam TX_WAIT =  4'h7;

    /// Inputs ///
    input wire clk, rst_n;  // Clock freq @ 200MHz

    /// DAC Registers ///
    input wire [15:0] SYNC_data;     // Data registers written to by the AXIlite bus from the C driver
    input wire [15:0] CONFIG_data;
    input wire [15:0] GAIN_data;
    input wire [15:0] TRIGGER_data;
    input wire [15:0] BRDCAST_data;
    input wire [15:0] DAC0_data;
    input wire [15:0] DAC1_data;
    input wire [15:0] DAC2_data;
    input wire [15:0] DAC3_data;
    input wire [15:0] DAC4_data;
    input wire [15:0] DAC5_data;
    input wire [15:0] DAC6_data;
    input wire [15:0] DAC7_data;

    /// Register Write flags ///
    input wire SYNC_w;        // Flags from the AXIlite bus that indicate the register was written to
    input wire CONFIG_w;
    input wire GAIN_w;
    input wire TRIGGER_w;
    input wire BRDCAST_w;
    input wire DAC0_w;
    input wire DAC1_w;
    input wire DAC2_w;
    input wire DAC3_w;
    input wire DAC4_w;
    input wire DAC5_w;
    input wire DAC6_w;
    input wire DAC7_w;

    /// Outputs ///
    output wire CS, SCLK, MOSI;

    /// Write Enables ///
    reg [15:0] reg_w;          // Indicates the register has updated data
    reg [15:0] reg_w_clr;      // Clears the write signal
    wire reg_queue;
    assign reg_queue = |reg_w; // Indicates atleast one register has data that needs to be tx
    
    // State machine signals
    reg [3:0] state, tx_state, next_state, return_state;
    reg save_state, save_data;
    reg [23:0] tx_reg_data;

    // SPI sigals
    reg tx_trig;            // Triggers a SPI transmission to the DAC
    reg [23:0] tx_data;     // The data transmitted across the MOSI line
    wire tx_done;           

    // Instantiate SPI transmitter
    SPI_TX spi_tx(
        .clk(clk), 
        .rst_n(rst_n), 
        .trig(tx_trig), 
        .tx_data(tx_data), 
        .CS(CS), 
        .SCLK(SCLK), 
        .MOSI(MOSI), 
        .done(tx_done)
    );

    //////////////////////////// SYNC Register Flops //////////////////////////

    // SYNC Write
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) reg_w[SYNC] <= 1'b0;
        else if(reg_w_clr[SYNC]) reg_w[SYNC] = 1'b0;
        else if(SYNC_w) reg_w[SYNC] = 1'b1;
    end

    //////////////////////////// CONFIG Register Flops //////////////////////////

    // CONFIG Write
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) reg_w[CONFIG] <= 1'b0;
        else if(reg_w_clr[CONFIG]) reg_w[CONFIG] = 1'b0;
        else if(CONFIG_w) reg_w[CONFIG] = 1'b1;
    end

    //////////////////////////// GAIN Register Flops //////////////////////////

    // GAIN Write
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) reg_w[GAIN] <= 1'b0;
        else if(reg_w_clr[GAIN]) reg_w[GAIN] = 1'b0;
        else if(GAIN_w) reg_w[GAIN] = 1'b1;
    end

    //////////////////////////// TRIGGER Register Flops //////////////////////////

    // TRIGGER Write
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) reg_w[TRIGGER] <= 1'b0;
        else if(reg_w_clr[TRIGGER]) reg_w[TRIGGER] = 1'b0;
        else if(TRIGGER_w) reg_w[TRIGGER] = 1'b1;
    end

    //////////////////////////// BRDCAST Register Flops //////////////////////////

    // BRDCAST Write
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) reg_w[BRDCAST] <= 1'b0;
        else if(reg_w_clr[BRDCAST]) reg_w[BRDCAST] = 1'b0;
        else if(BRDCAST_w) reg_w[BRDCAST] = 1'b1;
    end

    //////////////////////////// DAC0 Register Flops //////////////////////////

    // DAC0 Write
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) reg_w[DAC0] <= 1'b0;
        else if(reg_w_clr[DAC0]) reg_w[DAC0] = 1'b0;
        else if(DAC0_w) reg_w[DAC0] = 1'b1;
    end

    //////////////////////////// DAC1 Register Flops //////////////////////////

    // DAC1 Write
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) reg_w[DAC1] <= 1'b0;
        else if(reg_w_clr[DAC1]) reg_w[DAC1] = 1'b0;
        else if(DAC1_w) reg_w[DAC1] = 1'b1;
    end

    //////////////////////////// DAC2 Register Flops //////////////////////////
    
    // DAC2 Write
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) reg_w[DAC2] <= 1'b0;
        else if(reg_w_clr[DAC2]) reg_w[DAC2] = 1'b0;
        else if(DAC2_w) reg_w[DAC2] = 1'b1;
    end

    //////////////////////////// DAC3 Register Flops //////////////////////////

    // DAC3 Write
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) reg_w[DAC3] <= 1'b0;
        else if(reg_w_clr[DAC3]) reg_w[DAC3] = 1'b0;
        else if(DAC3_w) reg_w[DAC3] = 1'b1;
    end

    //////////////////////////// DAC4 Register Flops //////////////////////////

    // DAC4 Write
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) reg_w[DAC4] <= 1'b0;
        else if(reg_w_clr[DAC4]) reg_w[DAC4] = 1'b0;
        else if(DAC4_w) reg_w[DAC4] = 1'b1;
    end

    //////////////////////////// DAC5 Register Flops //////////////////////////

    // DAC5 Write
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) reg_w[DAC5] <= 1'b0;
        else if(reg_w_clr[DAC5]) reg_w[DAC5] = 1'b0;
        else if(DAC5_w) reg_w[DAC5] = 1'b1;
    end

    //////////////////////////// DAC6 Register Flops //////////////////////////
    
    // DAC6 Write
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) reg_w[DAC6] <= 1'b0;
        else if(reg_w_clr[DAC6]) reg_w[DAC6] = 1'b0;
        else if(DAC6_w) reg_w[DAC6] = 1'b1;
    end

    //////////////////////////// DAC7 Register Flops //////////////////////////

    // DAC7 Write
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) reg_w[DAC7] <= 1'b0;
        else if(reg_w_clr[DAC7]) reg_w[DAC7] = 1'b0;
        else if(DAC7_w) reg_w[DAC7] = 1'b1;
    end

    //////////////////////////// State Machine Flops //////////////////////////

    // QUEUE STATE Flip Flop
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) state <= IDLE;
        else state <= next_state;
    end

    // TX STATE Flip Flop
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) tx_state <= IDLE;
        else if(save_state) tx_state <= return_state;
    end

    // TX DATA Flip Flop
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) tx_data <= 24'h000000;
        else if(save_data) tx_data <= tx_reg_data;
    end

    // Round Robin style state machine where every regiseter is
    // given a chance to tx before the same register may tx twice
    // A write flag set on any register begins a linear search through
    // the registers and all registers waiting with valid tx data
    // will be transmitted. The search completes when there are
    // no register write flags set
    
    always @(*) begin
        next_state = IDLE;
        return_state = IDLE;
        save_state = 1'b0;
        save_data = 1'b0;
        tx_reg_data = 24'h000000;
        tx_trig = 1'b0;
        reg_w_clr = 16'h0000;

        case(state) 
            // QUEUE STATE MACHINE
            IDLE: begin
                if(reg_queue) next_state = SYNC;
            end
            SYNC: begin
                if(reg_w[SYNC]) begin // SYNC has data that needs to be transmitted
                    next_state = TX_START;
                    return_state = CONFIG;
                    save_state = 1'b1;
                    save_data = 1'b1;
                    reg_w_clr[SYNC] = 1'b1;
                    tx_reg_data = {4'h0, SYNC, SYNC_data};
                end
                else if(reg_queue) next_state = CONFIG; // Other data still needs to be transmitted
            end
            CONFIG: begin
                if(reg_w[CONFIG]) begin // CONFIG has data that needs to be transmitted
                    next_state = TX_START;
                    return_state = GAIN;
                    save_state = 1'b1;
                    save_data = 1'b1;
                    reg_w_clr[CONFIG] = 1'b1;
                    tx_reg_data = {4'h0, CONFIG, CONFIG_data};
                end
                else if(reg_queue) next_state = GAIN; // Other data still needs to be transmitted
            end
            GAIN: begin
                if(reg_w[GAIN]) begin // GAIN has data that needs to be transmitted
                    next_state = TX_START;
                    return_state = TRIGGER;
                    save_state = 1'b1;
                    save_data = 1'b1;
                    reg_w_clr[GAIN] = 1'b1;
                    tx_reg_data = {4'h0, GAIN, GAIN_data};
                end
                else if(reg_queue) next_state = TRIGGER; // Other data still needs to be transmitted
            end
            TRIGGER: begin
                if(reg_w[TRIGGER]) begin // TRIGGER has data that needs to be transmitted
                    next_state = TX_START;
                    return_state = BRDCAST;
                    save_state = 1'b1;
                    save_data = 1'b1;
                    reg_w_clr[TRIGGER] = 1'b1;
                    tx_reg_data = {4'h0, TRIGGER, TRIGGER_data};
                end
                else if(reg_queue) next_state = BRDCAST; // Other data still needs to be transmitted
            end
            BRDCAST: begin
                if(reg_w[BRDCAST]) begin // BRDCAST has data that needs to be transmitted
                    next_state = TX_START;
                    return_state = DAC0;
                    save_state = 1'b1;
                    save_data = 1'b1;
                    reg_w_clr[BRDCAST] = 1'b1;
                    tx_reg_data = {4'h0, BRDCAST, BRDCAST_data};
                end
                else if(reg_queue) next_state = DAC0; // Other data still needs to be transmitted
            end
            DAC0: begin
                if(reg_w[DAC0]) begin // DAC0 has data that needs to be transmitted
                    next_state = TX_START;
                    return_state = DAC1;
                    save_state = 1'b1;
                    save_data = 1'b1;
                    reg_w_clr[DAC0] = 1'b1;
                    tx_reg_data = {4'h0, DAC0, DAC0_data};
                end
                else if(reg_queue) next_state = DAC1; // Other data still needs to be transmitted
            end
            DAC1: begin
                if(reg_w[DAC1]) begin // DAC1 has data that needs to be transmitted
                    next_state = TX_START;
                    return_state = DAC2;
                    save_state = 1'b1;
                    save_data = 1'b1;
                    reg_w_clr[DAC1] = 1'b1;
                    tx_reg_data = {4'h0, DAC1, DAC1_data};
                end
                else if(reg_queue) next_state = DAC2; // Other data still needs to be transmitted
            end
            DAC2: begin
                if(reg_w[DAC2]) begin // DAC2 has data that needs to be transmitted
                    next_state = TX_START;
                    return_state = DAC3;
                    save_state = 1'b1;
                    save_data = 1'b1;
                    reg_w_clr[DAC2] = 1'b1;
                    tx_reg_data = {4'h0, DAC2, DAC2_data};
                end
                else if(reg_queue) next_state = DAC3; // Other data still needs to be transmitted
            end
            DAC3: begin
                if(reg_w[DAC3]) begin // DAC3 has data that needs to be transmitted
                    next_state = TX_START;
                    return_state = DAC4;
                    save_state = 1'b1;
                    save_data = 1'b1;
                    reg_w_clr[DAC3] = 1'b1;
                    tx_reg_data = {4'h0, DAC3, DAC3_data};
                end
                else if(reg_queue) next_state = DAC4; // Other data still needs to be transmitted
            end
            DAC4: begin
                if(reg_w[DAC4]) begin // DAC4 has data that needs to be transmitted
                    next_state = TX_START;
                    return_state = DAC5;
                    save_state = 1'b1;
                    save_data = 1'b1;
                    reg_w_clr[DAC4] = 1'b1;
                    tx_reg_data = {4'h0, DAC4, DAC4_data};
                end
                else if(reg_queue) next_state = DAC5; // Other data still needs to be transmitted
            end
            DAC5: begin
                if(reg_w[DAC5]) begin // DAC5 has data that needs to be transmitted
                    next_state = TX_START;
                    return_state = DAC6;
                    save_state = 1'b1;
                    save_data = 1'b1;
                    reg_w_clr[DAC5] = 1'b1;
                    tx_reg_data = {4'h0, DAC5, DAC5_data};
                end
                else if(reg_queue) next_state = DAC6; // Other data still needs to be transmitted
            end
            DAC6: begin
                if(reg_w[DAC6]) begin // DAC6 has data that needs to be transmitted
                    next_state = TX_START;
                    return_state = DAC7;
                    save_state = 1'b1;
                    save_data = 1'b1;
                    reg_w_clr[DAC6] = 1'b1;
                    tx_reg_data = {4'h0, DAC6, DAC6_data};
                end
                else if(reg_queue) next_state = DAC7; // Other data still needs to be transmitted
            end
            DAC7: begin
                if(reg_w[DAC7]) begin // DAC7 has data that needs to be transmitted
                    next_state = TX_START;
                    return_state = SYNC;
                    save_state = 1'b1;
                    save_data = 1'b1;
                    reg_w_clr[DAC7] = 1'b1;
                    tx_reg_data = {4'h0, DAC7, DAC7_data};
                end
                else if(reg_queue) next_state = SYNC; // Other data still needs to be transmitted
            end

            // TX STATE MACHINE
            TX_START: begin
                tx_trig = 1;
                next_state = TX_WAIT;
            end
            TX_WAIT: begin
                if(tx_done) begin
                    save_data = 1'b1;
                    tx_reg_data = 24'h000000;
                    next_state = tx_state;
                end
                else next_state = TX_WAIT;
            end
            default: next_state = IDLE;
        endcase
    end   
endmodule
