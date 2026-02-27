// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2025.1 (win64) Build 6140274 Thu May 22 00:12:29 MDT 2025
// Date        : Tue Jul  8 14:21:11 2025
// Host        : DESKTOP-JP2R5GF running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode synth_stub -rename_top sys_top_t4nfc_hlper_1_0 -prefix
//               sys_top_t4nfc_hlper_1_0_ sys_top_t4nfc_hlper_0_0_stub.v
// Design      : sys_top_t4nfc_hlper_0_0
// Purpose     : Stub declaration of top-level module interface
// Device      : xczu17eg-ffvc1760-2-e
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* CHECK_LICENSE_TYPE = "sys_top_t4nfc_hlper_0_0,FMCTop,{}" *) (* DowngradeIPIdentifiedWarnings = "yes" *) (* IP_DEFINITION_SOURCE = "package_project" *) 
(* X_CORE_INFO = "FMCTop,Vivado 2025.1" *) 
module sys_top_t4nfc_hlper_1_0(iClock, iReset, C_AWVALID, C_AWREADY, C_AWADDR, 
  C_AWPROT, C_WVALID, C_WREADY, C_WDATA, C_WSTRB, C_BVALID, C_BREADY, C_BRESP, C_ARVALID, C_ARREADY, 
  C_ARADDR, C_ARPROT, C_RVALID, C_RREADY, C_RDATA, C_RRESP, D_AWADDR, D_AWLEN, D_AWSIZE, D_AWBURST, 
  D_AWCACHE, D_AWPROT, D_AWVALID, D_AWREADY, D_WDATA, D_WSTRB, D_WLAST, D_WVALID, D_WREADY, D_BRESP, 
  D_BVALID, D_BREADY, D_ARADDR, D_ARLEN, D_ARSIZE, D_ARBURST, D_ARCACHE, D_ARPROT, D_ARVALID, 
  D_ARREADY, D_RDATA, D_RRESP, D_RLAST, D_RVALID, D_RREADY, oOpcode, oTargetID, oSourceID, oAddress, 
  oLength, oCMDValid, iCMDReady, oWriteData, oWriteLast, oWriteValid, iWriteReady, iReadData, 
  iReadLast, iReadValid, oReadReady, iReadyBusy, oROMClock, oROMReset, oROMAddr, oROMRW, 
  oROMEnable, oROMWData, iROMRData, oToECCWOpcode, oToECCWTargetID, oToECCWSourceID, 
  oToECCWAddress, oToECCWLength, oToECCWCmdValid, iToECCWCmdReady, oToECCWData, oToECCWValid, 
  oToECCWLast, iToECCWReady, oToECCROpcode, oToECCRTargetID, oToECCRSourceID, oToECCRAddress, 
  oToECCRLength, oToECCRCmdValid, iToECCRCmdReady, iToECCRData, iToECCRValid, iToECCRLast, 
  oToECCRReady, ifromECCWOpcode, ifromECCWTargetID, ifromECCWSourceID, ifromECCWAddress, 
  ifromECCWLength, ifromECCWCmdValid, ofromECCWCmdReady, ifromECCWData, ifromECCWValid, 
  ifromECCWLast, ofromECCWReady, ifromECCROpcode, ifromECCRTargetID, ifromECCRSourceID, 
  ifromECCRAddress, ifromECCRLength, ifromECCRCmdValid, ofromECCRCmdReady, ofromECCRData, 
  ofromECCRValid, ofromECCRLast, ifromECCRReady, O_DEBUG)
/* synthesis syn_black_box black_box_pad_pin="iReset,C_AWVALID,C_AWREADY,C_AWADDR[31:0],C_AWPROT[2:0],C_WVALID,C_WREADY,C_WDATA[31:0],C_WSTRB[3:0],C_BVALID,C_BREADY,C_BRESP[1:0],C_ARVALID,C_ARREADY,C_ARADDR[31:0],C_ARPROT[2:0],C_RVALID,C_RREADY,C_RDATA[31:0],C_RRESP[1:0],D_AWADDR[31:0],D_AWLEN[7:0],D_AWSIZE[2:0],D_AWBURST[1:0],D_AWCACHE[3:0],D_AWPROT[2:0],D_AWVALID,D_AWREADY,D_WDATA[31:0],D_WSTRB[3:0],D_WLAST,D_WVALID,D_WREADY,D_BRESP[1:0],D_BVALID,D_BREADY,D_ARADDR[31:0],D_ARLEN[7:0],D_ARSIZE[2:0],D_ARBURST[1:0],D_ARCACHE[3:0],D_ARPROT[2:0],D_ARVALID,D_ARREADY,D_RDATA[31:0],D_RRESP[1:0],D_RLAST,D_RVALID,D_RREADY,oOpcode[5:0],oTargetID[4:0],oSourceID[4:0],oAddress[31:0],oLength[15:0],oCMDValid,iCMDReady,oWriteData[31:0],oWriteLast,oWriteValid,iWriteReady,iReadData[31:0],iReadLast,iReadValid,oReadReady,iReadyBusy[7:0],oROMReset,oROMAddr[31:0],oROMRW,oROMEnable,oROMWData[31:0],iROMRData[31:0],oToECCWOpcode[5:0],oToECCWTargetID[4:0],oToECCWSourceID[4:0],oToECCWAddress[31:0],oToECCWLength[15:0],oToECCWCmdValid,iToECCWCmdReady,oToECCWData[31:0],oToECCWValid,oToECCWLast,iToECCWReady,oToECCROpcode[5:0],oToECCRTargetID[4:0],oToECCRSourceID[4:0],oToECCRAddress[31:0],oToECCRLength[15:0],oToECCRCmdValid,iToECCRCmdReady,iToECCRData[31:0],iToECCRValid,iToECCRLast,oToECCRReady,ifromECCWOpcode[5:0],ifromECCWTargetID[4:0],ifromECCWSourceID[4:0],ifromECCWAddress[31:0],ifromECCWLength[15:0],ifromECCWCmdValid,ofromECCWCmdReady,ifromECCWData[31:0],ifromECCWValid,ifromECCWLast,ofromECCWReady,ifromECCROpcode[5:0],ifromECCRTargetID[4:0],ifromECCRSourceID[4:0],ifromECCRAddress[31:0],ifromECCRLength[15:0],ifromECCRCmdValid,ofromECCRCmdReady,ofromECCRData[31:0],ofromECCRValid,ofromECCRLast,ifromECCRReady,O_DEBUG[31:0]" */
/* synthesis syn_force_seq_prim="iClock" */
/* synthesis syn_force_seq_prim="oROMClock" */;
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 iClock CLK" *) (* X_INTERFACE_MODE = "slave" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME iClock, ASSOCIATED_BUSIF nfch_cmd_if:nfch_data_if:v2nfc_if:to_ecc_if:from_ecc_if, ASSOCIATED_RESET iReset, FREQ_HZ 66666000, PHASE 0.0, CLK_DOMAIN /pll_bank10_clk_out1, INSERT_VIP 0" *) input iClock /* synthesis syn_isclock = 1 */;
  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 iReset RST" *) (* X_INTERFACE_MODE = "slave" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME iReset, POLARITY ACTIVE_HIGH, INSERT_VIP 0" *) input iReset;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_cmd_if AWVALID" *) (* X_INTERFACE_MODE = "slave" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME nfch_cmd_if, DATA_WIDTH 32, PROTOCOL AXI4LITE, FREQ_HZ 66666000, ID_WIDTH 0, ADDR_WIDTH 32, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, READ_WRITE_MODE READ_WRITE, HAS_BURST 0, HAS_LOCK 0, HAS_PROT 1, HAS_CACHE 0, HAS_QOS 0, HAS_REGION 0, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, SUPPORTS_NARROW_BURST 0, NUM_READ_OUTSTANDING 1, NUM_WRITE_OUTSTANDING 1, MAX_BURST_LENGTH 1, PHASE 0.0, CLK_DOMAIN /pll_bank10_clk_out1, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0" *) input C_AWVALID;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_cmd_if AWREADY" *) output C_AWREADY;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_cmd_if AWADDR" *) input [31:0]C_AWADDR;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_cmd_if AWPROT" *) input [2:0]C_AWPROT;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_cmd_if WVALID" *) input C_WVALID;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_cmd_if WREADY" *) output C_WREADY;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_cmd_if WDATA" *) input [31:0]C_WDATA;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_cmd_if WSTRB" *) input [3:0]C_WSTRB;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_cmd_if BVALID" *) output C_BVALID;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_cmd_if BREADY" *) input C_BREADY;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_cmd_if BRESP" *) output [1:0]C_BRESP;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_cmd_if ARVALID" *) input C_ARVALID;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_cmd_if ARREADY" *) output C_ARREADY;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_cmd_if ARADDR" *) input [31:0]C_ARADDR;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_cmd_if ARPROT" *) input [2:0]C_ARPROT;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_cmd_if RVALID" *) output C_RVALID;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_cmd_if RREADY" *) input C_RREADY;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_cmd_if RDATA" *) output [31:0]C_RDATA;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_cmd_if RRESP" *) output [1:0]C_RRESP;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if AWADDR" *) (* X_INTERFACE_MODE = "master" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME nfch_data_if, DATA_WIDTH 32, PROTOCOL AXI4, FREQ_HZ 66666000, ID_WIDTH 0, ADDR_WIDTH 32, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, READ_WRITE_MODE READ_WRITE, HAS_BURST 1, HAS_LOCK 0, HAS_PROT 1, HAS_CACHE 1, HAS_QOS 0, HAS_REGION 0, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, SUPPORTS_NARROW_BURST 1, NUM_READ_OUTSTANDING 2, NUM_WRITE_OUTSTANDING 2, MAX_BURST_LENGTH 256, PHASE 0.0, CLK_DOMAIN /pll_bank10_clk_out1, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0" *) output [31:0]D_AWADDR;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if AWLEN" *) output [7:0]D_AWLEN;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if AWSIZE" *) output [2:0]D_AWSIZE;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if AWBURST" *) output [1:0]D_AWBURST;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if AWCACHE" *) output [3:0]D_AWCACHE;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if AWPROT" *) output [2:0]D_AWPROT;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if AWVALID" *) output D_AWVALID;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if AWREADY" *) input D_AWREADY;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if WDATA" *) output [31:0]D_WDATA;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if WSTRB" *) output [3:0]D_WSTRB;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if WLAST" *) output D_WLAST;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if WVALID" *) output D_WVALID;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if WREADY" *) input D_WREADY;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if BRESP" *) input [1:0]D_BRESP;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if BVALID" *) input D_BVALID;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if BREADY" *) output D_BREADY;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if ARADDR" *) output [31:0]D_ARADDR;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if ARLEN" *) output [7:0]D_ARLEN;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if ARSIZE" *) output [2:0]D_ARSIZE;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if ARBURST" *) output [1:0]D_ARBURST;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if ARCACHE" *) output [3:0]D_ARCACHE;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if ARPROT" *) output [2:0]D_ARPROT;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if ARVALID" *) output D_ARVALID;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if ARREADY" *) input D_ARREADY;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if RDATA" *) input [31:0]D_RDATA;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if RRESP" *) input [1:0]D_RRESP;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if RLAST" *) input D_RLAST;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if RVALID" *) input D_RVALID;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 nfch_data_if RREADY" *) output D_RREADY;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if Opcode" *) (* X_INTERFACE_MODE = "master" *) output [5:0]oOpcode;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if TargetID" *) output [4:0]oTargetID;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if SourceID" *) output [4:0]oSourceID;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if Address" *) output [31:0]oAddress;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if Length" *) output [15:0]oLength;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if CMDValid" *) output oCMDValid;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if CMDReady" *) input iCMDReady;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if WriteData" *) output [31:0]oWriteData;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if WriteLast" *) output oWriteLast;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if WriteValid" *) output oWriteValid;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if WriteReady" *) input iWriteReady;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if ReadData" *) input [31:0]iReadData;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if ReadLast" *) input iReadLast;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if ReadValid" *) input iReadValid;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if ReadReady" *) output oReadReady;
  input [7:0]iReadyBusy;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bram:1.0 ucode_if CLK" *) (* X_INTERFACE_MODE = "master" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME ucode_if, MEM_SIZE 8192, MEM_WIDTH 32, MEM_ECC NONE, MASTER_TYPE OTHER, READ_LATENCY 1" *) output oROMClock /* synthesis syn_isclock = 1 */;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bram:1.0 ucode_if RST" *) output oROMReset;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bram:1.0 ucode_if ADDR" *) output [31:0]oROMAddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bram:1.0 ucode_if WE" *) output oROMRW;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bram:1.0 ucode_if EN" *) output oROMEnable;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bram:1.0 ucode_if DIN" *) output [31:0]oROMWData;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bram:1.0 ucode_if DOUT" *) input [31:0]iROMRData;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if WOpcode" *) (* X_INTERFACE_MODE = "master" *) output [5:0]oToECCWOpcode;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if WTargetID" *) output [4:0]oToECCWTargetID;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if WSourceID" *) output [4:0]oToECCWSourceID;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if WAddress" *) output [31:0]oToECCWAddress;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if WLength" *) output [15:0]oToECCWLength;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if WCmdValid" *) output oToECCWCmdValid;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if WCmdReady" *) input iToECCWCmdReady;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if WData" *) output [31:0]oToECCWData;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if WValid" *) output oToECCWValid;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if WLast" *) output oToECCWLast;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if WReady" *) input iToECCWReady;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if ROpcode" *) output [5:0]oToECCROpcode;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if RTargetID" *) output [4:0]oToECCRTargetID;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if RSourceID" *) output [4:0]oToECCRSourceID;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if RAddress" *) output [31:0]oToECCRAddress;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if RLength" *) output [15:0]oToECCRLength;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if RCmdValid" *) output oToECCRCmdValid;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if RCmdReady" *) input iToECCRCmdReady;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if RData" *) input [31:0]iToECCRData;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if RValid" *) input iToECCRValid;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if RLast" *) input iToECCRLast;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if RReady" *) output oToECCRReady;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if WOpcode" *) (* X_INTERFACE_MODE = "slave" *) input [5:0]ifromECCWOpcode;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if WTargetID" *) input [4:0]ifromECCWTargetID;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if WSourceID" *) input [4:0]ifromECCWSourceID;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if WAddress" *) input [31:0]ifromECCWAddress;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if WLength" *) input [15:0]ifromECCWLength;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if WCmdValid" *) input ifromECCWCmdValid;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if WCmdReady" *) output ofromECCWCmdReady;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if WData" *) input [31:0]ifromECCWData;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if WValid" *) input ifromECCWValid;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if WLast" *) input ifromECCWLast;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if WReady" *) output ofromECCWReady;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if ROpcode" *) input [5:0]ifromECCROpcode;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if RTargetID" *) input [4:0]ifromECCRTargetID;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if RSourceID" *) input [4:0]ifromECCRSourceID;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if RAddress" *) input [31:0]ifromECCRAddress;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if RLength" *) input [15:0]ifromECCRLength;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if RCmdValid" *) input ifromECCRCmdValid;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if RCmdReady" *) output ofromECCRCmdReady;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if RData" *) output [31:0]ofromECCRData;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if RValid" *) output ofromECCRValid;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if RLast" *) output ofromECCRLast;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if RReady" *) input ifromECCRReady;
  output [31:0]O_DEBUG;
endmodule
