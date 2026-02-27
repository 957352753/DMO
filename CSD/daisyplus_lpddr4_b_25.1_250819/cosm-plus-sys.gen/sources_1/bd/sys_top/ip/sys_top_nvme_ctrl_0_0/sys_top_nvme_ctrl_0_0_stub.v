// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2025.1 (win64) Build 6140274 Thu May 22 00:12:29 MDT 2025
// Date        : Tue Jul 15 21:29:22 2025
// Host        : DESKTOP-JP2R5GF running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode synth_stub
//               d:/sources/DaisyPlus/2025.1/daisyplus_openssd_micron_4c2w_20251_20250708/cosm-plus-sys.gen/sources_1/bd/sys_top/ip/sys_top_nvme_ctrl_0_0/sys_top_nvme_ctrl_0_0_stub.v
// Design      : sys_top_nvme_ctrl_0_0
// Purpose     : Stub declaration of top-level module interface
// Device      : xczu17eg-ffvc1760-2-e
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* CHECK_LICENSE_TYPE = "sys_top_nvme_ctrl_0_0,s_axi_nvme,{}" *) (* DowngradeIPIdentifiedWarnings = "yes" *) (* IP_DEFINITION_SOURCE = "package_project" *) 
(* X_CORE_INFO = "s_axi_nvme,Vivado 2025.1" *) 
module sys_top_nvme_ctrl_0_0(s0_axi_aclk, s0_axi_aresetn, s0_axi_awaddr, 
  s0_axi_awready, s0_axi_awvalid, s0_axi_awprot, s0_axi_wvalid, s0_axi_wready, s0_axi_wdata, 
  s0_axi_wstrb, s0_axi_bvalid, s0_axi_bready, s0_axi_bresp, s0_axi_arvalid, s0_axi_arready, 
  s0_axi_araddr, s0_axi_arprot, s0_axi_rvalid, s0_axi_rready, s0_axi_rdata, s0_axi_rresp, 
  m0_axi_aclk, m0_axi_aresetn, m0_axi_awid, m0_axi_awaddr, m0_axi_awlen, m0_axi_awsize, 
  m0_axi_awburst, m0_axi_awlock, m0_axi_awcache, m0_axi_awprot, m0_axi_awregion, 
  m0_axi_awqos, m0_axi_awuser, m0_axi_awvalid, m0_axi_awready, m0_axi_wid, m0_axi_wdata, 
  m0_axi_wstrb, m0_axi_wlast, m0_axi_wuser, m0_axi_wvalid, m0_axi_wready, m0_axi_bid, 
  m0_axi_bresp, m0_axi_bvalid, m0_axi_buser, m0_axi_bready, m0_axi_arid, m0_axi_araddr, 
  m0_axi_arlen, m0_axi_arsize, m0_axi_arburst, m0_axi_arlock, m0_axi_arcache, m0_axi_arprot, 
  m0_axi_arregion, m0_axi_arqos, m0_axi_aruser, m0_axi_arvalid, m0_axi_arready, m0_axi_rid, 
  m0_axi_rdata, m0_axi_rresp, m0_axi_rlast, m0_axi_ruser, m0_axi_rvalid, m0_axi_rready, 
  dev_irq_assert, pcie_ref_clk_p, pcie_ref_clk_n, pcie_perst_n, pci_exp_txp, pci_exp_txn, 
  pci_exp_rxp, pci_exp_rxn, user_lnk_up)
/* synthesis syn_black_box black_box_pad_pin="s0_axi_aresetn,s0_axi_awaddr[31:0],s0_axi_awready,s0_axi_awvalid,s0_axi_awprot[2:0],s0_axi_wvalid,s0_axi_wready,s0_axi_wdata[31:0],s0_axi_wstrb[3:0],s0_axi_bvalid,s0_axi_bready,s0_axi_bresp[1:0],s0_axi_arvalid,s0_axi_arready,s0_axi_araddr[31:0],s0_axi_arprot[2:0],s0_axi_rvalid,s0_axi_rready,s0_axi_rdata[31:0],s0_axi_rresp[1:0],m0_axi_aresetn,m0_axi_awid[0:0],m0_axi_awaddr[31:0],m0_axi_awlen[7:0],m0_axi_awsize[2:0],m0_axi_awburst[1:0],m0_axi_awlock,m0_axi_awcache[3:0],m0_axi_awprot[2:0],m0_axi_awregion[3:0],m0_axi_awqos[3:0],m0_axi_awuser[0:0],m0_axi_awvalid,m0_axi_awready,m0_axi_wid[0:0],m0_axi_wdata[63:0],m0_axi_wstrb[7:0],m0_axi_wlast,m0_axi_wuser[0:0],m0_axi_wvalid,m0_axi_wready,m0_axi_bid[0:0],m0_axi_bresp[1:0],m0_axi_bvalid,m0_axi_buser[0:0],m0_axi_bready,m0_axi_arid[0:0],m0_axi_araddr[31:0],m0_axi_arlen[7:0],m0_axi_arsize[2:0],m0_axi_arburst[1:0],m0_axi_arlock,m0_axi_arcache[3:0],m0_axi_arprot[2:0],m0_axi_arregion[3:0],m0_axi_arqos[3:0],m0_axi_aruser[0:0],m0_axi_arvalid,m0_axi_arready,m0_axi_rid[0:0],m0_axi_rdata[63:0],m0_axi_rresp[1:0],m0_axi_rlast,m0_axi_ruser[0:0],m0_axi_rvalid,m0_axi_rready,dev_irq_assert,pcie_ref_clk_p,pcie_ref_clk_n,pcie_perst_n,pci_exp_txp[15:0],pci_exp_txn[15:0],pci_exp_rxp[15:0],pci_exp_rxn[15:0],user_lnk_up" */
/* synthesis syn_force_seq_prim="s0_axi_aclk" */
/* synthesis syn_force_seq_prim="m0_axi_aclk" */;
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 s0_axi_aclk CLK" *) (* X_INTERFACE_MODE = "slave" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME s0_axi_aclk, ASSOCIATED_BUSIF s0_axi, ASSOCIATED_RESET s0_axi_aresetn, FREQ_HZ 199998001, FREQ_TOLERANCE_HZ 0, PHASE 0.0, CLK_DOMAIN sys_top_zynq_ultra_ps_e_0_0_pl_clk2, INSERT_VIP 0" *) input s0_axi_aclk /* synthesis syn_isclock = 1 */;
  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 s0_axi_aresetn RST" *) (* X_INTERFACE_MODE = "slave" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME s0_axi_aresetn, POLARITY ACTIVE_LOW, INSERT_VIP 0" *) input s0_axi_aresetn;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s0_axi AWADDR" *) (* X_INTERFACE_MODE = "slave" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME s0_axi, DATA_WIDTH 32, PROTOCOL AXI4LITE, FREQ_HZ 199998001, ID_WIDTH 0, ADDR_WIDTH 32, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, READ_WRITE_MODE READ_WRITE, HAS_BURST 0, HAS_LOCK 0, HAS_PROT 1, HAS_CACHE 0, HAS_QOS 0, HAS_REGION 0, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, SUPPORTS_NARROW_BURST 0, NUM_READ_OUTSTANDING 1, NUM_WRITE_OUTSTANDING 1, MAX_BURST_LENGTH 1, PHASE 0.0, CLK_DOMAIN sys_top_zynq_ultra_ps_e_0_0_pl_clk2, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0" *) input [31:0]s0_axi_awaddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s0_axi AWREADY" *) output s0_axi_awready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s0_axi AWVALID" *) input s0_axi_awvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s0_axi AWPROT" *) input [2:0]s0_axi_awprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s0_axi WVALID" *) input s0_axi_wvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s0_axi WREADY" *) output s0_axi_wready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s0_axi WDATA" *) input [31:0]s0_axi_wdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s0_axi WSTRB" *) input [3:0]s0_axi_wstrb;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s0_axi BVALID" *) output s0_axi_bvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s0_axi BREADY" *) input s0_axi_bready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s0_axi BRESP" *) output [1:0]s0_axi_bresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s0_axi ARVALID" *) input s0_axi_arvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s0_axi ARREADY" *) output s0_axi_arready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s0_axi ARADDR" *) input [31:0]s0_axi_araddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s0_axi ARPROT" *) input [2:0]s0_axi_arprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s0_axi RVALID" *) output s0_axi_rvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s0_axi RREADY" *) input s0_axi_rready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s0_axi RDATA" *) output [31:0]s0_axi_rdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 s0_axi RRESP" *) output [1:0]s0_axi_rresp;
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 m0_axi_aclk CLK" *) (* X_INTERFACE_MODE = "slave" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME m0_axi_aclk, ASSOCIATED_BUSIF m0_axi, ASSOCIATED_RESET m0_axi_aresetn, FREQ_HZ 249997498, FREQ_TOLERANCE_HZ 0, PHASE 0.0, CLK_DOMAIN sys_top_zynq_ultra_ps_e_0_0_pl_clk3, INSERT_VIP 0" *) input m0_axi_aclk /* synthesis syn_isclock = 1 */;
  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 m0_axi_aresetn RST" *) (* X_INTERFACE_MODE = "slave" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME m0_axi_aresetn, POLARITY ACTIVE_LOW, INSERT_VIP 0" *) input m0_axi_aresetn;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi AWID" *) (* X_INTERFACE_MODE = "master" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME m0_axi, DATA_WIDTH 64, PROTOCOL AXI4, FREQ_HZ 249997498, ID_WIDTH 1, ADDR_WIDTH 32, AWUSER_WIDTH 1, ARUSER_WIDTH 1, WUSER_WIDTH 1, RUSER_WIDTH 1, BUSER_WIDTH 1, READ_WRITE_MODE READ_WRITE, HAS_BURST 1, HAS_LOCK 1, HAS_PROT 1, HAS_CACHE 1, HAS_QOS 1, HAS_REGION 1, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, SUPPORTS_NARROW_BURST 1, NUM_READ_OUTSTANDING 2, NUM_WRITE_OUTSTANDING 2, MAX_BURST_LENGTH 256, PHASE 0.0, CLK_DOMAIN sys_top_zynq_ultra_ps_e_0_0_pl_clk3, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0" *) output [0:0]m0_axi_awid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi AWADDR" *) output [31:0]m0_axi_awaddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi AWLEN" *) output [7:0]m0_axi_awlen;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi AWSIZE" *) output [2:0]m0_axi_awsize;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi AWBURST" *) output [1:0]m0_axi_awburst;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi AWLOCK" *) output m0_axi_awlock;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi AWCACHE" *) output [3:0]m0_axi_awcache;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi AWPROT" *) output [2:0]m0_axi_awprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi AWREGION" *) output [3:0]m0_axi_awregion;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi AWQOS" *) output [3:0]m0_axi_awqos;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi AWUSER" *) output [0:0]m0_axi_awuser;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi AWVALID" *) output m0_axi_awvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi AWREADY" *) input m0_axi_awready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi WID" *) output [0:0]m0_axi_wid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi WDATA" *) output [63:0]m0_axi_wdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi WSTRB" *) output [7:0]m0_axi_wstrb;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi WLAST" *) output m0_axi_wlast;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi WUSER" *) output [0:0]m0_axi_wuser;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi WVALID" *) output m0_axi_wvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi WREADY" *) input m0_axi_wready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi BID" *) input [0:0]m0_axi_bid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi BRESP" *) input [1:0]m0_axi_bresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi BVALID" *) input m0_axi_bvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi BUSER" *) input [0:0]m0_axi_buser;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi BREADY" *) output m0_axi_bready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi ARID" *) output [0:0]m0_axi_arid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi ARADDR" *) output [31:0]m0_axi_araddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi ARLEN" *) output [7:0]m0_axi_arlen;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi ARSIZE" *) output [2:0]m0_axi_arsize;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi ARBURST" *) output [1:0]m0_axi_arburst;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi ARLOCK" *) output m0_axi_arlock;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi ARCACHE" *) output [3:0]m0_axi_arcache;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi ARPROT" *) output [2:0]m0_axi_arprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi ARREGION" *) output [3:0]m0_axi_arregion;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi ARQOS" *) output [3:0]m0_axi_arqos;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi ARUSER" *) output [0:0]m0_axi_aruser;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi ARVALID" *) output m0_axi_arvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi ARREADY" *) input m0_axi_arready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi RID" *) input [0:0]m0_axi_rid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi RDATA" *) input [63:0]m0_axi_rdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi RRESP" *) input [1:0]m0_axi_rresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi RLAST" *) input m0_axi_rlast;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi RUSER" *) input [0:0]m0_axi_ruser;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi RVALID" *) input m0_axi_rvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 m0_axi RREADY" *) output m0_axi_rready;
  output dev_irq_assert;
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 pcie_ref_clk_p CLK, xilinx.com:interface:diff_clock:1.0 pcie_ref CLK_P" *) (* X_INTERFACE_MODE = "slave pcie_ref" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME pcie_ref, CAN_DEBUG false, FREQ_HZ 100000000" *) input pcie_ref_clk_p;
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 pcie_ref_clk_n CLK, xilinx.com:interface:diff_clock:1.0 pcie_ref CLK_N" *) (* X_INTERFACE_MODE = "slave pcie_ref_clk_n" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME pcie_ref_clk_n, ASSOCIATED_RESET pcie_perst_n, FREQ_HZ 100000000, FREQ_TOLERANCE_HZ 0, PHASE 0.0, INSERT_VIP 0" *) input pcie_ref_clk_n;
  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 pcie_perst_n RST" *) (* X_INTERFACE_MODE = "slave" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME pcie_perst_n, POLARITY ACTIVE_LOW, INSERT_VIP 0" *) input pcie_perst_n;
  (* X_INTERFACE_INFO = "xilinx.com:interface:pcie_7x_mgt:1.0 pci_exp txp" *) (* X_INTERFACE_MODE = "master" *) output [15:0]pci_exp_txp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:pcie_7x_mgt:1.0 pci_exp txn" *) output [15:0]pci_exp_txn;
  (* X_INTERFACE_INFO = "xilinx.com:interface:pcie_7x_mgt:1.0 pci_exp rxp" *) input [15:0]pci_exp_rxp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:pcie_7x_mgt:1.0 pci_exp rxn" *) input [15:0]pci_exp_rxn;
  output user_lnk_up;
endmodule
