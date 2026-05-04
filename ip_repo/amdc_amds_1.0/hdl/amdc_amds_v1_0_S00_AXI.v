
`timescale 1 ns / 1 ps

    module amdc_amds_v1_0_S00_AXI #
    (
        // Users to add parameters here

        // User parameters ends
        // Do not modify the parameters beyond this line

        // Width of S_AXI data bus
        parameter integer C_S_AXI_DATA_WIDTH    = 32,
        // Width of S_AXI address bus
        parameter integer C_S_AXI_ADDR_WIDTH    = 7
    )
    (
        // Users to add ports here
        input wire enable,
        input wire trigger,
        input wire [1:0] amds_data,
        output wire sync_adc,
        output wire done,
        // User ports ends
        // Do not modify the ports beyond this line

        // Global Clock Signal
        input wire  S_AXI_ACLK,
        // Global Reset Signal. This Signal is Active LOW
        input wire  S_AXI_ARESETN,
        // Write address (issued by master, acceped by Slave)
        input wire [C_S_AXI_ADDR_WIDTH-1 : 0] S_AXI_AWADDR,
        // Write channel Protection type. This signal indicates the
            // privilege and security level of the transaction, and whether
            // the transaction is a data access or an instruction access.
        input wire [2 : 0] S_AXI_AWPROT,
        // Write address valid. This signal indicates that the master signaling
            // valid write address and control information.
        input wire  S_AXI_AWVALID,
        // Write address ready. This signal indicates that the slave is ready
            // to accept an address and associated control signals.
        output wire  S_AXI_AWREADY,
        // Write data (issued by master, acceped by Slave) 
        input wire [C_S_AXI_DATA_WIDTH-1 : 0] S_AXI_WDATA,
        // Write strobes. This signal indicates which byte lanes hold
            // valid data. There is one write strobe bit for each eight
            // bits of the write data bus.    
        input wire [(C_S_AXI_DATA_WIDTH/8)-1 : 0] S_AXI_WSTRB,
        // Write valid. This signal indicates that valid write
            // data and strobes are available.
        input wire  S_AXI_WVALID,
        // Write ready. This signal indicates that the slave
            // can accept the write data.
        output wire  S_AXI_WREADY,
        // Write response. This signal indicates the status
            // of the write transaction.
        output wire [1 : 0] S_AXI_BRESP,
        // Write response valid. This signal indicates that the channel
            // is signaling a valid write response.
        output wire  S_AXI_BVALID,
        // Response ready. This signal indicates that the master
            // can accept a write response.
        input wire  S_AXI_BREADY,
        // Read address (issued by master, acceped by Slave)
        input wire [C_S_AXI_ADDR_WIDTH-1 : 0] S_AXI_ARADDR,
        // Protection type. This signal indicates the privilege
            // and security level of the transaction, and whether the
            // transaction is a data access or an instruction access.
        input wire [2 : 0] S_AXI_ARPROT,
        // Read address valid. This signal indicates that the channel
            // is signaling valid read address and control information.
        input wire  S_AXI_ARVALID,
        // Read address ready. This signal indicates that the slave is
            // ready to accept an address and associated control signals.
        output wire  S_AXI_ARREADY,
        // Read data (issued by slave)
        output wire [C_S_AXI_DATA_WIDTH-1 : 0] S_AXI_RDATA,
        // Read response. This signal indicates the status of the
            // read transfer.
        output wire [1 : 0] S_AXI_RRESP,
        // Read valid. This signal indicates that the channel is
            // signaling the required read data.
        output wire  S_AXI_RVALID,
        // Read ready. This signal indicates that the master can
            // accept the read data and response information.
        input wire  S_AXI_RREADY
    );

    // User registers    
    reg [31:0] adc_dout0;
    reg [31:0] adc_dout1;
    reg [31:0] adc_dout2;
    reg [31:0] adc_dout3;
    reg [31:0] adc_dout4;
    reg [31:0] adc_dout5;
    reg [31:0] adc_dout6;
    reg [31:0] adc_dout7;
    reg [31:0] adc_dout8;
    reg [31:0] adc_dout9;
    reg [31:0] adc_dout10;
    reg [31:0] adc_dout11;
    reg [31:0] adc_dout12;
    reg [31:0] adc_dout13;
    reg [31:0] adc_dout14;
    reg [31:0] adc_dout15;
    reg [31:0] adc_dout16;
    reg [31:0] adc_dout17;
    reg [31:0] adc_dout18;
    reg [31:0] adc_dout19;
    reg [31:0] adc_dout20;
    reg [31:0] adc_dout21;
    reg [31:0] adc_dout22;
    reg [31:0] adc_dout23;
    reg [15:0] trigger_to_fe0_timer, trigger_to_fe1_timer;
    reg [31:0] ch_valid_reg;
    reg [31:0] valid_reg;
    reg [31:0] corrupt_reg;
    reg [31:0] timeout_reg;
    
    // Debugging counters: Valid and Corrupt Counters are in each instance of adc_uart
    //                     Timeout Counters are just below in this module
    wire [15:0] counter_line0_bytes_valid;
    wire [15:0] counter_line1_bytes_valid;
    wire [15:0] counter_line0_bytes_corrupt;
    wire [15:0] counter_line1_bytes_corrupt;
    wire [15:0] counter_line0_bytes_timed_out; // byte timeout counters count the bytes timed out in uart_rx.v
    wire [15:0] counter_line1_bytes_timed_out;
    reg  [15:0] counter_line0_data_timed_out; // data timeout counters count the timeouts from the trigger to the first data line falling edge (ie if the AMDS is not plugged in)
    reg  [15:0] counter_line1_data_timed_out;

    // AXI4LITE signals
    reg [C_S_AXI_ADDR_WIDTH-1 : 0]     axi_awaddr;
    reg      axi_awready;
    reg      axi_wready;
    reg [1 : 0]     axi_bresp;
    reg      axi_bvalid;
    reg [C_S_AXI_ADDR_WIDTH-1 : 0]     axi_araddr;
    reg      axi_arready;
    reg [C_S_AXI_DATA_WIDTH-1 : 0]     axi_rdata;
    reg [1 : 0]     axi_rresp;
    reg      axi_rvalid;

    // Example-specific design signals
    // local parameter for addressing 32 bit / 64 bit C_S_AXI_DATA_WIDTH
    // ADDR_LSB is used for addressing 32/64 bit registers/memories
    // ADDR_LSB = 2 for 32 bits (n downto 2)
    // ADDR_LSB = 3 for 64 bits (n downto 3)
    localparam integer ADDR_LSB = (C_S_AXI_DATA_WIDTH/32) + 1;
    localparam integer OPT_MEM_ADDR_BITS = 4;
    //----------------------------------------------
    //-- Signals for user logic register space example
    //------------------------------------------------
    //-- Number of Slave Registers 32
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg0;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg1;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg2;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg3;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg4;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg5;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg6;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg7;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg8;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg9;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg10;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg11;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg12;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg13;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg14;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg15;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg16;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg17;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg18;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg19;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg20;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg21;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg22;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg23;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg24;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg25;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg26;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg27;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg28;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg29;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg30;
    reg [C_S_AXI_DATA_WIDTH-1:0]    slv_reg31;
    wire     slv_reg_rden;
    wire     slv_reg_wren;
    reg [C_S_AXI_DATA_WIDTH-1:0]     reg_data_out;
    integer     byte_index;
    reg     aw_en;

    // I/O Connections assignments

    assign S_AXI_AWREADY    = axi_awready;
    assign S_AXI_WREADY    = axi_wready;
    assign S_AXI_BRESP    = axi_bresp;
    assign S_AXI_BVALID    = axi_bvalid;
    assign S_AXI_ARREADY    = axi_arready;
    assign S_AXI_RDATA    = axi_rdata;
    assign S_AXI_RRESP    = axi_rresp;
    assign S_AXI_RVALID    = axi_rvalid;
    // Implement axi_awready generation
    // axi_awready is asserted for one S_AXI_ACLK clock cycle when both
    // S_AXI_AWVALID and S_AXI_WVALID are asserted. axi_awready is
    // de-asserted when reset is low.

    always @( posedge S_AXI_ACLK )
    begin
      if ( S_AXI_ARESETN == 1'b0 )
        begin
          axi_awready <= 1'b0;
          aw_en <= 1'b1;
        end 
      else
        begin    
          if (~axi_awready && S_AXI_AWVALID && S_AXI_WVALID && aw_en)
            begin
              // slave is ready to accept write address when 
              // there is a valid write address and write data
              // on the write address and data bus. This design 
              // expects no outstanding transactions. 
              axi_awready <= 1'b1;
              aw_en <= 1'b0;
            end
            else if (S_AXI_BREADY && axi_bvalid)
                begin
                  aw_en <= 1'b1;
                  axi_awready <= 1'b0;
                end
          else           
            begin
              axi_awready <= 1'b0;
            end
        end 
    end       

    // Implement axi_awaddr latching
    // This process is used to latch the address when both 
    // S_AXI_AWVALID and S_AXI_WVALID are valid. 

    always @( posedge S_AXI_ACLK )
    begin
      if ( S_AXI_ARESETN == 1'b0 )
        begin
          axi_awaddr <= 0;
        end 
      else
        begin    
          if (~axi_awready && S_AXI_AWVALID && S_AXI_WVALID && aw_en)
            begin
              // Write Address latching 
              axi_awaddr <= S_AXI_AWADDR;
            end
        end 
    end       

    // Implement axi_wready generation
    // axi_wready is asserted for one S_AXI_ACLK clock cycle when both
    // S_AXI_AWVALID and S_AXI_WVALID are asserted. axi_wready is 
    // de-asserted when reset is low. 

    always @( posedge S_AXI_ACLK )
    begin
      if ( S_AXI_ARESETN == 1'b0 )
        begin
          axi_wready <= 1'b0;
        end 
      else
        begin    
          if (~axi_wready && S_AXI_WVALID && S_AXI_AWVALID && aw_en )
            begin
              // slave is ready to accept write data when 
              // there is a valid write address and write data
              // on the write address and data bus. This design 
              // expects no outstanding transactions. 
              axi_wready <= 1'b1;
            end
          else
            begin
              axi_wready <= 1'b0;
            end
        end 
    end       

    // Implement memory mapped register select and write logic generation
    // The write data is accepted and written to memory mapped registers when
    // axi_awready, S_AXI_WVALID, axi_wready and S_AXI_WVALID are asserted. Write strobes are used to
    // select byte enables of slave registers while writing.
    // These registers are cleared when reset (active low) is applied.
    // Slave register write enable is asserted when valid address and data are available
    // and the slave is ready to accept the write address and write data.
    assign slv_reg_wren = axi_wready && S_AXI_WVALID && axi_awready && S_AXI_AWVALID;

    always @( posedge S_AXI_ACLK )
    begin
      if ( S_AXI_ARESETN == 1'b0 )
        begin
          slv_reg0 <= 0;
          slv_reg1 <= 0;
          slv_reg2 <= 0;
          slv_reg3 <= 0;
          slv_reg4 <= 0;
          slv_reg5 <= 0;
          slv_reg6 <= 0;
          slv_reg7 <= 0;
          slv_reg8 <= 0;
          slv_reg9 <= 0;
          slv_reg10 <= 0;
          slv_reg11 <= 0;
          slv_reg12 <= 0;
          slv_reg13 <= 0;
          slv_reg14 <= 0;
          slv_reg15 <= 0;
          slv_reg16 <= 0;
          slv_reg17 <= 0;
          slv_reg18 <= 0;
          slv_reg19 <= 0;
          slv_reg20 <= 0;
          slv_reg21 <= 0;
          slv_reg22 <= 0;
          slv_reg23 <= 0;
          slv_reg24 <= 0;
          slv_reg25 <= 0;
          slv_reg26 <= 0;
          slv_reg27 <= 0;
          slv_reg28 <= 0;
          slv_reg29 <= 0;
          slv_reg30 <= 0;
          slv_reg31 <= 0;
        end 
      else begin
        if (slv_reg_wren)
          begin
            case ( axi_awaddr[ADDR_LSB+OPT_MEM_ADDR_BITS:ADDR_LSB] )
              5'h0:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 0
                    slv_reg0[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'h1:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 1
                    slv_reg1[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'h2:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 2
                    slv_reg2[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'h3:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 3
                    slv_reg3[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'h4:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 4
                    slv_reg4[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'h5:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 5
                    slv_reg5[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'h6:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 6
                    slv_reg6[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'h7:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 7
                    slv_reg7[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'h8:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 8
                    slv_reg8[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'h9:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 9
                    slv_reg9[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'hA:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 10
                    slv_reg10[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'hB:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 11
                    slv_reg11[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'hC:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 12
                    slv_reg12[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'hD:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 13
                    slv_reg13[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'hE:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 14
                    slv_reg14[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'hF:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 15
                    slv_reg15[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'h10:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 16
                    slv_reg16[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'h11:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 17
                    slv_reg17[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end 
              5'h12:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 18
                    slv_reg18[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'h13:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 19
                    slv_reg19[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end
              5'h14:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 20
                    slv_reg20[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'h15:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 21
                    slv_reg21[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end 
              5'h16:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 22
                    slv_reg22[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'h17:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 23
                    slv_reg23[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end
              5'h18:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 24
                    slv_reg24[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'h19:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 25
                    slv_reg25[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end 
              5'h1A:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 26
                    slv_reg26[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'h1B:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 27
                    slv_reg27[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end
              5'h1C:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 28
                    slv_reg28[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'h1D:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 29
                    slv_reg29[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end 
              5'h1E:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 30
                    slv_reg30[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              5'h1F:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 31
                    slv_reg31[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end
              default : begin
                          slv_reg0 <= slv_reg0;
                          slv_reg1 <= slv_reg1;
                          slv_reg2 <= slv_reg2;
                          slv_reg3 <= slv_reg3;
                          slv_reg4 <= slv_reg4;
                          slv_reg5 <= slv_reg5;
                          slv_reg6 <= slv_reg6;
                          slv_reg7 <= slv_reg7;
                          slv_reg8 <= slv_reg8;
                          slv_reg9 <= slv_reg9;
                          slv_reg10 <= slv_reg10;
                          slv_reg11 <= slv_reg11;
                          slv_reg12 <= slv_reg12;
                          slv_reg13 <= slv_reg13;
                          slv_reg14 <= slv_reg14;
                          slv_reg15 <= slv_reg15;
                          slv_reg16 <= slv_reg16;
                          slv_reg17 <= slv_reg17;
                          slv_reg18 <= slv_reg18;
                          slv_reg19 <= slv_reg19;
                          slv_reg20 <= slv_reg20;
                          slv_reg21 <= slv_reg21;
                          slv_reg22 <= slv_reg22;
                          slv_reg23 <= slv_reg23;
                          slv_reg24 <= slv_reg24;
                          slv_reg25 <= slv_reg25;
                          slv_reg26 <= slv_reg26;
                          slv_reg27 <= slv_reg27;
                          slv_reg28 <= slv_reg28;
                          slv_reg29 <= slv_reg29;
                          slv_reg30 <= slv_reg30;
                          slv_reg31 <= slv_reg31;
                        end
            endcase
          end
      end
    end    

    // Implement write response logic generation
    // The write response and response valid signals are asserted by the slave 
    // when axi_wready, S_AXI_WVALID, axi_wready and S_AXI_WVALID are asserted.  
    // This marks the acceptance of address and indicates the status of 
    // write transaction.

    always @( posedge S_AXI_ACLK )
    begin
      if ( S_AXI_ARESETN == 1'b0 )
        begin
          axi_bvalid  <= 0;
          axi_bresp   <= 2'b0;
        end 
      else
        begin    
          if (axi_awready && S_AXI_AWVALID && ~axi_bvalid && axi_wready && S_AXI_WVALID)
            begin
              // indicates a valid write response is available
              axi_bvalid <= 1'b1;
              axi_bresp  <= 2'b0; // 'OKAY' response 
            end                   // work error responses in future
          else
            begin
              if (S_AXI_BREADY && axi_bvalid) 
                //check if bready is asserted while bvalid is high) 
                //(there is a possibility that bready is always asserted high)   
                begin
                  axi_bvalid <= 1'b0; 
                end  
            end
        end
    end   

    // Implement axi_arready generation
    // axi_arready is asserted for one S_AXI_ACLK clock cycle when
    // S_AXI_ARVALID is asserted. axi_awready is 
    // de-asserted when reset (active low) is asserted. 
    // The read address is also latched when S_AXI_ARVALID is 
    // asserted. axi_araddr is reset to zero on reset assertion.

    always @( posedge S_AXI_ACLK )
    begin
      if ( S_AXI_ARESETN == 1'b0 )
        begin
          axi_arready <= 1'b0;
          axi_araddr  <= 32'b0;
        end 
      else
        begin    
          if (~axi_arready && S_AXI_ARVALID)
            begin
              // indicates that the slave has acceped the valid read address
              axi_arready <= 1'b1;
              // Read address latching
              axi_araddr  <= S_AXI_ARADDR;
            end
          else
            begin
              axi_arready <= 1'b0;
            end
        end 
    end       

    // Implement axi_arvalid generation
    // axi_rvalid is asserted for one S_AXI_ACLK clock cycle when both 
    // S_AXI_ARVALID and axi_arready are asserted. The slave registers 
    // data are available on the axi_rdata bus at this instance. The 
    // assertion of axi_rvalid marks the validity of read data on the 
    // bus and axi_rresp indicates the status of read transaction.axi_rvalid 
    // is deasserted on reset (active low). axi_rresp and axi_rdata are 
    // cleared to zero on reset (active low).  
    always @( posedge S_AXI_ACLK )
    begin
      if ( S_AXI_ARESETN == 1'b0 )
        begin
          axi_rvalid <= 0;
          axi_rresp  <= 0;
        end 
      else
        begin    
          if (axi_arready && S_AXI_ARVALID && ~axi_rvalid)
            begin
              // Valid read data is available at the read data bus
              axi_rvalid <= 1'b1;
              axi_rresp  <= 2'b0; // 'OKAY' response
            end   
          else if (axi_rvalid && S_AXI_RREADY)
            begin
              // Read data is accepted by the master
              axi_rvalid <= 1'b0;
            end                
        end
    end    

    // Implement memory mapped register select and read logic generation
    // Slave register read enable is asserted when valid address is available
    // and the slave is ready to accept the read address.
    assign slv_reg_rden = axi_arready & S_AXI_ARVALID & ~axi_rvalid;
    always @(*)
    begin
          // Address decoding for reading registers
          case ( axi_araddr[ADDR_LSB+OPT_MEM_ADDR_BITS:ADDR_LSB] )
            5'h0   : reg_data_out <= adc_dout0;
            5'h1   : reg_data_out <= adc_dout1;
            5'h2   : reg_data_out <= adc_dout2;
            5'h3   : reg_data_out <= adc_dout3;
            5'h4   : reg_data_out <= adc_dout4;
            5'h5   : reg_data_out <= adc_dout5;
            5'h6   : reg_data_out <= adc_dout6;
            5'h7   : reg_data_out <= adc_dout7;
            5'h8   : reg_data_out <= adc_dout8;
            5'h9   : reg_data_out <= adc_dout9;
            5'hA   : reg_data_out <= adc_dout10;                    // slv_reg10
            5'hB   : reg_data_out <= adc_dout11;                    // slv_reg11
            5'hC   : reg_data_out <= adc_dout12;                    // slv_reg12
            5'hD   : reg_data_out <= adc_dout13;                    // slv_reg13
            5'hE   : reg_data_out <= adc_dout14;
            5'hF   : reg_data_out <= adc_dout15;
            5'h10  : reg_data_out <= adc_dout16;
            5'h11  : reg_data_out <= adc_dout17;
            5'h12  : reg_data_out <= adc_dout18;
            5'h13  : reg_data_out <= adc_dout19;
            5'h14  : reg_data_out <= adc_dout20;
            5'h15  : reg_data_out <= adc_dout21;
            5'h16  : reg_data_out <= adc_dout22;
            5'h17  : reg_data_out <= adc_dout23;
            5'h18  : reg_data_out <= {trigger_to_fe1_timer, trigger_to_fe0_timer};
            5'h19  : reg_data_out <= ch_valid_reg;
            5'h1A  : reg_data_out <= {counter_line1_bytes_valid, counter_line0_bytes_valid};
            5'h1B  : reg_data_out <= {counter_line1_bytes_corrupt, counter_line0_bytes_corrupt};
            5'h1C  : reg_data_out <= {counter_line1_bytes_timed_out, counter_line0_bytes_timed_out};
            5'h1D  : reg_data_out <= {counter_line1_data_timed_out, counter_line0_data_timed_out};
            5'h1E  : reg_data_out <= slv_reg30;                     // channel enable register
            5'h1F  : reg_data_out <= slv_reg31;
            default : reg_data_out <= 0;
          endcase
    end

    // Output register or memory read data
    always @( posedge S_AXI_ACLK )
    begin
      if ( S_AXI_ARESETN == 1'b0 )
        begin
          axi_rdata  <= 0;
        end 
      else
        begin    
          // When there is a valid read address (S_AXI_ARVALID) with 
          // acceptance of read address by the slave (axi_arready), 
          // output the read dada 
          if (slv_reg_rden)
            begin
              axi_rdata <= reg_data_out;     // register read data
            end   
        end
    end    

    // Add user logic here

    // ======
    // Construct SYNC_ADC interrupt signal that goes out of the physical port
    // to the AMDS, based on trigger and user enable bit
    // ======
    reg sync_adc_flop;
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            sync_adc_flop <= 1'b0;
        else if (trigger)
            sync_adc_flop <= ~sync_adc_flop;
    end
    
    assign sync_adc = (enable) ? sync_adc_flop : 1'b0;

    // The AMDS sensor interface should not pay attention to the
    // timing manager trigger unless it is enabled:
    wire enabled_trigger;
    assign enabled_trigger = trigger & enable;

    // This module listens to the two data inputs from the AMDS.
    // When a data line first goes low after a trigger event, this module
    // knows to expect a new data packet transmission. Therefore, it will
    // start a state machine internally to read each UART word.
    wire [11:0] is_dout0_valid, is_dout1_valid;
    wire [11:0] is_dout0_enabled, is_dout1_enabled;
    wire adc_uart0_done, assert_done_0;
    wire adc_uart1_done, assert_done_1;
    
    assign is_dout0_enabled = {slv_reg30[19:16], slv_reg30[11:8], slv_reg30[3:0]};
    assign is_dout1_enabled = {slv_reg30[23:20], slv_reg30[15:12], slv_reg30[7:4]};
   
    wire [15:0] my_adc_data0;
    wire [15:0] my_adc_data1;
    wire [15:0] my_adc_data2;
    wire [15:0] my_adc_data3;
    wire [15:0] my_adc_data4;
    wire [15:0] my_adc_data5;
    wire [15:0] my_adc_data6;
    wire [15:0] my_adc_data7;
    wire [15:0] my_adc_data8;
    wire [15:0] my_adc_data9;
    wire [15:0] my_adc_data10;
    wire [15:0] my_adc_data11;
    wire [15:0] my_adc_data12;
    wire [15:0] my_adc_data13;
    wire [15:0] my_adc_data14;
    wire [15:0] my_adc_data15;
    wire [15:0] my_adc_data16;
    wire [15:0] my_adc_data17;
    wire [15:0] my_adc_data18;
    wire [15:0] my_adc_data19;
    wire [15:0] my_adc_data20;
    wire [15:0] my_adc_data21;
    wire [15:0] my_adc_data22;
    wire [15:0] my_adc_data23;
    
    

    // Timing Out:
    // This is a tempermental thing to track... at what point do we give up?
    //  The state machine in adc_uart_rx begins when the first falling edge is detected
    //  on the data line, which is taken care of in this module by the start_rx signal.
    //  So this waiting period should timeout if the trigger_to_fe timer below goes too
    //  far beyond the expected value of about 5us (Although we will give it 10us, or about 
    //  2000 clock cycles, to avoid being too aggressive). We will just stop waiting_for_first_fe
    //  and increment the *data* timeout counter to show that all four packets failed to send.
    // There is also a timeout generated by the uart_rx state machine if it was told to expect
    //  a start bit, but it doesn't arrive after 2.5us. This is the "bytes_timed_out" counter
    // In the event of either timeout, the driver must say that it is done, so that the timing
    //  manager does not freeze.
    wire first_packet_timeout0, first_packet_timeout1;
    assign first_packet_timeout0 = (trigger_to_fe0_timer > 13'd6000);
    assign first_packet_timeout1 = (trigger_to_fe1_timer > 13'd6000);

    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            counter_line0_data_timed_out <= 16'b0;
        else if (first_packet_timeout0)
            counter_line0_data_timed_out <= counter_line0_data_timed_out + 1;
    end

    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            counter_line1_data_timed_out <= 16'b0;
        else if (first_packet_timeout1)
            counter_line1_data_timed_out <= counter_line1_data_timed_out + 1;
    end

    // Start receiving data (assert start_rx) on the first falling edge of the data line, after the trigger
    reg [1:0] amds_data_ff1, amds_data_ff2, amds_data_ff3;
    wire amds_data0_fe, amds_data1_fe;

    // Need to double-flop the data lines for meta-stability! ff3 is used for edge detection
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN) begin
            // Good idea to reset to zero to guarantee the first falling edge is real
            amds_data_ff1 <= 2'b00;
            amds_data_ff2 <= 2'b00;
            amds_data_ff3 <= 2'b00;
        end
        else begin
            amds_data_ff1 <= amds_data;
            amds_data_ff2 <= amds_data_ff1;
            amds_data_ff3 <= amds_data_ff2;
        end
    end

    assign amds_data0_fe = amds_data_ff3[0] & ~amds_data_ff2[0];
    assign amds_data1_fe = amds_data_ff3[1] & ~amds_data_ff2[1];


    reg waiting_for_first_fe0, waiting_for_first_fe1, start_rx0, start_rx1;

    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            waiting_for_first_fe0 <= 1'b0;
        else if (~enable)
            waiting_for_first_fe0 <= 1'b0;
        else if (done & enabled_trigger)
            waiting_for_first_fe0 <= 1'b1;
        else if ((waiting_for_first_fe0 & amds_data0_fe) | first_packet_timeout0)
            waiting_for_first_fe0 <= 1'b0;
    end

    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            waiting_for_first_fe1 <= 1'b0;
        else if (~enable)
            waiting_for_first_fe1 <= 1'b0;
        else if (done & enabled_trigger)
            waiting_for_first_fe1 <= 1'b1;
        else if ((waiting_for_first_fe1 & amds_data1_fe) | first_packet_timeout1)
            waiting_for_first_fe1 <= 1'b0;
    end

    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            start_rx0 <= 1'b0;
        else if (waiting_for_first_fe0 & amds_data0_fe)
            start_rx0 <= 1'b1;
        else
            start_rx0 <= 1'b0;
    end

    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            start_rx1 <= 1'b0;
        else if (waiting_for_first_fe1 & amds_data1_fe)
            start_rx1 <= 1'b1;
        else
            start_rx1 <= 1'b0;
    end

    // In testing, we noticed that the delay from the SYNC_ADC trigger to the
    // first falling edge is not always consistent. These timers count that delay
    // and dump it into a slave register for debugging
    //
    // This timer from a "true trigger" to the first falling edge is also used
    // to determine if we should timeout this sensor interface's wait for data
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            trigger_to_fe0_timer <= 16'h0;
        else if (waiting_for_first_fe0)
            trigger_to_fe0_timer <= trigger_to_fe0_timer + 1;
        else
            trigger_to_fe0_timer <= 16'b0;
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            trigger_to_fe1_timer <= 16'h0;
        else if (waiting_for_first_fe1)
            trigger_to_fe1_timer <= trigger_to_fe1_timer + 1;
        else
            trigger_to_fe1_timer <= 16'b0;
    end
    
    adc_uart_rx iADC_UART_RX0 (
        .clk(S_AXI_ACLK),
        .rst_n(S_AXI_ARESETN),
        .start_rx(start_rx0),
        .din(amds_data_ff2[0]),
        .is_dout_valid(is_dout0_valid),
        .is_dout_enabled(is_dout0_enabled),
        .adc_uart_done(adc_uart0_done),
        .assert_done(assert_done_0),
        .adc_dout0(my_adc_data0),
        .adc_dout1(my_adc_data1),
        .adc_dout2(my_adc_data2),
        .adc_dout3(my_adc_data3),
        .adc_dout4(my_adc_data8),
        .adc_dout5(my_adc_data9),
        .adc_dout6(my_adc_data10),
        .adc_dout7(my_adc_data11),
        .adc_dout8(my_adc_data16),
        .adc_dout9(my_adc_data17),
        .adc_dout10(my_adc_data18),
        .adc_dout11(my_adc_data19),
        .counter_bytes_valid(counter_line0_bytes_valid),
        .counter_bytes_corrupt(counter_line0_bytes_corrupt),
        .counter_bytes_timed_out(counter_line0_bytes_timed_out)
    );
    
    adc_uart_rx iADC_UART_RX1 (
        .clk(S_AXI_ACLK),
        .rst_n(S_AXI_ARESETN),
        .start_rx(start_rx1),
        .din(amds_data_ff2[1]),
        .is_dout_valid(is_dout1_valid),
        .is_dout_enabled(is_dout1_enabled),
        .adc_uart_done(adc_uart1_done),
        .assert_done(assert_done_1),
        .adc_dout0(my_adc_data4),
        .adc_dout1(my_adc_data5),
        .adc_dout2(my_adc_data6),
        .adc_dout3(my_adc_data7),
        .adc_dout4(my_adc_data12),
        .adc_dout5(my_adc_data13),
        .adc_dout6(my_adc_data14),
        .adc_dout7(my_adc_data15),
        .adc_dout8(my_adc_data20),
        .adc_dout9(my_adc_data21),
        .adc_dout10(my_adc_data22),
        .adc_dout11(my_adc_data23),
        .counter_bytes_valid(counter_line1_bytes_valid),
        .counter_bytes_corrupt(counter_line1_bytes_corrupt),
        .counter_bytes_timed_out(counter_line1_bytes_timed_out)
    );
    
    // Latch in new data when we get the done signal from the adc_uart_rx module
    // ** IMPORTANT: Data is not necessarily always valid! User will have to read the 
    //               channel valid register in the C code to verify validity of each ADC's data!
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout0 <= 32'b0;
        else if (adc_uart0_done & slv_reg30[0])
            adc_dout0 <= {{16{my_adc_data0[15]}}, my_adc_data0};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout1 <= 32'b0;
        else if (adc_uart0_done & slv_reg30[1])
            adc_dout1 <= {{16{my_adc_data1[15]}}, my_adc_data1};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout2 <= 32'b0;
        else if (adc_uart0_done & slv_reg30[2])
            adc_dout2 <= {{16{my_adc_data2[15]}}, my_adc_data2};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout3 <= 32'b0;
        else if (adc_uart0_done & slv_reg30[3])
            adc_dout3 <= {{16{my_adc_data3[15]}}, my_adc_data3};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout4 <= 32'b0;
        else if (adc_uart1_done & slv_reg30[4])
            adc_dout4 <= {{16{my_adc_data4[15]}}, my_adc_data4};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout5 <= 32'b0;
        else if (adc_uart1_done & slv_reg30[5])
            adc_dout5 <= {{16{my_adc_data5[15]}}, my_adc_data5};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout6 <= 32'b0;
        else if (adc_uart1_done & slv_reg30[6])
            adc_dout6 <= {{16{my_adc_data6[15]}}, my_adc_data6};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout7 <= 32'b0;
        else if (adc_uart1_done & slv_reg30[7])
            adc_dout7 <= {{16{my_adc_data7[15]}}, my_adc_data7};
    end
    
    
    
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout8 <= 32'b0;
        else if (adc_uart0_done & slv_reg30[8])
            adc_dout8 <= {{16{my_adc_data8[15]}}, my_adc_data8};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout9 <= 32'b0;
        else if (adc_uart0_done & slv_reg30[9])
            adc_dout9 <= {{16{my_adc_data9[15]}}, my_adc_data9};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout10 <= 32'b0;
        else if (adc_uart0_done & slv_reg30[10])
            adc_dout10 <= {{16{my_adc_data10[15]}}, my_adc_data10};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout11 <= 32'b0;
        else if (adc_uart0_done & slv_reg30[11])
            adc_dout11 <= {{16{my_adc_data11[15]}}, my_adc_data11};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout12 <= 32'b0;
        else if (adc_uart1_done & slv_reg30[12])
            adc_dout12 <= {{16{my_adc_data12[15]}}, my_adc_data12};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout13 <= 32'b0;
        else if (adc_uart1_done & slv_reg30[13])
            adc_dout13 <= {{16{my_adc_data13[15]}}, my_adc_data13};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout14 <= 32'b0;
        else if (adc_uart1_done & slv_reg30[14])
            adc_dout14 <= {{16{my_adc_data14[15]}}, my_adc_data14};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout15 <= 32'b0;
        else if (adc_uart1_done & slv_reg30[15])
            adc_dout15 <= {{16{my_adc_data15[15]}}, my_adc_data15};
    end
    
    
    
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout16 <= 32'b0;
        else if (adc_uart0_done & slv_reg30[16])
            adc_dout16 <= {{16{my_adc_data16[15]}}, my_adc_data16};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout17 <= 32'b0;
        else if (adc_uart0_done & slv_reg30[17])
            adc_dout17 <= {{16{my_adc_data17[15]}}, my_adc_data17};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout18 <= 32'b0;
        else if (adc_uart0_done & slv_reg30[18])
            adc_dout18 <= {{16{my_adc_data18[15]}}, my_adc_data18};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout19 <= 32'b0;
        else if (adc_uart0_done & slv_reg30[19])
            adc_dout19 <= {{16{my_adc_data19[15]}}, my_adc_data19};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout20 <= 32'b0;
        else if (adc_uart1_done & slv_reg30[20])
            adc_dout20 <= {{16{my_adc_data20[15]}}, my_adc_data20};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout21 <= 32'b0;
        else if (adc_uart1_done & slv_reg30[21])
            adc_dout21 <= {{16{my_adc_data21[15]}}, my_adc_data21};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout22 <= 32'b0;
        else if (adc_uart1_done & slv_reg30[22])
            adc_dout22 <= {{16{my_adc_data22[15]}}, my_adc_data22};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout23 <= 32'b0;
        else if (adc_uart1_done & slv_reg30[23])
            adc_dout23 <= {{16{my_adc_data23[15]}}, my_adc_data23};
    end

    // =============================================
    // Channel Valid Register
    // * Allows C code to check if the data is valid
    // =============================================
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            ch_valid_reg <= 32'b0;
        else if (done)
            ch_valid_reg <= {8'b0, is_dout1_valid, is_dout0_valid};
    end

    // Receiving is done when both sub-receivers are done, but
    //   both done FFs must be cleared as soon as we recieve a trigger
    // We cannot reuse the is_dout_valid signals, because if corrupt data was received, the receiver is "done",
    //   but the data is not valid. And done MUST go high even in the case of corrupt/timed-out data, because the 
    //   timing manager will freeze, and not send out another trigger or call the ISR until we tell it we're done
    reg done_0;
    always @(posedge S_AXI_ACLK, negedge S_AXI_ARESETN) begin
        if (~S_AXI_ARESETN)
            done_0 <= 1'b1;
        else if (enabled_trigger)
            done_0 <= 1'b0;
        else if (assert_done_0 | first_packet_timeout0)
            done_0 <= 1'b1;
    end
    
    reg done_1;
    always @(posedge S_AXI_ACLK, negedge S_AXI_ARESETN) begin
        if (~S_AXI_ARESETN)
            done_1 <= 1'b1;
        else if (enabled_trigger)
            done_1 <= 1'b0;
        else if (assert_done_1 | first_packet_timeout1)
            done_1 <= 1'b1;
    end
    
    assign done = done_0 & done_1;

    // User logic ends

    endmodule
