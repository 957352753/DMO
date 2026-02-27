-- Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
-- Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.
-- --------------------------------------------------------------------------------
-- Tool Version: Vivado v.2025.1 (win64) Build 6140274 Thu May 22 00:12:29 MDT 2025
-- Date        : Tue Jul 15 21:35:13 2025
-- Host        : DESKTOP-JP2R5GF running 64-bit major release  (build 9200)
-- Command     : write_vhdl -force -mode synth_stub
--               d:/sources/DaisyPlus/2025.1/daisyplus_openssd_micron_4c2w_20251_20250708/cosm-plus-sys.gen/sources_1/bd/sys_top/ip/sys_top_v2nfc_0_0/sys_top_v2nfc_0_0_stub.vhdl
-- Design      : sys_top_v2nfc_0_0
-- Purpose     : Stub declaration of top-level module interface
-- Device      : xczu17eg-ffvc1760-2-e
-- --------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity sys_top_v2nfc_0_0 is
  Port ( 
    iSystemClock : in STD_LOGIC;
    iDelayRefClock : in STD_LOGIC;
    iOutputDrivingClock : in STD_LOGIC;
    iOutputStrobeClock : in STD_LOGIC;
    iReset : in STD_LOGIC;
    iOpcode : in STD_LOGIC_VECTOR ( 5 downto 0 );
    iTargetID : in STD_LOGIC_VECTOR ( 4 downto 0 );
    iSourceID : in STD_LOGIC_VECTOR ( 4 downto 0 );
    iAddress : in STD_LOGIC_VECTOR ( 31 downto 0 );
    iLength : in STD_LOGIC_VECTOR ( 15 downto 0 );
    iCMDValid : in STD_LOGIC;
    oCMDReady : out STD_LOGIC;
    iWriteData : in STD_LOGIC_VECTOR ( 31 downto 0 );
    iWriteLast : in STD_LOGIC;
    iWriteValid : in STD_LOGIC;
    oWriteReady : out STD_LOGIC;
    oReadData : out STD_LOGIC_VECTOR ( 31 downto 0 );
    oReadLast : out STD_LOGIC;
    oReadValid : out STD_LOGIC;
    iReadReady : in STD_LOGIC;
    oReadyBusy : out STD_LOGIC_VECTOR ( 7 downto 0 );
    IO_NAND_DQS_P : inout STD_LOGIC;
    IO_NAND_DQS_N : inout STD_LOGIC;
    IO_NAND_DQ : inout STD_LOGIC_VECTOR ( 7 downto 0 );
    O_NAND_CE : out STD_LOGIC_VECTOR ( 7 downto 0 );
    O_NAND_WE : out STD_LOGIC;
    O_NAND_RE_P : out STD_LOGIC;
    O_NAND_RE_N : out STD_LOGIC;
    O_NAND_ALE : out STD_LOGIC;
    O_NAND_CLE : out STD_LOGIC;
    I_NAND_RB : in STD_LOGIC_VECTOR ( 7 downto 0 );
    O_NAND_WP : out STD_LOGIC;
    iDQSIDelayTap : in STD_LOGIC_VECTOR ( 8 downto 0 );
    iDQSIDelayTapLoad : in STD_LOGIC_VECTOR ( 1 downto 0 );
    iDQ0IDelayTap : in STD_LOGIC_VECTOR ( 8 downto 0 );
    iDQ0IDelayTapLoad : in STD_LOGIC_VECTOR ( 1 downto 0 );
    iDQ1IDelayTap : in STD_LOGIC_VECTOR ( 8 downto 0 );
    iDQ1IDelayTapLoad : in STD_LOGIC_VECTOR ( 1 downto 0 );
    iDQ2IDelayTap : in STD_LOGIC_VECTOR ( 8 downto 0 );
    iDQ2IDelayTapLoad : in STD_LOGIC_VECTOR ( 1 downto 0 );
    iDQ3IDelayTap : in STD_LOGIC_VECTOR ( 8 downto 0 );
    iDQ3IDelayTapLoad : in STD_LOGIC_VECTOR ( 1 downto 0 );
    iDQ4IDelayTap : in STD_LOGIC_VECTOR ( 8 downto 0 );
    iDQ4IDelayTapLoad : in STD_LOGIC_VECTOR ( 1 downto 0 );
    iDQ5IDelayTap : in STD_LOGIC_VECTOR ( 8 downto 0 );
    iDQ5IDelayTapLoad : in STD_LOGIC_VECTOR ( 1 downto 0 );
    iDQ6IDelayTap : in STD_LOGIC_VECTOR ( 8 downto 0 );
    iDQ6IDelayTapLoad : in STD_LOGIC_VECTOR ( 1 downto 0 );
    iDQ7IDelayTap : in STD_LOGIC_VECTOR ( 8 downto 0 );
    iDQ7IDelayTapLoad : in STD_LOGIC_VECTOR ( 1 downto 0 )
  );

  attribute CHECK_LICENSE_TYPE : string;
  attribute CHECK_LICENSE_TYPE of sys_top_v2nfc_0_0 : entity is "sys_top_v2nfc_0_0,NFC_Toggle_Top_DDR100,{}";
  attribute DowngradeIPIdentifiedWarnings : string;
  attribute DowngradeIPIdentifiedWarnings of sys_top_v2nfc_0_0 : entity is "yes";
  attribute IP_DEFINITION_SOURCE : string;
  attribute IP_DEFINITION_SOURCE of sys_top_v2nfc_0_0 : entity is "package_project";
end sys_top_v2nfc_0_0;

architecture stub of sys_top_v2nfc_0_0 is
  attribute syn_black_box : boolean;
  attribute black_box_pad_pin : string;
  attribute syn_black_box of stub : architecture is true;
  attribute black_box_pad_pin of stub : architecture is "iSystemClock,iDelayRefClock,iOutputDrivingClock,iOutputStrobeClock,iReset,iOpcode[5:0],iTargetID[4:0],iSourceID[4:0],iAddress[31:0],iLength[15:0],iCMDValid,oCMDReady,iWriteData[31:0],iWriteLast,iWriteValid,oWriteReady,oReadData[31:0],oReadLast,oReadValid,iReadReady,oReadyBusy[7:0],IO_NAND_DQS_P,IO_NAND_DQS_N,IO_NAND_DQ[7:0],O_NAND_CE[7:0],O_NAND_WE,O_NAND_RE_P,O_NAND_RE_N,O_NAND_ALE,O_NAND_CLE,I_NAND_RB[7:0],O_NAND_WP,iDQSIDelayTap[8:0],iDQSIDelayTapLoad[1:0],iDQ0IDelayTap[8:0],iDQ0IDelayTapLoad[1:0],iDQ1IDelayTap[8:0],iDQ1IDelayTapLoad[1:0],iDQ2IDelayTap[8:0],iDQ2IDelayTapLoad[1:0],iDQ3IDelayTap[8:0],iDQ3IDelayTapLoad[1:0],iDQ4IDelayTap[8:0],iDQ4IDelayTapLoad[1:0],iDQ5IDelayTap[8:0],iDQ5IDelayTapLoad[1:0],iDQ6IDelayTap[8:0],iDQ6IDelayTapLoad[1:0],iDQ7IDelayTap[8:0],iDQ7IDelayTapLoad[1:0]";
  attribute X_INTERFACE_INFO : string;
  attribute X_INTERFACE_INFO of iSystemClock : signal is "xilinx.com:signal:clock:1.0 iSystemClock CLK";
  attribute X_INTERFACE_MODE : string;
  attribute X_INTERFACE_MODE of iSystemClock : signal is "slave";
  attribute X_INTERFACE_PARAMETER : string;
  attribute X_INTERFACE_PARAMETER of iSystemClock : signal is "XIL_INTERFACENAME iSystemClock, ASSOCIATED_RESET iReset, ASSOCIATED_BUSIF v2nfc_if, FREQ_HZ 66666000, FREQ_TOLERANCE_HZ 0, PHASE 0.0, CLK_DOMAIN /pll_bank10_clk_out1, INSERT_VIP 0";
  attribute X_INTERFACE_INFO of iReset : signal is "xilinx.com:signal:reset:1.0 iReset RST";
  attribute X_INTERFACE_MODE of iReset : signal is "slave";
  attribute X_INTERFACE_PARAMETER of iReset : signal is "XIL_INTERFACENAME iReset, POLARITY ACTIVE_HIGH, INSERT_VIP 0";
  attribute X_INTERFACE_INFO of iOpcode : signal is "enclab:user:v2nfc_if:1.0 v2nfc_if Opcode";
  attribute X_INTERFACE_MODE of iOpcode : signal is "slave";
  attribute X_INTERFACE_INFO of iTargetID : signal is "enclab:user:v2nfc_if:1.0 v2nfc_if TargetID";
  attribute X_INTERFACE_INFO of iSourceID : signal is "enclab:user:v2nfc_if:1.0 v2nfc_if SourceID";
  attribute X_INTERFACE_INFO of iAddress : signal is "enclab:user:v2nfc_if:1.0 v2nfc_if Address";
  attribute X_INTERFACE_INFO of iLength : signal is "enclab:user:v2nfc_if:1.0 v2nfc_if Length";
  attribute X_INTERFACE_INFO of iCMDValid : signal is "enclab:user:v2nfc_if:1.0 v2nfc_if CMDValid";
  attribute X_INTERFACE_INFO of oCMDReady : signal is "enclab:user:v2nfc_if:1.0 v2nfc_if CMDReady";
  attribute X_INTERFACE_INFO of iWriteData : signal is "enclab:user:v2nfc_if:1.0 v2nfc_if WriteData";
  attribute X_INTERFACE_INFO of iWriteLast : signal is "enclab:user:v2nfc_if:1.0 v2nfc_if WriteLast";
  attribute X_INTERFACE_INFO of iWriteValid : signal is "enclab:user:v2nfc_if:1.0 v2nfc_if WriteValid";
  attribute X_INTERFACE_INFO of oWriteReady : signal is "enclab:user:v2nfc_if:1.0 v2nfc_if WriteReady";
  attribute X_INTERFACE_INFO of oReadData : signal is "enclab:user:v2nfc_if:1.0 v2nfc_if ReadData";
  attribute X_INTERFACE_INFO of oReadLast : signal is "enclab:user:v2nfc_if:1.0 v2nfc_if ReadLast";
  attribute X_INTERFACE_INFO of oReadValid : signal is "enclab:user:v2nfc_if:1.0 v2nfc_if ReadValid";
  attribute X_INTERFACE_INFO of iReadReady : signal is "enclab:user:v2nfc_if:1.0 v2nfc_if ReadReady";
  attribute X_INTERFACE_INFO of IO_NAND_DQS_P : signal is "enclab:user:nand_if:1.0 nand_if NAND_DQS_P";
  attribute X_INTERFACE_MODE of IO_NAND_DQS_P : signal is "master";
  attribute X_INTERFACE_INFO of IO_NAND_DQS_N : signal is "enclab:user:nand_if:1.0 nand_if NAND_DQS_N";
  attribute X_INTERFACE_INFO of IO_NAND_DQ : signal is "enclab:user:nand_if:1.0 nand_if NAND_DQ";
  attribute X_INTERFACE_INFO of O_NAND_CE : signal is "enclab:user:nand_if:1.0 nand_if NAND_CE";
  attribute X_INTERFACE_INFO of O_NAND_WE : signal is "enclab:user:nand_if:1.0 nand_if NAND_WE";
  attribute X_INTERFACE_INFO of O_NAND_RE_P : signal is "enclab:user:nand_if:1.0 nand_if NAND_RE_P";
  attribute X_INTERFACE_INFO of O_NAND_RE_N : signal is "enclab:user:nand_if:1.0 nand_if NAND_RE_N";
  attribute X_INTERFACE_INFO of O_NAND_ALE : signal is "enclab:user:nand_if:1.0 nand_if NAND_ALE";
  attribute X_INTERFACE_INFO of O_NAND_CLE : signal is "enclab:user:nand_if:1.0 nand_if NAND_CLE";
  attribute X_INTERFACE_INFO of I_NAND_RB : signal is "enclab:user:nand_if:1.0 nand_if NAND_RB";
  attribute X_INTERFACE_INFO of O_NAND_WP : signal is "enclab:user:nand_if:1.0 nand_if NAND_WP";
  attribute X_CORE_INFO : string;
  attribute X_CORE_INFO of stub : architecture is "NFC_Toggle_Top_DDR100,Vivado 2025.1";
begin
end;
