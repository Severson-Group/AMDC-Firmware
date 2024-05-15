
`timescale 1 ns / 1 ps

    module amdc_amds_v1_0_S00_AXI #
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
    reg [31:0] ch_valid_reg;
    reg [31:0] valid_reg;
    reg [31:0] corrupt_reg;
    reg [31:0] timeout_reg;

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

    // Implement memory mapped register select and read logic generation
    // Slave register read enable is asserted when valid address is available
    // and the slave is ready to accept the read address.
    assign slv_reg_rden = axi_arready & S_AXI_ARVALID & ~axi_rvalid;
    always @(*)
    begin
          // Address decoding for reading registers
          case ( axi_araddr[ADDR_LSB+OPT_MEM_ADDR_BITS:ADDR_LSB] )
            4'h0   : reg_data_out <= adc_dout0;
            4'h1   : reg_data_out <= adc_dout1;
            4'h2   : reg_data_out <= adc_dout2;
            4'h3   : reg_data_out <= adc_dout3;
            4'h4   : reg_data_out <= adc_dout4;
            4'h5   : reg_data_out <= adc_dout5;
            4'h6   : reg_data_out <= adc_dout6;
            4'h7   : reg_data_out <= adc_dout7;
            4'h8   : reg_data_out <= slv_reg8;
            4'h9   : reg_data_out <= ch_valid_reg;
            4'hA   : reg_data_out <= valid_reg;
            4'hB   : reg_data_out <= corrupt_reg;
            4'hC   : reg_data_out <= timeout_reg;
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

    // ======
    // Construct SYNC_ADC signal based on trigger and user enable bit
    // ======
    reg sync_adc_flop;
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            sync_adc_flop <= 1'b0;
        else if (trigger)
            sync_adc_flop <= ~sync_adc_flop;
    end
    
    assign sync_adc = (enable) ? sync_adc_flop : 1'b0;
    
    // This module listens to the two data inputs from the AMDS.
    // When a data line first goes low after a trigger event, this module
    // knows to expect a new data packet transmission. Therefore, it will
    // start a state machine internally to read each UART word.
    wire [3:0] is_dout0_valid, is_dout1_valid;
    wire adc_uart0_done, adc_uart1_done;
   
    wire [15:0] my_adc_data0;
    wire [15:0] my_adc_data1;
    wire [15:0] my_adc_data2;
    wire [15:0] my_adc_data3;
    wire [15:0] my_adc_data4;
    wire [15:0] my_adc_data5;
    wire [15:0] my_adc_data6;
    wire [15:0] my_adc_data7;
    
    // Debugging counters
    wire [15:0] counter_data0_valid;
    wire [15:0] counter_data0_corrupt;
    wire [15:0] counter_data0_timeout;
    wire [15:0] counter_data1_valid;
    wire [15:0] counter_data1_corrupt;
    wire [15:0] counter_data1_timeout;

    // Start receiving data (assert start_rx) on the first falling edge of the data line, after the trigger
    reg [1:0] amds_data_ff;
    wire amds_data0_fe, amds_data1_fe;

    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            // Good idea to reset to zero to guarantee the first falling edge is real
            amds_data_ff <= 2'b00;
        else
            amds_data_ff <= amds_data;
    end

    assign amds_data0_fe = amds_data_ff[0] & ~amds_data[0];
    assign amds_data1_fe = amds_data_ff[1] & ~amds_data[1];


    reg waiting_for_first_fe0, waiting_for_first_fe1, start_rx0, start_rx1;

    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            waiting_for_first_fe0 <= 1'b0;
        else if (trigger & done)
            waiting_for_first_fe0 <= 1'b1;
        else if (start_rx0)
            waiting_for_first_fe0 <= 1'b0;
    end

    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            waiting_for_first_fe1 <= 1'b0;
        else if (trigger & done)
            waiting_for_first_fe1 <= 1'b1;
        else if (start_rx1)
            waiting_for_first_fe1 <= 1'b0;
    end

    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            start_rx0 <= 1'b0;
        else if (waiting_for_first_fe0 & amds_data0_fe)
            start_rx0 <= 1'b1;
    end

    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            start_rx1 <= 1'b0;
        else if (waiting_for_first_fe1 & amds_data1_fe)
            start_rx1 <= 1'b1;
    end
    
    adc_uart_rx iADC_UART_RX0 (
        .clk(S_AXI_ACLK),
        .rst_n(S_AXI_ARESETN),
        .start_rx(start_rx0),
        .din(amds_data[0]),
        .is_dout_valid(is_dout0_valid),
        .adc_uart_done(adc_uart0_done),
        .adc_dout0(my_adc_data0),
        .adc_dout1(my_adc_data1),
        .adc_dout2(my_adc_data2),
        .adc_dout3(my_adc_data3),
        .counter_data_valid(counter_data0_valid),
        .counter_data_corrupt(counter_data0_corrupt),
        .counter_data_timeout(counter_data0_timeout)
    );
    
    adc_uart_rx iADC_UART_RX1 (
        .clk(S_AXI_ACLK),
        .rst_n(S_AXI_ARESETN),
        .start_rx(start_rx1),
        .din(amds_data[1]),
        .is_dout_valid(is_dout1_valid),
        .adc_uart_done(adc_uart1_done),
        .adc_dout0(my_adc_data4),
        .adc_dout1(my_adc_data5),
        .adc_dout2(my_adc_data6),
        .adc_dout3(my_adc_data7),
        .counter_data_valid(counter_data1_valid),
        .counter_data_corrupt(counter_data1_corrupt),
        .counter_data_timeout(counter_data1_timeout)
    );
    
    // Latch in new data when we get the done signal from the adc_uart_rx module
    // ** IMPORTANT: Data is not necessarily always valid! User will have to read the 
    //               channel valid register in the C code to verify validity of each ADC's data!
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout0 <= 32'b0;
        else if (adc_uart0_done)
            adc_dout0 <= {{16{my_adc_data0[15]}}, my_adc_data0};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout1 <= 32'b0;
        else if (adc_uart0_done)
            adc_dout1 <= {{16{my_adc_data1[15]}}, my_adc_data1};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout2 <= 32'b0;
        else if (adc_uart0_done)
            adc_dout2 <= {{16{my_adc_data2[15]}}, my_adc_data2};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout3 <= 32'b0;
        else if (adc_uart0_done)
            adc_dout3 <= {{16{my_adc_data3[15]}}, my_adc_data3};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout4 <= 32'b0;
        else if (adc_uart1_done)
            adc_dout4 <= {{16{my_adc_data4[15]}}, my_adc_data4};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout5 <= 32'b0;
        else if (adc_uart1_done)
            adc_dout5 <= {{16{my_adc_data5[15]}}, my_adc_data5};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout6 <= 32'b0;
        else if (adc_uart1_done)
            adc_dout6 <= {{16{my_adc_data6[15]}}, my_adc_data6};
    end
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            adc_dout7 <= 32'b0;
        else if (adc_uart1_done)
            adc_dout7 <= {{16{my_adc_data7[15]}}, my_adc_data7};
    end

    // =============================================
    // Channel Valid Register
    // * Allows C code to check if the data is valid
    // =============================================
    always @(posedge S_AXI_ACLK) begin
        if (~S_AXI_ARESETN)
            ch_valid_reg <= 32'b0;
        else if (done)
            ch_valid_reg <= {24'b0, is_dout1_valid, is_dout0_valid};
    end

    // ===============
    // Debug Registers
    // ===============
    always @(posedge S_AXI_ACLK) begin
        valid_reg[31:0] <= {counter_data1_valid, counter_data0_valid};
        corrupt_reg[31:0] <= {counter_data1_corrupt, counter_data0_corrupt};
        timeout_reg[31:0] <= {counter_data1_timeout, counter_data0_timeout};
    end

    // Receiving is done when both sub-receivers are done
    // We cannot reuse the is_dout_valid signals, because if corrupt data was received, the receiver is "done",
    //   but the data is not valid. And done MUST go high even in the case of corrupt/timed-out data, because the 
    //   timing manager will freeze, and not send out another trigger or call the ISR until we tell it we're done
    assign done = adc_uart0_done & adc_uart1_done;

    // User logic ends

    endmodule
