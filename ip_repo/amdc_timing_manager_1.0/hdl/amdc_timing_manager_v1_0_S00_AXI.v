
`timescale 1 ns / 1 ps

    module amdc_timing_manager_v1_0_S00_AXI #
    (
        // Users to add parameters here

        // User parameters ends
        // Do not modify the parameters beyond this line

        // Width of S_AXI data bus
        parameter integer C_S_AXI_DATA_WIDTH    = 32,
        // Width of S_AXI address bus
        parameter integer C_S_AXI_ADDR_WIDTH    = 6
    )
    (
        // Users to add ports here
        input  wire pwm_carrier_high,
        input  wire pwm_carrier_low,
        input  wire adc_done,
        input  wire encoder_done,
        input  wire amds_0_done,
        input  wire amds_1_done,
        input  wire amds_2_done,
        input  wire amds_3_done,
        input  wire eddy_0_done,
        input  wire eddy_1_done,
        input  wire eddy_2_done,
        input  wire eddy_3_done,
        output wire sched_isr,
        output wire en_amds_0,
        output wire en_amds_1,
        output wire en_amds_2,
        output wire en_amds_3,
        output wire en_eddy_0,
        output wire en_eddy_1,
        output wire en_eddy_2,
        output wire en_eddy_3,
        output wire en_adc,
        output wire en_encoder,
        output wire trigger,
        output wire [2:0] debug,

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
    localparam integer OPT_MEM_ADDR_BITS = 3;
    //----------------------------------------------
    //-- Signals for user logic register space example
    //------------------------------------------------
    //-- Number of Slave Registers 16
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
          slv_reg0 <= 32'h00000001;
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
        end 
      else begin
        if (slv_reg_wren)
          begin
            case ( axi_awaddr[ADDR_LSB+OPT_MEM_ADDR_BITS:ADDR_LSB] )
              4'h0:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 0
                    slv_reg0[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              4'h1:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 1
                    slv_reg1[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              4'h2:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 2
                    slv_reg2[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              4'h3:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 3
                    slv_reg3[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              4'h4:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 4
                    slv_reg4[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              4'h5:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 5
                    slv_reg5[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              4'h6:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 6
                    slv_reg6[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              4'h7:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 7
                    slv_reg7[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              4'h8:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 8
                    slv_reg8[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              4'h9:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 9
                    slv_reg9[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              4'hA:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 10
                    slv_reg10[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              4'hB:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 11
                    slv_reg11[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              4'hC:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 12
                    slv_reg12[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              4'hD:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 13
                    slv_reg13[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              4'hE:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 14
                    slv_reg14[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                  end  
              4'hF:
                for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                  if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                    // Respective byte enables are asserted as per write strobes 
                    // Slave register 15
                    slv_reg15[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
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

    // Internal signals
    reg [27:0] trigger_count;
    wire [31:0] sensor_done_status;
    wire [31:0] sched_tick_time;
    wire [31:0] adc_enc_time_reg;
    wire [31:0] amds_01_time_reg;
    wire [31:0] amds_23_time_reg;
    wire [31:0] eddy_01_time_reg;
    wire [31:0] eddy_23_time_reg;

    // Implement memory mapped register select and read logic generation
    // Slave register read enable is asserted when valid address is available
    // and the slave is ready to accept the read address.
    assign slv_reg_rden = axi_arready & S_AXI_ARVALID & ~axi_rvalid;
    always @(*)
    begin
          // Address decoding for reading registers
          case ( axi_araddr[ADDR_LSB+OPT_MEM_ADDR_BITS:ADDR_LSB] )
            4'h0   : reg_data_out <= {trigger_count, slv_reg0[3:0]}; // Trigger configuration and debug count
            4'h1   : reg_data_out <= slv_reg1; // Sensor Enable Bits
            4'h2   : reg_data_out <= sensor_done_status; // Sensor Done Status
            4'h3   : reg_data_out <= slv_reg3; // User Ratio
            4'h4   : reg_data_out <= slv_reg4; // PWM Sync
            4'h5   : reg_data_out <= {sched_isr,slv_reg5[30:0]};  // ISR - MSb is sched_isr from TM for debug
            4'h6   : reg_data_out <= sched_tick_time; // Trigger timer
            4'h7   : reg_data_out <= adc_enc_time_reg; // ADC & Encoder Times
            4'h8   : reg_data_out <= amds_01_time_reg; // AMDS Times
            4'h9   : reg_data_out <= amds_23_time_reg; // AMDS Times
            4'hA   : reg_data_out <= eddy_01_time_reg; // Eddy Times
            4'hB   : reg_data_out <= eddy_23_time_reg; // Eddy Times
            4'hC   : reg_data_out <= slv_reg12;
            4'hD   : reg_data_out <= slv_reg13;
            4'hE   : reg_data_out <= slv_reg14;
            4'hF   : reg_data_out <= slv_reg15;
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
    
    wire [15:0] adc_time, encoder_time;
    wire [15:0] amds_0_time, amds_1_time, amds_2_time, amds_3_time;
    wire [15:0] eddy_0_time, eddy_1_time, eddy_2_time, eddy_3_time;
    assign adc_enc_time_reg = {encoder_time, adc_time};
    assign amds_01_time_reg = {amds_1_time, amds_0_time};
    assign amds_23_time_reg = {amds_3_time, amds_2_time};
    assign eddy_01_time_reg = {eddy_1_time, eddy_0_time};
    assign eddy_23_time_reg = {eddy_3_time, eddy_2_time};

    wire do_auto_triggering;
    wire send_manual_trigger;
    reg manual_trigger_ff;
    wire sched_source_mode;
    wire [15:0] user_ratio;
    wire [15:0] en_bits;
    wire reset_sched_isr;
    reg reset_isr_ff;

    // Trigger configuration
    // * Auto triggering is the default, triggers will be sent whenever <user_ratio> PWM events
    //   have occurred and all the enabled sensors are done sampling
    assign do_auto_triggering = slv_reg0[0]; // reset to 1
    // * Manual triggering can be used for debugging sensor interfaces. Flipping the manual trigger
    //   bit in the config register will request a single trigger event aligned to the next
    //   peak and/or valley of the PWM carrier.
    assign send_manual_trigger = (manual_trigger_ff ^ slv_reg0[1]) & !do_auto_triggering;

    always @(posedge S_AXI_ACLK) begin
      if ( S_AXI_ARESETN == 1'b0 )
        manual_trigger_ff <= 0;
      else
        manual_trigger_ff <= slv_reg0[1];
    end

    // Trigger Count
    //   Count the number of triggers (for use when debugging timing manager)
    //   28 bits lets us count 2^28, or 268 million triggers
    //   If we're controlling at 10 kHz, it will take 26,843 seconds, or 7.4 hours
    //   for this value to roll over
    always @(posedge S_AXI_ACLK) begin
        if (!S_AXI_ARESETN)
            trigger_count <= 28'b0;
        else if (trigger)
            trigger_count <= trigger_count + 1;
    end
    
    // Get the user ratio from slave register 2, assigning
    // the lower 16 bits
    assign user_ratio = slv_reg3[15:0];
    
    // Reset scheduler interrupt: flipping the reset ISR bit in the configuration
    // register will assert a reset signal to clear the interrupt
    assign reset_sched_isr = slv_reg5[0] ^ reset_isr_ff;
    always @(posedge S_AXI_ACLK, negedge S_AXI_ARESETN) begin
      if (!S_AXI_ARESETN)
        reset_isr_ff <= 0;
      else
        reset_isr_ff <= slv_reg5[0];
    end

    // Determines the source of the interrupt for the scheduler with two modes
    assign sched_source_mode = slv_reg5[1];

    // Get the enable bits from the user to
    // decode them in the timing manager
    assign en_bits = slv_reg1[15:0];

    // Output the sensor done statuses into a slv_reg to be accessible to the C code
    wire all_done;
    assign all_done = adc_done & encoder_done &
                      amds_0_done & amds_1_done & amds_2_done & amds_3_done & 
                      eddy_0_done & eddy_1_done & eddy_2_done & eddy_3_done;
    assign sensor_done_status = {all_done, 21'b0,
                                 eddy_3_done, eddy_2_done, eddy_1_done, eddy_0_done,
                                 amds_3_done, amds_2_done, amds_1_done, amds_0_done, 
                                 encoder_done, adc_done};

    // User defined method of synchronizing the pwm on high, low, or both
    wire event_qualifier;
    wire pwm_sync_high;
    wire pwm_sync_low;
    assign pwm_sync_high = slv_reg4[0];
    assign pwm_sync_low  = slv_reg4[1];
    
    //////////////////////////////////////////////////////////////////
    // Generate event qualifier signal based on PWM to start        //
    // a conversion/sequence for a sensor. This allows firmware     //
    // synchronization between all the sensors (ADC, encoder, eddy  //
    // current sensor, AMDS) and user configuration. This also      //
    // acts as the event qualifier for the sched ISR generation.    //
    // It can only be triggered once that past cycle is complete.   //
    //////////////////////////////////////////////////////////////////
    assign event_qualifier = (pwm_sync_high & pwm_carrier_high) | (pwm_sync_low & pwm_carrier_low); 
    
    timing_manager iTime(
    .clk(S_AXI_ACLK),
    .rst_n(S_AXI_ARESETN),
    .do_auto_triggering(do_auto_triggering),
    .send_manual_trigger(send_manual_trigger),
    .event_qualifier(event_qualifier),
    .user_ratio(user_ratio),
    .sched_isr(sched_isr),
    .en_bits(en_bits),
    .adc_done(adc_done),
    .encoder_done(encoder_done),
    .amds_0_done(amds_0_done),
    .amds_1_done(amds_1_done),
    .amds_2_done(amds_2_done),
    .amds_3_done(amds_3_done),
    .eddy_0_done(eddy_0_done),
    .eddy_1_done(eddy_1_done),
    .eddy_2_done(eddy_2_done),
    .eddy_3_done(eddy_3_done),
    .en_adc(en_adc),
    .en_encoder(en_encoder),
    .en_amds_0(en_amds_0),
    .en_amds_1(en_amds_1),
    .en_amds_2(en_amds_2),
    .en_amds_3(en_amds_3),
    .en_eddy_0(en_eddy_0),
    .en_eddy_1(en_eddy_1),
    .en_eddy_2(en_eddy_2),
    .en_eddy_3(en_eddy_3),
    .adc_time(adc_time),
    .encoder_time(encoder_time),
    .amds_0_time(amds_0_time),
    .amds_1_time(amds_1_time),
    .amds_2_time(amds_2_time),
    .amds_3_time(amds_3_time),
    .eddy_0_time(eddy_0_time),
    .eddy_1_time(eddy_1_time),
    .eddy_2_time(eddy_2_time),
    .eddy_3_time(eddy_3_time),
    .trigger(trigger),
    .reset_sched_isr(reset_sched_isr),
    .sched_source_mode(sched_source_mode),
    .sched_tick_time(sched_tick_time),
    .debug(debug)
    );

    // User logic ends

    endmodule
