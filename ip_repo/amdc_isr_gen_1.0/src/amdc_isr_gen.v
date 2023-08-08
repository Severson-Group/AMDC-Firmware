module amdc_isr_gen(clk, rst_n, user_ratio, pwm_carrier_high, pwm_carrier_low, interrupt);

input clk, rst_n;
input wire [15:0] user_ratio;
input wire pwm_carrier_high, pwm_carrier_low;
output reg interrupt;

reg [15:0] count;

always @(posedge clk, negedge rst_n) begin
    if (!rst_n) begin
        count <= 0;
        interrupt <= 0;
    end
    else if (user_ratio == count) begin
        count <= 0;
        interrupt <= 1;
    end
    else if (pwm_carrier_high || pwm_carrier_low) begin
        count <= count + 1'b1;
        interrupt <= 0;
    end
end

endmodule