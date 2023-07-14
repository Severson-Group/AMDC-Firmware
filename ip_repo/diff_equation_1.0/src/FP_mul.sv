module FP_mul(A, B, OUT);
	// FP format
	// SEEE EEEE EMMM MMMM MMMM MMMM MMMM MMMM
	// value = (-1)^S * 2^(E-127) * (1.M)
	// when |E = 0, value = (-1)^S * 2^(E-127) * (0.M)
	// +0 = 32'h0000_0000, -0 = 32'h8000_0000
	// +INF = 32'h7F80_0000, -INF = 32'hFF80_0000

	input [31:0] A;					// FP value input
	input [31:0] B;					// FP value input
	output [31:0] OUT;				// the product of A*B

	logic    	 SA, SB, SO;		// sign bit
	logic [7:0]  EA, EB, EO;		// exponent bits
	logic [23:0] MA, MB, MO;		// mantissa bits

	logic [47:0] prod_M;			// product of mantissas (after 0 or 1 expansion)
	logic [4:0] shift_amount;		// essentially, this is the count of trailing zeros for prod_M[47:24]
	logic [47:0] prod_M_shifted;	// left shift prod_M tring to make the MSB set. Shift amount <= 24

	logic ZERO;						// result is +0 or -0
	logic INF;						// result is +INF or -INF
	logic NaN;						// result is NaN

	logic DENORMALIZED;				// set when product is denormalized
	logic NORMALIZABLE;				// when only one input is denormalized, there is a chance that the produt could be normalized
	logic [4:0] room_for_denormalization;		// when product is smaller than 2^(-126), this indicate the capability of going further down from -126
	logic [4:0] need_for_denormalization;		// when product is smaller than 2^(-126), this indicate the need of going further down from -126
	logic [22:0] denormalized_MO;				// denormalized mantissa output

	// extract parts from input FP number
	assign SA = A[31];
	assign SB = B[31];
	assign EA = A[30:23];
	assign EB = B[30:23];

	// appending mantissa with 1 when input is normalized, 0 when input is denormalized
	assign MA = {|EA, A[22:0]};			// FP value is denormalized when E = 0
	assign MB = {|EB, B[22:0]};			// FP value is denormalized when E = 0

	// result is a ZERO if any input is a ZERO, or if result's exponent is too small
	assign ZERO = ~|A[30:0] || ~|B[30:0] || ({1'b0, EA} + {1'b0, EB} < 9'h080 - room_for_denormalization);

	// need for denormalization is how far the product's exponent below -126, if one of EA and EB is already denormalized, restore 1 due to -127 is essentially -126 for E
	assign need_for_denormalization = 9'h07F - {1'b0, EA} - {1'b0, EB} - ((~|EA) ^ (~|EB));

	// room for denormalization is how much can prod_M be shifted to the right before becoming ZERO
	assign room_for_denormalization = 5'h18 - shift_amount;

	// get denormalized MO according to the needed amount
	assign denormalized_MO = prod_M[46:24] >> need_for_denormalization;

	// result is a INF is any input is a INF, or if result's exponent value is larger than 127
	// this happens when (EA - 127) + (EB - 127) > 127, which is equivalent to EA + EB > 381
	assign INF = (&EA && ~|A[22:0]) || (&EB && ~|B[22:0]) || ({1'b0, EA} + {1'b0, EB} > 9'h17D);

	// result is NaN if any input is NaN or ZERO and INF are multiplied together
	assign NaN = (&EA && |A[22:0]) || (&EB && |B[22:0]) || (ZERO && INF);

	// generate sign of result
	assign SO = SA ^ SB;

	// product is denormalized when smaller than 2^(-126)
	assign DENORMALIZED = {1'b0, EA} + {1'b0, EB} < 9'h07F || (({1'b0, EA} + {1'b0, EB} == 9'h07F) && ~prod_M[47]);

	// when only one input is denormalized, check if the product could be normalized (>= 2^(-126))
	assign NORMALIZABLE = (~|EA && ((EB - shift_amount) > 8'd127)) || (~|EB && ((EA - shift_amount) > 8'd127));

	// generate EO according to if the product is denormalized
	assign EO = DENORMALIZED ? 8'h00 :		// if product is denormalized, just let EO = 0
				NORMALIZABLE ? (EA + EB + (prod_M_shifted[47] ? 8'h01 : 8'h00) - 8'd126 - shift_amount) :		// if only one input is denormalized, but product is not denormalized, normalize raw product by left shifts
				(EA + EB + (prod_M[47] ? 8'h01 : 8'h00) - 8'd127);			// if denormalization is out of the table, handle simple exponent addition

	// unsigned raw product of mantissas
	assign prod_M = MA * MB;

	// generate MO according to if the product is denormalized
	assign MO = DENORMALIZED ? (!ZERO ? denormalized_MO : 23'h000000) :		// when product is denormalized, pick MO according to if the product is ZERO
				NORMALIZABLE ? prod_M_shifted[47:24] :						// when product is nomalizable, pick left-shifted prodM
				prod_M[47] ? prod_M[47:24] : prod_M[46:23];					// if denormalization is out of the table, handle simple mantissa prodM


	// find the count of trailing zeros for prod_M[47:24]
	always_comb begin
		casex(prod_M[47:24])
			24'b1xxx_xxxx_xxxx_xxxx_xxxx_xxxx: shift_amount = 5'h00;
			24'b01xx_xxxx_xxxx_xxxx_xxxx_xxxx: shift_amount = 5'h01;
			24'b001x_xxxx_xxxx_xxxx_xxxx_xxxx: shift_amount = 5'h02;
			24'b0001_xxxx_xxxx_xxxx_xxxx_xxxx: shift_amount = 5'h03;
			24'b0000_1xxx_xxxx_xxxx_xxxx_xxxx: shift_amount = 5'h04;
			24'b0000_01xx_xxxx_xxxx_xxxx_xxxx: shift_amount = 5'h05;
			24'b0000_001x_xxxx_xxxx_xxxx_xxxx: shift_amount = 5'h06;
			24'b0000_0001_xxxx_xxxx_xxxx_xxxx: shift_amount = 5'h07;
			24'b0000_0000_1xxx_xxxx_xxxx_xxxx: shift_amount = 5'h08;
			24'b0000_0000_01xx_xxxx_xxxx_xxxx: shift_amount = 5'h09;
			24'b0000_0000_001x_xxxx_xxxx_xxxx: shift_amount = 5'h0A;
			24'b0000_0000_0001_xxxx_xxxx_xxxx: shift_amount = 5'h0B;
			24'b0000_0000_0000_1xxx_xxxx_xxxx: shift_amount = 5'h0C;
			24'b0000_0000_0000_01xx_xxxx_xxxx: shift_amount = 5'h0D;
			24'b0000_0000_0000_001x_xxxx_xxxx: shift_amount = 5'h0E;
			24'b0000_0000_0000_0001_xxxx_xxxx: shift_amount = 5'h0F;
			24'b0000_0000_0000_0000_1xxx_xxxx: shift_amount = 5'h10;
			24'b0000_0000_0000_0000_01xx_xxxx: shift_amount = 5'h11;
			24'b0000_0000_0000_0000_001x_xxxx: shift_amount = 5'h12;
			24'b0000_0000_0000_0000_0001_xxxx: shift_amount = 5'h13;
			24'b0000_0000_0000_0000_0000_1xxx: shift_amount = 5'h14;
			24'b0000_0000_0000_0000_0000_01xx: shift_amount = 5'h15;
			24'b0000_0000_0000_0000_0000_001x: shift_amount = 5'h16;
			24'b0000_0000_0000_0000_0000_0001: shift_amount = 5'h17;
			default:	shift_amount = 5'h18;
		endcase
	end
	// shift accordingly
	assign prod_M_shifted = prod_M << shift_amount;

	// generate output, if it's not a special value, concatenate SO, EO, and MO (MSB of MO is inplicit)
	assign OUT = NaN ? {SO, 8'hFF, 23'hFFFFFF} :		// if any of the lower 23 bits is set while E = 8'hFF, value is NaN, so we just pick this one
				 ZERO ? {SO, 8'h00, 23'h000000} :
				 INF ? {SO, 8'hFF, 23'h000000} :
				 {SO, EO, MO[22:0]};					// if result is not a special value above, do simple concatenation

endmodule