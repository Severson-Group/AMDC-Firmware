`timescale 1ns / 1ps

module pwm_mux(
    input wire clk,
    input wire rst_n,
    input wire [47:0] all_i_PWM,
    output wire pwm_out,
    input wire [31:0] selector
);

reg my_pwm_out;
assign pwm_out = my_pwm_out;

always @(posedge clk)
	begin
      case (selector)
        32'd00   : my_pwm_out <= all_i_PWM[0];
        32'd01   : my_pwm_out <= all_i_PWM[1];
        32'd02   : my_pwm_out <= all_i_PWM[2];
        32'd03   : my_pwm_out <= all_i_PWM[3];
        32'd04   : my_pwm_out <= all_i_PWM[4];
        32'd05   : my_pwm_out <= all_i_PWM[5];
        32'd06   : my_pwm_out <= all_i_PWM[6];
        32'd07   : my_pwm_out <= all_i_PWM[7];
        32'd08   : my_pwm_out <= all_i_PWM[8];
        32'd09   : my_pwm_out <= all_i_PWM[9];
        32'd10   : my_pwm_out <= all_i_PWM[10];
        32'd11   : my_pwm_out <= all_i_PWM[11];
        32'd12   : my_pwm_out <= all_i_PWM[12];
        32'd13   : my_pwm_out <= all_i_PWM[13];
        32'd14   : my_pwm_out <= all_i_PWM[14];
        32'd15   : my_pwm_out <= all_i_PWM[15];
        32'd16   : my_pwm_out <= all_i_PWM[16];
        32'd17   : my_pwm_out <= all_i_PWM[17];
        32'd18   : my_pwm_out <= all_i_PWM[18];
        32'd19   : my_pwm_out <= all_i_PWM[19];
        32'd20   : my_pwm_out <= all_i_PWM[20];
        32'd21   : my_pwm_out <= all_i_PWM[21];
        32'd22   : my_pwm_out <= all_i_PWM[22];
        32'd23   : my_pwm_out <= all_i_PWM[23];
        32'd24   : my_pwm_out <= all_i_PWM[24];
        32'd25   : my_pwm_out <= all_i_PWM[25];
        32'd26   : my_pwm_out <= all_i_PWM[26];
        32'd27   : my_pwm_out <= all_i_PWM[27];
        32'd28   : my_pwm_out <= all_i_PWM[28];
        32'd29   : my_pwm_out <= all_i_PWM[29];
        32'd30   : my_pwm_out <= all_i_PWM[30];
        32'd31   : my_pwm_out <= all_i_PWM[31];
        32'd32   : my_pwm_out <= all_i_PWM[32];
        32'd33   : my_pwm_out <= all_i_PWM[33];
        32'd34   : my_pwm_out <= all_i_PWM[34];
        32'd35   : my_pwm_out <= all_i_PWM[35];
        32'd36   : my_pwm_out <= all_i_PWM[36];
        32'd37   : my_pwm_out <= all_i_PWM[37];
        32'd38   : my_pwm_out <= all_i_PWM[38];
        32'd39   : my_pwm_out <= all_i_PWM[39];
        32'd40   : my_pwm_out <= all_i_PWM[40];
        32'd41   : my_pwm_out <= all_i_PWM[41];
        32'd42   : my_pwm_out <= all_i_PWM[42];
        32'd43   : my_pwm_out <= all_i_PWM[43];
        32'd44   : my_pwm_out <= all_i_PWM[44];
        32'd45   : my_pwm_out <= all_i_PWM[45];
        32'd46   : my_pwm_out <= all_i_PWM[46];
        32'd47   : my_pwm_out <= all_i_PWM[47];
        default  : my_pwm_out <= 0;
      endcase
end

endmodule
