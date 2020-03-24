`timescale 1ns / 1ps

module tb_master();

reg clk, rst_n;
reg [15:0] tx_distance;
reg [1:0] tx_error;
reg tx_start;
wire tx_done;
wire tx_dout;

wire dout;

wire [15:0] rx_distance;
wire [1:0] rx_error;
wire rx_fresh;


integer i;
integer rx_error_count;
integer dt_timer;

initial begin
	// Set the inputs
	tx_start = 0;
	rx_error_count = 0;

	clk	= 1'b0;
	rst_n = 1'b1;
	
	@(posedge clk);
	@(posedge clk);
	
	rst_n = 1'b0;
	@(posedge clk);
	@(negedge clk);
	rst_n = 1'b1;
	
	// Send multiple packets of data out	
	for (i = 0; i < 50; i=i+1) begin
		dt_timer = 0;
		// Dead-time should be (period of tx) - (length of tx)
		//
		// If it sends data every 1e6 = period_us, baud_rate = 921600, clk_ns = 5
		// deadtime = (period_us - (30 * 1 / baud_rate * 1e9)) / clk_ns
		// = 193490 cycles
		while (dt_timer < 32'd193490) begin
			dt_timer += 1;
			@(posedge clk);
		end
		
		// Generate random data...
		tx_distance = $urandom_range(0, 65535);
		tx_error = $urandom_range(0, 3);
		
		$display("txing: %H, %H", tx_distance, tx_error);
		
		tx_start = 1;
		@(posedge clk);
		tx_start = 0;
		
		// Wait for it to be sent
		@(posedge clk);
		while (!tx_done) @(posedge clk);
		
		// The data just finished sending over the UART line,
		// so our rx should have the tx data!
		if (tx_distance != rx_distance || tx_error != rx_error) begin
			$display("distance\t\ttx: %H\trx: %H", tx_distance, rx_distance);
			$display("error\t\ttx: %H\trx: %H", tx_error, rx_error);
			rx_error_count += 1;
		end
	end
	
	$display("Simulation done, # of rx errors: %d", rx_error_count);
	$stop;
end

assign dout = tx_done ? 1'b1 : tx_dout;


always
	#5 clk <= ~clk;

ild1420_tx iDUT_tx(
	.clk(clk),
	.rst_n(rst_n),
	.distance(tx_distance),
	.error(tx_error),
	.start(tx_start),
	.dout(tx_dout),
	.done(tx_done)
);

ild1420_rx iDUT_rx(
	.clk(clk),
	.rst_n(rst_n),
	.distance(rx_distance),
	.error(rx_error),
	.fresh(rx_fresh),
	.din(dout)
);

endmodule