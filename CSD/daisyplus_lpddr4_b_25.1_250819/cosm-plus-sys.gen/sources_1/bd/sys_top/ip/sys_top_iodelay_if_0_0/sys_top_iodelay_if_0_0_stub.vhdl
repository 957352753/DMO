-- Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
-- Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.
-- --------------------------------------------------------------------------------
-- Tool Version: Vivado v.2025.1 (win64) Build 6140274 Thu May 22 00:12:29 MDT 2025
-- Date        : Tue Jul  8 14:34:43 2025
-- Host        : DESKTOP-JP2R5GF running 64-bit major release  (build 9200)
-- Command     : write_vhdl -force -mode synth_stub -rename_top sys_top_iodelay_if_0_0 -prefix
--               sys_top_iodelay_if_0_0_ sys_top_iodelay_if_0_2_stub.vhdl
-- Design      : sys_top_iodelay_if_0_2
-- Purpose     : Stub declaration of top-level module interface
-- Device      : xczu17eg-ffvc1760-2-e
-- --------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity sys_top_iodelay_if_0_0 is
  Port ( 
    \sys__clk\ : in STD_LOGIC;
    \sys__srstn\ : in STD_LOGIC;
    \ctrl__s_awaddr\ : in STD_LOGIC_VECTOR ( 31 downto 0 );
    \ctrl__s_awprot\ : in STD_LOGIC_VECTOR ( 2 downto 0 );
    \ctrl__s_awvalid\ : in STD_LOGIC;
    \ctrl__s_awready\ : out STD_LOGIC;
    \ctrl__s_wdata\ : in STD_LOGIC_VECTOR ( 31 downto 0 );
    \ctrl__s_wstrb\ : in STD_LOGIC_VECTOR ( 3 downto 0 );
    \ctrl__s_wvalid\ : in STD_LOGIC;
    \ctrl__s_wready\ : out STD_LOGIC;
    \ctrl__s_bresp\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \ctrl__s_bvalid\ : out STD_LOGIC;
    \ctrl__s_bready\ : in STD_LOGIC;
    \ctrl__s_araddr\ : in STD_LOGIC_VECTOR ( 31 downto 0 );
    \ctrl__s_arprot\ : in STD_LOGIC_VECTOR ( 2 downto 0 );
    \ctrl__s_arvalid\ : in STD_LOGIC;
    \ctrl__s_arready\ : out STD_LOGIC;
    \ctrl__s_rdata\ : out STD_LOGIC_VECTOR ( 31 downto 0 );
    \ctrl__s_rresp\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \ctrl__s_rvalid\ : out STD_LOGIC;
    \ctrl__s_rready\ : in STD_LOGIC;
    \iodly_00__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_00__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_01__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_01__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_02__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_02__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_03__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_03__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_04__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_04__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_05__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_05__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_06__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_06__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_07__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_07__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_08__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_08__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_09__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_09__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_10__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_10__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_11__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_11__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_12__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_12__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_13__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_13__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_14__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_14__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_15__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_15__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_16__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_16__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_17__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_17__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_18__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_18__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_19__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_19__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_20__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_20__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_21__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_21__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_22__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_22__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_23__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_23__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_24__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_24__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_25__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_25__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_26__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_26__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_27__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_27__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_28__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_28__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_29__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_29__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_30__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_30__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 );
    \iodly_31__tap\ : out STD_LOGIC_VECTOR ( 8 downto 0 );
    \iodly_31__tap_load\ : out STD_LOGIC_VECTOR ( 1 downto 0 )
  );

  attribute CHECK_LICENSE_TYPE : string;
  attribute CHECK_LICENSE_TYPE of sys_top_iodelay_if_0_0 : entity is "sys_top_iodelay_if_0_2,iodelay_if,{}";
  attribute DowngradeIPIdentifiedWarnings : string;
  attribute DowngradeIPIdentifiedWarnings of sys_top_iodelay_if_0_0 : entity is "yes";
  attribute IP_DEFINITION_SOURCE : string;
  attribute IP_DEFINITION_SOURCE of sys_top_iodelay_if_0_0 : entity is "package_project";
end sys_top_iodelay_if_0_0;

architecture stub of sys_top_iodelay_if_0_0 is
  attribute syn_black_box : boolean;
  attribute black_box_pad_pin : string;
  attribute syn_black_box of stub : architecture is true;
  attribute black_box_pad_pin of stub : architecture is "\sys__clk\,\sys__srstn\,\ctrl__s_awaddr\[31:0],\ctrl__s_awprot\[2:0],\ctrl__s_awvalid\,\ctrl__s_awready\,\ctrl__s_wdata\[31:0],\ctrl__s_wstrb\[3:0],\ctrl__s_wvalid\,\ctrl__s_wready\,\ctrl__s_bresp\[1:0],\ctrl__s_bvalid\,\ctrl__s_bready\,\ctrl__s_araddr\[31:0],\ctrl__s_arprot\[2:0],\ctrl__s_arvalid\,\ctrl__s_arready\,\ctrl__s_rdata\[31:0],\ctrl__s_rresp\[1:0],\ctrl__s_rvalid\,\ctrl__s_rready\,\iodly_00__tap\[8:0],\iodly_00__tap_load\[1:0],\iodly_01__tap\[8:0],\iodly_01__tap_load\[1:0],\iodly_02__tap\[8:0],\iodly_02__tap_load\[1:0],\iodly_03__tap\[8:0],\iodly_03__tap_load\[1:0],\iodly_04__tap\[8:0],\iodly_04__tap_load\[1:0],\iodly_05__tap\[8:0],\iodly_05__tap_load\[1:0],\iodly_06__tap\[8:0],\iodly_06__tap_load\[1:0],\iodly_07__tap\[8:0],\iodly_07__tap_load\[1:0],\iodly_08__tap\[8:0],\iodly_08__tap_load\[1:0],\iodly_09__tap\[8:0],\iodly_09__tap_load\[1:0],\iodly_10__tap\[8:0],\iodly_10__tap_load\[1:0],\iodly_11__tap\[8:0],\iodly_11__tap_load\[1:0],\iodly_12__tap\[8:0],\iodly_12__tap_load\[1:0],\iodly_13__tap\[8:0],\iodly_13__tap_load\[1:0],\iodly_14__tap\[8:0],\iodly_14__tap_load\[1:0],\iodly_15__tap\[8:0],\iodly_15__tap_load\[1:0],\iodly_16__tap\[8:0],\iodly_16__tap_load\[1:0],\iodly_17__tap\[8:0],\iodly_17__tap_load\[1:0],\iodly_18__tap\[8:0],\iodly_18__tap_load\[1:0],\iodly_19__tap\[8:0],\iodly_19__tap_load\[1:0],\iodly_20__tap\[8:0],\iodly_20__tap_load\[1:0],\iodly_21__tap\[8:0],\iodly_21__tap_load\[1:0],\iodly_22__tap\[8:0],\iodly_22__tap_load\[1:0],\iodly_23__tap\[8:0],\iodly_23__tap_load\[1:0],\iodly_24__tap\[8:0],\iodly_24__tap_load\[1:0],\iodly_25__tap\[8:0],\iodly_25__tap_load\[1:0],\iodly_26__tap\[8:0],\iodly_26__tap_load\[1:0],\iodly_27__tap\[8:0],\iodly_27__tap_load\[1:0],\iodly_28__tap\[8:0],\iodly_28__tap_load\[1:0],\iodly_29__tap\[8:0],\iodly_29__tap_load\[1:0],\iodly_30__tap\[8:0],\iodly_30__tap_load\[1:0],\iodly_31__tap\[8:0],\iodly_31__tap_load\[1:0]";
  attribute X_INTERFACE_INFO : string;
  attribute X_INTERFACE_INFO of \sys__clk\ : signal is "xilinx.com:signal:clock:1.0 sys__clk CLK";
  attribute X_INTERFACE_MODE : string;
  attribute X_INTERFACE_MODE of \sys__clk\ : signal is "slave";
  attribute X_INTERFACE_PARAMETER : string;
  attribute X_INTERFACE_PARAMETER of \sys__clk\ : signal is "XIL_INTERFACENAME sys__clk, ASSOCIATED_RESET sys__srstn, ASSOCIATED_BUSIF ctrl__s, FREQ_HZ 66666000, PHASE 0.0, CLK_DOMAIN sys_top_zynq_ultra_ps_e_0_0_pl_clk0, INSERT_VIP 0";
  attribute X_INTERFACE_INFO of \sys__srstn\ : signal is "xilinx.com:signal:reset:1.0 sys__srstn RST";
  attribute X_INTERFACE_MODE of \sys__srstn\ : signal is "slave";
  attribute X_INTERFACE_PARAMETER of \sys__srstn\ : signal is "XIL_INTERFACENAME sys__srstn, POLARITY ACTIVE_LOW, INSERT_VIP 0";
  attribute X_INTERFACE_INFO of \ctrl__s_awaddr\ : signal is "xilinx.com:interface:aximm:1.0 ctrl__s AWADDR";
  attribute X_INTERFACE_MODE of \ctrl__s_awaddr\ : signal is "slave";
  attribute X_INTERFACE_PARAMETER of \ctrl__s_awaddr\ : signal is "XIL_INTERFACENAME ctrl__s, DATA_WIDTH 32, PROTOCOL AXI4LITE, FREQ_HZ 66666000, ID_WIDTH 0, ADDR_WIDTH 32, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, READ_WRITE_MODE READ_WRITE, HAS_BURST 0, HAS_LOCK 0, HAS_PROT 1, HAS_CACHE 0, HAS_QOS 0, HAS_REGION 0, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, SUPPORTS_NARROW_BURST 0, NUM_READ_OUTSTANDING 1, NUM_WRITE_OUTSTANDING 1, MAX_BURST_LENGTH 1, PHASE 0.0, CLK_DOMAIN sys_top_zynq_ultra_ps_e_0_0_pl_clk0, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0";
  attribute X_INTERFACE_INFO of \ctrl__s_awprot\ : signal is "xilinx.com:interface:aximm:1.0 ctrl__s AWPROT";
  attribute X_INTERFACE_INFO of \ctrl__s_awvalid\ : signal is "xilinx.com:interface:aximm:1.0 ctrl__s AWVALID";
  attribute X_INTERFACE_INFO of \ctrl__s_awready\ : signal is "xilinx.com:interface:aximm:1.0 ctrl__s AWREADY";
  attribute X_INTERFACE_INFO of \ctrl__s_wdata\ : signal is "xilinx.com:interface:aximm:1.0 ctrl__s WDATA";
  attribute X_INTERFACE_INFO of \ctrl__s_wstrb\ : signal is "xilinx.com:interface:aximm:1.0 ctrl__s WSTRB";
  attribute X_INTERFACE_INFO of \ctrl__s_wvalid\ : signal is "xilinx.com:interface:aximm:1.0 ctrl__s WVALID";
  attribute X_INTERFACE_INFO of \ctrl__s_wready\ : signal is "xilinx.com:interface:aximm:1.0 ctrl__s WREADY";
  attribute X_INTERFACE_INFO of \ctrl__s_bresp\ : signal is "xilinx.com:interface:aximm:1.0 ctrl__s BRESP";
  attribute X_INTERFACE_INFO of \ctrl__s_bvalid\ : signal is "xilinx.com:interface:aximm:1.0 ctrl__s BVALID";
  attribute X_INTERFACE_INFO of \ctrl__s_bready\ : signal is "xilinx.com:interface:aximm:1.0 ctrl__s BREADY";
  attribute X_INTERFACE_INFO of \ctrl__s_araddr\ : signal is "xilinx.com:interface:aximm:1.0 ctrl__s ARADDR";
  attribute X_INTERFACE_INFO of \ctrl__s_arprot\ : signal is "xilinx.com:interface:aximm:1.0 ctrl__s ARPROT";
  attribute X_INTERFACE_INFO of \ctrl__s_arvalid\ : signal is "xilinx.com:interface:aximm:1.0 ctrl__s ARVALID";
  attribute X_INTERFACE_INFO of \ctrl__s_arready\ : signal is "xilinx.com:interface:aximm:1.0 ctrl__s ARREADY";
  attribute X_INTERFACE_INFO of \ctrl__s_rdata\ : signal is "xilinx.com:interface:aximm:1.0 ctrl__s RDATA";
  attribute X_INTERFACE_INFO of \ctrl__s_rresp\ : signal is "xilinx.com:interface:aximm:1.0 ctrl__s RRESP";
  attribute X_INTERFACE_INFO of \ctrl__s_rvalid\ : signal is "xilinx.com:interface:aximm:1.0 ctrl__s RVALID";
  attribute X_INTERFACE_INFO of \ctrl__s_rready\ : signal is "xilinx.com:interface:aximm:1.0 ctrl__s RREADY";
  attribute X_CORE_INFO : string;
  attribute X_CORE_INFO of stub : architecture is "iodelay_if,Vivado 2025.1";
begin
end;
