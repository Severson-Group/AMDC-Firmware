module SPI_TX(clk, rst_n, trig, tx_data, CS, SCLK, MOSI, done);

    input wire clk, rst_n; // clk is 200MHz
    input wire trig;    // Begin Transmission
    input wire [23:0] tx_data; // Data to transmit

    output reg SCLK, MOSI; // Generated SPI signals
    output reg CS, done;    // Transmission completed

    reg [1:0] state, next_state;
    reg [2:0] div_cnt; // Divides the clk by 8 to reach a 25MHz SCLK
    reg [4:0] bit_cnt; // Counts the 24 bit for tx
    reg [23:0] shift;

    // Flags
    reg cnt_f, rst_f, CS_f;

    localparam IDLE = 2'b00;
    localparam TX = 2'b01;
    localparam HOLD = 2'b11;
    localparam LAST_BIT = 5'd24;
    localparam FALL = 3'b000;
    localparam RISE = 3'b100;

    // State FF
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) state <= IDLE;
        else state <= next_state;
    end

    // Div counter FF
    always @(posedge clk) begin
        if(rst_f) div_cnt <= 3'b100; // SCLK starts high
        else div_cnt <= div_cnt + 1'b1;
    end

    // Bit counter FF
    always @(posedge clk) begin
        if(rst_f) bit_cnt <= 5'b00000;
        else if (cnt_f) bit_cnt <= bit_cnt + 1'b1;
    end

    // Shift FF
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) shift <= 24'h000000;
        else if(trig) shift <= tx_data;
        else if(cnt_f) begin
            shift <= {shift[22:0], 1'b0};
        end
    end
    
    // SCLK FF
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) SCLK <= 1'b1;
        else SCLK <= div_cnt[2];
    end
    
    // MOSI FF
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) MOSI <= 1'b0;
        else MOSI <= shift[23];
    end

    // CS FF
    always @(posedge clk, negedge rst_n) begin
        if(!rst_n) CS <= 1'b1;
        else CS <= CS_f;
    end

    always @(*) begin
        // Defaults
        CS_f = 1'b1;
        cnt_f = 1'b0;
        rst_f = 1'b0;
        next_state = IDLE;
        done = 1'b1;

        case (state) 
            IDLE: begin // Watiting to start a tx
                rst_f = 1'b1;
                if(trig) next_state = TX;
            end
            TX: begin // Tx each bit
                done = 1'b0;
                CS_f = 1'b0;
                cnt_f = div_cnt == FALL ? 1'b1 : 1'b0; // Falling edge of SCLK
                if(bit_cnt == LAST_BIT) next_state = HOLD;
                else next_state = TX;
            end
            HOLD: begin // Hold CS low until rising edge of SCLK
                done = 1'b0;
                CS_f = 1'b0;
                if(div_cnt == RISE) next_state = IDLE;
                else next_state = HOLD;
            end
            default: next_state = IDLE;
        endcase
    end



endmodule
