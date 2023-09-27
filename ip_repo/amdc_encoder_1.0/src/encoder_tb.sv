`timescale 1ns / 1ps

`define PULSES_PER_REVOLUTION_BITS (32'd12)

module encoder_tb();

logic clk, rst_n;
logic A, B, Z;
logic [31:0] pulses_per_rev_bits;
logic [7:0] divider;
logic [15:0] carrier_max;

logic pwm_carrier_low, pwm_carrier_high;
logic [31:0] counter;
logic [31:0] position;
logic [31:0] steps_synced;
logic [31:0] position_synced;
logic [15:0] carrier;

logic [31:0] value_steps, value_pos;

triangle_carrier triangleWaves(
	.clk(clk),
	.rst_n(rst_n),
	.divider(divider),
	.carrier_max(carrier_max),
	.carrier(carrier),
	.carrier_high(pwm_carrier_high),
	.carrier_low(pwm_carrier_low)
);

encoder iDUT(
	.clk(clk),
	.rst_n(rst_n),
	.A(A),
	.B(B),
	.Z(Z),
	.counter(counter),
	.pwm_carrier_high(pwm_carrier_high),
	.pwm_carrier_low(pwm_carrier_low),
	.position(position),
	.pulses_per_rev(`PULSES_PER_REVOLUTION_BITS),
	.steps_synced(steps_synced),
	.position_synced(position_synced)
);

// task for testing, call from initial block?
task test_updates;
	input int dir;
	input int num_revs;
	input int cycles;
	input int speed;
	
	begin
		// Simulate revs
		repeat (num_revs) begin
			Z = 1;
			if (dir) A = 1; else B = 1;
			repeat (speed) @(posedge clk);
			Z = 0;
			if (dir) B = 1; else A = 1;
			repeat (speed) @(posedge clk);
			if (dir) A = 0; else B = 0;
			repeat (speed) @(posedge clk);
			if (dir) B = 0; else A = 0;
			repeat (speed) @(posedge clk);


			// Simulate (`cycles_per_rev` - 1) more cycles (or 4x pulses)
			repeat (cycles - 1) begin
				if (dir) A = 1; else B = 1;
				repeat (speed) @(posedge clk);
				if (dir) B = 1; else A = 1;
				repeat (speed) @(posedge clk);
				if (dir) A = 0; else B = 0;
				repeat (speed) @(posedge clk);
				if (dir) B = 0; else A = 0;
				repeat (speed) @(posedge clk);
			
			end
		end
	end
endtask

initial begin
	clk = 1'b0;
	rst_n = 1'b1;
	
	A = 0;
	B = 0;
	Z = 0;
	divider = 8'b0;
	carrier_max = 16'd1000;
	
	@(posedge clk);
	rst_n = 1'b0;
	
	@(posedge clk);
	@(negedge clk);
	rst_n = 1'b1;

	// Simulate some amount of cycles before hitting
	// Z pulse; the counter should hold 20 steps here
	repeat (5) begin
		A = 1;
		repeat (5) @(posedge clk);
		B = 1;
		repeat (5) @(posedge clk);
		A = 0;
		repeat (5) @(posedge clk);
		B = 0;
		repeat (5) @(posedge clk);
	end

	test_updates(1, 1, (1 << `PULSES_PER_REVOLUTION_BITS) / 4, 5);	// 2,400,000 RPM
	$display("2,400,000 RPM | Synced Steps: %d | Synced Position: %d | Inst. Steps: %d | Inst. Position: %d",steps_synced, position_synced, counter, position);
	test_updates(0, 1, (1 << `PULSES_PER_REVOLUTION_BITS) / 4, 60);	// 200,000 RPM
	$display("200,000 RPM   | Synced Steps: %d | Synced Position: %d | Inst. Steps: %d | Inst. Position: %d",steps_synced, position_synced, counter, position);
	test_updates(1, 1, (1 << `PULSES_PER_REVOLUTION_BITS) / 4, 120);	// 100,000 RPM
	$display("100,000 RPM   | Synced Steps: %d | Synced Position: %d | Inst. Steps: %d | Inst. Position: %d",steps_synced, position_synced, counter, position);
	test_updates(1, 1, (1 << `PULSES_PER_REVOLUTION_BITS) / 4, 240);	// 50,000 RPM
	$display("50,000 RPM    | Synced Steps: %d | Synced Position: %d | Inst. Steps: %d | Inst. Position: %d",steps_synced, position_synced, counter, position);
	test_updates(1, 1, (1 << `PULSES_PER_REVOLUTION_BITS) / 4, 1200);	// 10,000 RPM
	$display("10,000 RPM    | Synced Steps: %d | Synced Position: %d | Inst. Steps: %d | Inst. Position: %d",steps_synced, position_synced, counter, position);
	test_updates(1, 1, (1 << `PULSES_PER_REVOLUTION_BITS) / 4, 2400);	// 5,000 RPM
	$display("5,000 RPM     | Synced Steps: %d | Synced Position: %d | Inst. Steps: %d | Inst. Position: %d",steps_synced, position_synced, counter, position);

	$display("All tests passed!");
	$stop();

end

// *********************************************************
// Check that the synced values are unchanging between carrier 
// high and low signal assertion
// *********************************************************

// Falling edge detector for carrier high signal
logic fall_edge_high, carrier_high_ff;
always_ff @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		carrier_high_ff <= 1'b0;
	else
		carrier_high_ff <= pwm_carrier_high;
end

assign fall_edge_high = (~pwm_carrier_high) & carrier_high_ff;

// Falling edge detector for carrier low signal
logic fall_low_low, carrier_low_ff;
always_ff @(posedge clk, negedge rst_n) begin
	if (!rst_n)
		carrier_low_ff <= 1'b0;
	else
		carrier_low_ff <= pwm_carrier_low;
end

assign fall_edge_low = (~pwm_carrier_low) & carrier_low_ff;

// Check that values are held
always_ff @(posedge clk) begin
	if (fall_edge_high) begin
		value_steps <= steps_synced;
		value_pos <= position_synced;
		if (pwm_carrier_low) begin
			// Check that synced values are retained
			if ((value_steps !== steps_synced) || (value_pos !== position_synced)) begin
				$display("Updates not synced!");
				$stop();
			end
		end
	end else if (fall_edge_low) begin
		value_steps <= steps_synced;
		value_pos <= position_synced;
		if (pwm_carrier_high) begin
			// Check that synced values are retained
			if ((value_steps !== steps_synced) || (value_pos !== position_synced)) begin
				$display("Updates not synced!");
				$stop();
			end
		end
	end
end


always
	#2.5 clk = ~clk; // 200 MHz clock with 5 ns period

		
endmodule
