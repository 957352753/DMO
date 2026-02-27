// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2025.1 (win64) Build 6140274 Thu May 22 00:12:29 MDT 2025
// Date        : Tue Jul  8 14:34:43 2025
// Host        : DESKTOP-JP2R5GF running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode synth_stub -rename_top sys_top_iodelay_if_0_0 -prefix
//               sys_top_iodelay_if_0_0_ sys_top_iodelay_if_0_2_stub.v
// Design      : sys_top_iodelay_if_0_2
// Purpose     : Stub declaration of top-level module interface
// Device      : xczu17eg-ffvc1760-2-e
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* CHECK_LICENSE_TYPE = "sys_top_iodelay_if_0_2,iodelay_if,{}" *) (* DowngradeIPIdentifiedWarnings = "yes" *) (* IP_DEFINITION_SOURCE = "package_project" *) 
(* X_CORE_INFO = "iodelay_if,Vivado 2025.1" *) 
module sys_top_iodelay_if_0_0(sys__clk, sys__srstn, ctrl__s_awaddr, 
  ctrl__s_awprot, ctrl__s_awvalid, ctrl__s_awready, ctrl__s_wdata, ctrl__s_wstrb, 
  ctrl__s_wvalid, ctrl__s_wready, ctrl__s_bresp, ctrl__s_bvalid, ctrl__s_bready, 
  ctrl__s_araddr, ctrl__s_arprot, ctrl__s_arvalid, ctrl__s_arready, ctrl__s_rdata, 
  ctrl__s_rresp, ctrl__s_rvalid, ctrl__s_rready, iodly_00__tap, iodly_00__tap_load, 
  iodly_01__tap, iodly_01__tap_load, iodly_02__tap, iodly_02__tap_load, iodly_03__tap, 
  iodly_03__tap_load, iodly_04__tap, iodly_04__tap_load, iodly_05__tap, iodly_05__tap_load, 
  iodly_06__tap, iodly_06__tap_load, iodly_07__tap, iodly_07__tap_load, iodly_08__tap, 
  iodly_08__tap_load, iodly_09__tap, iodly_09__tap_load, iodly_10__tap, iodly_10__tap_load, 
  iodly_11__tap, iodly_11__tap_load, iodly_12__tap, iodly_12__tap_load, iodly_13__tap, 
  iodly_13__tap_load, iodly_14__tap, iodly_14__tap_load, iodly_15__tap, iodly_15__tap_load, 
  iodly_16__tap, iodly_16__tap_load, iodly_17__tap, iodly_17__tap_load, iodly_18__tap, 
  iodly_18__tap_load, iodly_19__tap, iodly_19__tap_load, iodly_20__tap, iodly_20__tap_load, 
  iodly_21__tap, iodly_21__tap_load, iodly_22__tap, iodly_22__tap_load, iodly_23__tap, 
  iodly_23__tap_load, iodly_24__tap, iodly_24__tap_load, iodly_25__tap, iodly_25__tap_load, 
  iodly_26__tap, iodly_26__tap_load, iodly_27__tap, iodly_27__tap_load, iodly_28__tap, 
  iodly_28__tap_load, iodly_29__tap, iodly_29__tap_load, iodly_30__tap, iodly_30__tap_load, 
  iodly_31__tap, iodly_31__tap_load)
/* synthesis syn_black_box black_box_pad_pin="sys__srstn,ctrl__s_awaddr[31:0],ctrl__s_awprot[2:0],ctrl__s_awvalid,ctrl__s_awready,ctrl__s_wdata[31:0],ctrl__s_wstrb[3:0],ctrl__s_wvalid,ctrl__s_wready,ctrl__s_bresp[1:0],ctrl__s_bvalid,ctrl__s_bready,ctrl__s_araddr[31:0],ctrl__s_arprot[2:0],ctrl__s_arvalid,ctrl__s_arready,ctrl__s_rdata[31:0],ctrl__s_rresp[1:0],ctrl__s_rvalid,ctrl__s_rready,iodly_00__tap[8:0],iodly_00__tap_load[1:0],iodly_01__tap[8:0],iodly_01__tap_load[1:0],iodly_02__tap[8:0],iodly_02__tap_load[1:0],iodly_03__tap[8:0],iodly_03__tap_load[1:0],iodly_04__tap[8:0],iodly_04__tap_load[1:0],iodly_05__tap[8:0],iodly_05__tap_load[1:0],iodly_06__tap[8:0],iodly_06__tap_load[1:0],iodly_07__tap[8:0],iodly_07__tap_load[1:0],iodly_08__tap[8:0],iodly_08__tap_load[1:0],iodly_09__tap[8:0],iodly_09__tap_load[1:0],iodly_10__tap[8:0],iodly_10__tap_load[1:0],iodly_11__tap[8:0],iodly_11__tap_load[1:0],iodly_12__tap[8:0],iodly_12__tap_load[1:0],iodly_13__tap[8:0],iodly_13__tap_load[1:0],iodly_14__tap[8:0],iodly_14__tap_load[1:0],iodly_15__tap[8:0],iodly_15__tap_load[1:0],iodly_16__tap[8:0],iodly_16__tap_load[1:0],iodly_17__tap[8:0],iodly_17__tap_load[1:0],iodly_18__tap[8:0],iodly_18__tap_load[1:0],iodly_19__tap[8:0],iodly_19__tap_load[1:0],iodly_20__tap[8:0],iodly_20__tap_load[1:0],iodly_21__tap[8:0],iodly_21__tap_load[1:0],iodly_22__tap[8:0],iodly_22__tap_load[1:0],iodly_23__tap[8:0],iodly_23__tap_load[1:0],iodly_24__tap[8:0],iodly_24__tap_load[1:0],iodly_25__tap[8:0],iodly_25__tap_load[1:0],iodly_26__tap[8:0],iodly_26__tap_load[1:0],iodly_27__tap[8:0],iodly_27__tap_load[1:0],iodly_28__tap[8:0],iodly_28__tap_load[1:0],iodly_29__tap[8:0],iodly_29__tap_load[1:0],iodly_30__tap[8:0],iodly_30__tap_load[1:0],iodly_31__tap[8:0],iodly_31__tap_load[1:0]" */
/* synthesis syn_force_seq_prim="sys__clk" */;
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 sys__clk CLK" *) (* X_INTERFACE_MODE = "slave" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME sys__clk, ASSOCIATED_RESET sys__srstn, ASSOCIATED_BUSIF ctrl__s, FREQ_HZ 66666000, PHASE 0.0, CLK_DOMAIN sys_top_zynq_ultra_ps_e_0_0_pl_clk0, INSERT_VIP 0" *) input sys__clk /* synthesis syn_isclock = 1 */;
  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 sys__srstn RST" *) (* X_INTERFACE_MODE = "slave" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME sys__srstn, POLARITY ACTIVE_LOW, INSERT_VIP 0" *) input sys__srstn;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 ctrl__s AWADDR" *) (* X_INTERFACE_MODE = "slave" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME ctrl__s, DATA_WIDTH 32, PROTOCOL AXI4LITE, FREQ_HZ 66666000, ID_WIDTH 0, ADDR_WIDTH 32, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, READ_WRITE_MODE READ_WRITE, HAS_BURST 0, HAS_LOCK 0, HAS_PROT 1, HAS_CACHE 0, HAS_QOS 0, HAS_REGION 0, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, SUPPORTS_NARROW_BURST 0, NUM_READ_OUTSTANDING 1, NUM_WRITE_OUTSTANDING 1, MAX_BURST_LENGTH 1, PHASE 0.0, CLK_DOMAIN sys_top_zynq_ultra_ps_e_0_0_pl_clk0, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0" *) input [31:0]ctrl__s_awaddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 ctrl__s AWPROT" *) input [2:0]ctrl__s_awprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 ctrl__s AWVALID" *) input ctrl__s_awvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 ctrl__s AWREADY" *) output ctrl__s_awready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 ctrl__s WDATA" *) input [31:0]ctrl__s_wdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 ctrl__s WSTRB" *) input [3:0]ctrl__s_wstrb;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 ctrl__s WVALID" *) input ctrl__s_wvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 ctrl__s WREADY" *) output ctrl__s_wready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 ctrl__s BRESP" *) output [1:0]ctrl__s_bresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 ctrl__s BVALID" *) output ctrl__s_bvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 ctrl__s BREADY" *) input ctrl__s_bready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 ctrl__s ARADDR" *) input [31:0]ctrl__s_araddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 ctrl__s ARPROT" *) input [2:0]ctrl__s_arprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 ctrl__s ARVALID" *) input ctrl__s_arvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 ctrl__s ARREADY" *) output ctrl__s_arready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 ctrl__s RDATA" *) output [31:0]ctrl__s_rdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 ctrl__s RRESP" *) output [1:0]ctrl__s_rresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 ctrl__s RVALID" *) output ctrl__s_rvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 ctrl__s RREADY" *) input ctrl__s_rready;
  output [8:0]iodly_00__tap;
  output [1:0]iodly_00__tap_load;
  output [8:0]iodly_01__tap;
  output [1:0]iodly_01__tap_load;
  output [8:0]iodly_02__tap;
  output [1:0]iodly_02__tap_load;
  output [8:0]iodly_03__tap;
  output [1:0]iodly_03__tap_load;
  output [8:0]iodly_04__tap;
  output [1:0]iodly_04__tap_load;
  output [8:0]iodly_05__tap;
  output [1:0]iodly_05__tap_load;
  output [8:0]iodly_06__tap;
  output [1:0]iodly_06__tap_load;
  output [8:0]iodly_07__tap;
  output [1:0]iodly_07__tap_load;
  output [8:0]iodly_08__tap;
  output [1:0]iodly_08__tap_load;
  output [8:0]iodly_09__tap;
  output [1:0]iodly_09__tap_load;
  output [8:0]iodly_10__tap;
  output [1:0]iodly_10__tap_load;
  output [8:0]iodly_11__tap;
  output [1:0]iodly_11__tap_load;
  output [8:0]iodly_12__tap;
  output [1:0]iodly_12__tap_load;
  output [8:0]iodly_13__tap;
  output [1:0]iodly_13__tap_load;
  output [8:0]iodly_14__tap;
  output [1:0]iodly_14__tap_load;
  output [8:0]iodly_15__tap;
  output [1:0]iodly_15__tap_load;
  output [8:0]iodly_16__tap;
  output [1:0]iodly_16__tap_load;
  output [8:0]iodly_17__tap;
  output [1:0]iodly_17__tap_load;
  output [8:0]iodly_18__tap;
  output [1:0]iodly_18__tap_load;
  output [8:0]iodly_19__tap;
  output [1:0]iodly_19__tap_load;
  output [8:0]iodly_20__tap;
  output [1:0]iodly_20__tap_load;
  output [8:0]iodly_21__tap;
  output [1:0]iodly_21__tap_load;
  output [8:0]iodly_22__tap;
  output [1:0]iodly_22__tap_load;
  output [8:0]iodly_23__tap;
  output [1:0]iodly_23__tap_load;
  output [8:0]iodly_24__tap;
  output [1:0]iodly_24__tap_load;
  output [8:0]iodly_25__tap;
  output [1:0]iodly_25__tap_load;
  output [8:0]iodly_26__tap;
  output [1:0]iodly_26__tap_load;
  output [8:0]iodly_27__tap;
  output [1:0]iodly_27__tap_load;
  output [8:0]iodly_28__tap;
  output [1:0]iodly_28__tap_load;
  output [8:0]iodly_29__tap;
  output [1:0]iodly_29__tap_load;
  output [8:0]iodly_30__tap;
  output [1:0]iodly_30__tap_load;
  output [8:0]iodly_31__tap;
  output [1:0]iodly_31__tap_load;
endmodule
