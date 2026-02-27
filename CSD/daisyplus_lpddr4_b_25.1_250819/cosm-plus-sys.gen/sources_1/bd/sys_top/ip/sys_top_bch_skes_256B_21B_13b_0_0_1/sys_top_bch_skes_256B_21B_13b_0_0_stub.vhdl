-- Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
-- Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.
-- --------------------------------------------------------------------------------
-- Tool Version: Vivado v.2025.1 (win64) Build 6140274 Thu May 22 00:12:29 MDT 2025
-- Date        : Tue Jul  8 14:28:46 2025
-- Host        : DESKTOP-JP2R5GF running 64-bit major release  (build 9200)
-- Command     : write_vhdl -force -mode synth_stub -rename_top sys_top_bch_skes_256B_21B_13b_0_0 -prefix
--               sys_top_bch_skes_256B_21B_13b_0_0_ sys_top_bch_skes_256B_21B_13b_0_0_stub.vhdl
-- Design      : sys_top_bch_skes_256B_21B_13b_0_0
-- Purpose     : Stub declaration of top-level module interface
-- Device      : xczu17eg-ffvc1760-2-e
-- --------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity sys_top_bch_skes_256B_21B_13b_0_0 is
  Port ( 
    iClock : in STD_LOGIC;
    iReset : in STD_LOGIC;
    oSharedKESReady_0 : out STD_LOGIC;
    iErrorDetectionEnd_0 : in STD_LOGIC_VECTOR ( 1 downto 0 );
    iDecodeNeeded_0 : in STD_LOGIC_VECTOR ( 1 downto 0 );
    iSyndromes_0 : in STD_LOGIC_VECTOR ( 647 downto 0 );
    iCSAvailable_0 : in STD_LOGIC;
    oIntraSharedKESEnd_0 : out STD_LOGIC;
    oErroredChunk_0 : out STD_LOGIC_VECTOR ( 1 downto 0 );
    oCorrectionFail_0 : out STD_LOGIC_VECTOR ( 1 downto 0 );
    oClusterErrorCount_0 : out STD_LOGIC_VECTOR ( 17 downto 0 );
    oELPCoefficients_0 : out STD_LOGIC_VECTOR ( 359 downto 0 );
    oSharedKESReady_1 : out STD_LOGIC;
    iErrorDetectionEnd_1 : in STD_LOGIC_VECTOR ( 1 downto 0 );
    iDecodeNeeded_1 : in STD_LOGIC_VECTOR ( 1 downto 0 );
    iSyndromes_1 : in STD_LOGIC_VECTOR ( 647 downto 0 );
    iCSAvailable_1 : in STD_LOGIC;
    oIntraSharedKESEnd_1 : out STD_LOGIC;
    oErroredChunk_1 : out STD_LOGIC_VECTOR ( 1 downto 0 );
    oCorrectionFail_1 : out STD_LOGIC_VECTOR ( 1 downto 0 );
    oClusterErrorCount_1 : out STD_LOGIC_VECTOR ( 17 downto 0 );
    oELPCoefficients_1 : out STD_LOGIC_VECTOR ( 359 downto 0 );
    oSharedKESReady_2 : out STD_LOGIC;
    iErrorDetectionEnd_2 : in STD_LOGIC_VECTOR ( 1 downto 0 );
    iDecodeNeeded_2 : in STD_LOGIC_VECTOR ( 1 downto 0 );
    iSyndromes_2 : in STD_LOGIC_VECTOR ( 647 downto 0 );
    iCSAvailable_2 : in STD_LOGIC;
    oIntraSharedKESEnd_2 : out STD_LOGIC;
    oErroredChunk_2 : out STD_LOGIC_VECTOR ( 1 downto 0 );
    oCorrectionFail_2 : out STD_LOGIC_VECTOR ( 1 downto 0 );
    oClusterErrorCount_2 : out STD_LOGIC_VECTOR ( 17 downto 0 );
    oELPCoefficients_2 : out STD_LOGIC_VECTOR ( 359 downto 0 );
    oSharedKESReady_3 : out STD_LOGIC;
    iErrorDetectionEnd_3 : in STD_LOGIC_VECTOR ( 1 downto 0 );
    iDecodeNeeded_3 : in STD_LOGIC_VECTOR ( 1 downto 0 );
    iSyndromes_3 : in STD_LOGIC_VECTOR ( 647 downto 0 );
    iCSAvailable_3 : in STD_LOGIC;
    oIntraSharedKESEnd_3 : out STD_LOGIC;
    oErroredChunk_3 : out STD_LOGIC_VECTOR ( 1 downto 0 );
    oCorrectionFail_3 : out STD_LOGIC_VECTOR ( 1 downto 0 );
    oClusterErrorCount_3 : out STD_LOGIC_VECTOR ( 17 downto 0 );
    oELPCoefficients_3 : out STD_LOGIC_VECTOR ( 359 downto 0 )
  );

  attribute CHECK_LICENSE_TYPE : string;
  attribute CHECK_LICENSE_TYPE of sys_top_bch_skes_256B_21B_13b_0_0 : entity is "sys_top_bch_skes_256B_21B_13b_0_0,SharedKESTop,{}";
  attribute DowngradeIPIdentifiedWarnings : string;
  attribute DowngradeIPIdentifiedWarnings of sys_top_bch_skes_256B_21B_13b_0_0 : entity is "yes";
end sys_top_bch_skes_256B_21B_13b_0_0;

architecture stub of sys_top_bch_skes_256B_21B_13b_0_0 is
  attribute syn_black_box : boolean;
  attribute black_box_pad_pin : string;
  attribute syn_black_box of stub : architecture is true;
  attribute black_box_pad_pin of stub : architecture is "iClock,iReset,oSharedKESReady_0,iErrorDetectionEnd_0[1:0],iDecodeNeeded_0[1:0],iSyndromes_0[647:0],iCSAvailable_0,oIntraSharedKESEnd_0,oErroredChunk_0[1:0],oCorrectionFail_0[1:0],oClusterErrorCount_0[17:0],oELPCoefficients_0[359:0],oSharedKESReady_1,iErrorDetectionEnd_1[1:0],iDecodeNeeded_1[1:0],iSyndromes_1[647:0],iCSAvailable_1,oIntraSharedKESEnd_1,oErroredChunk_1[1:0],oCorrectionFail_1[1:0],oClusterErrorCount_1[17:0],oELPCoefficients_1[359:0],oSharedKESReady_2,iErrorDetectionEnd_2[1:0],iDecodeNeeded_2[1:0],iSyndromes_2[647:0],iCSAvailable_2,oIntraSharedKESEnd_2,oErroredChunk_2[1:0],oCorrectionFail_2[1:0],oClusterErrorCount_2[17:0],oELPCoefficients_2[359:0],oSharedKESReady_3,iErrorDetectionEnd_3[1:0],iDecodeNeeded_3[1:0],iSyndromes_3[647:0],iCSAvailable_3,oIntraSharedKESEnd_3,oErroredChunk_3[1:0],oCorrectionFail_3[1:0],oClusterErrorCount_3[17:0],oELPCoefficients_3[359:0]";
  attribute X_INTERFACE_INFO : string;
  attribute X_INTERFACE_INFO of iClock : signal is "xilinx.com:signal:clock:1.0 signal_clock CLK";
  attribute X_INTERFACE_MODE : string;
  attribute X_INTERFACE_MODE of iClock : signal is "slave";
  attribute X_INTERFACE_PARAMETER : string;
  attribute X_INTERFACE_PARAMETER of iClock : signal is "XIL_INTERFACENAME signal_clock, ASSOCIATED_RESET iReset, ASSOCIATED_BUSIF bch_skes_ch0_if:bch_skes_ch1_if:bch_skes_ch2_if:bch_skes_ch3_if, FREQ_HZ 66666000, PHASE 0.0, CLK_DOMAIN sys_top_zynq_ultra_ps_e_0_0_pl_clk0, INSERT_VIP 0";
  attribute X_INTERFACE_INFO of iReset : signal is "xilinx.com:signal:reset:1.0 signal_reset RST";
  attribute X_INTERFACE_MODE of iReset : signal is "slave";
  attribute X_INTERFACE_PARAMETER of iReset : signal is "XIL_INTERFACENAME signal_reset, POLARITY ACTIVE_HIGH, INSERT_VIP 0";
  attribute X_INTERFACE_INFO of oSharedKESReady_0 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch0_if SharedKESReady";
  attribute X_INTERFACE_MODE of oSharedKESReady_0 : signal is "slave";
  attribute X_INTERFACE_INFO of iErrorDetectionEnd_0 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch0_if ErrorDetectionEnd";
  attribute X_INTERFACE_INFO of iDecodeNeeded_0 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch0_if DecodeNeeded";
  attribute X_INTERFACE_INFO of iSyndromes_0 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch0_if Syndromes";
  attribute X_INTERFACE_INFO of iCSAvailable_0 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch0_if CSAvailable";
  attribute X_INTERFACE_INFO of oIntraSharedKESEnd_0 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch0_if IntraSharedKESEnd";
  attribute X_INTERFACE_INFO of oErroredChunk_0 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch0_if ErroredChunk";
  attribute X_INTERFACE_INFO of oCorrectionFail_0 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch0_if CorrectionFail";
  attribute X_INTERFACE_INFO of oClusterErrorCount_0 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch0_if ErrorCount";
  attribute X_INTERFACE_INFO of oELPCoefficients_0 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch0_if ELPCoefficients";
  attribute X_INTERFACE_INFO of oSharedKESReady_1 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch1_if SharedKESReady";
  attribute X_INTERFACE_MODE of oSharedKESReady_1 : signal is "slave";
  attribute X_INTERFACE_INFO of iErrorDetectionEnd_1 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch1_if ErrorDetectionEnd";
  attribute X_INTERFACE_INFO of iDecodeNeeded_1 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch1_if DecodeNeeded";
  attribute X_INTERFACE_INFO of iSyndromes_1 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch1_if Syndromes";
  attribute X_INTERFACE_INFO of iCSAvailable_1 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch1_if CSAvailable";
  attribute X_INTERFACE_INFO of oIntraSharedKESEnd_1 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch1_if IntraSharedKESEnd";
  attribute X_INTERFACE_INFO of oErroredChunk_1 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch1_if ErroredChunk";
  attribute X_INTERFACE_INFO of oCorrectionFail_1 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch1_if CorrectionFail";
  attribute X_INTERFACE_INFO of oClusterErrorCount_1 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch1_if ErrorCount";
  attribute X_INTERFACE_INFO of oELPCoefficients_1 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch1_if ELPCoefficients";
  attribute X_INTERFACE_INFO of oSharedKESReady_2 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch2_if SharedKESReady";
  attribute X_INTERFACE_MODE of oSharedKESReady_2 : signal is "slave";
  attribute X_INTERFACE_INFO of iErrorDetectionEnd_2 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch2_if ErrorDetectionEnd";
  attribute X_INTERFACE_INFO of iDecodeNeeded_2 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch2_if DecodeNeeded";
  attribute X_INTERFACE_INFO of iSyndromes_2 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch2_if Syndromes";
  attribute X_INTERFACE_INFO of iCSAvailable_2 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch2_if CSAvailable";
  attribute X_INTERFACE_INFO of oIntraSharedKESEnd_2 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch2_if IntraSharedKESEnd";
  attribute X_INTERFACE_INFO of oErroredChunk_2 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch2_if ErroredChunk";
  attribute X_INTERFACE_INFO of oCorrectionFail_2 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch2_if CorrectionFail";
  attribute X_INTERFACE_INFO of oClusterErrorCount_2 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch2_if ErrorCount";
  attribute X_INTERFACE_INFO of oELPCoefficients_2 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch2_if ELPCoefficients";
  attribute X_INTERFACE_INFO of oSharedKESReady_3 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch3_if SharedKESReady";
  attribute X_INTERFACE_MODE of oSharedKESReady_3 : signal is "slave";
  attribute X_INTERFACE_INFO of iErrorDetectionEnd_3 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch3_if ErrorDetectionEnd";
  attribute X_INTERFACE_INFO of iDecodeNeeded_3 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch3_if DecodeNeeded";
  attribute X_INTERFACE_INFO of iSyndromes_3 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch3_if Syndromes";
  attribute X_INTERFACE_INFO of iCSAvailable_3 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch3_if CSAvailable";
  attribute X_INTERFACE_INFO of oIntraSharedKESEnd_3 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch3_if IntraSharedKESEnd";
  attribute X_INTERFACE_INFO of oErroredChunk_3 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch3_if ErroredChunk";
  attribute X_INTERFACE_INFO of oCorrectionFail_3 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch3_if CorrectionFail";
  attribute X_INTERFACE_INFO of oClusterErrorCount_3 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch3_if ErrorCount";
  attribute X_INTERFACE_INFO of oELPCoefficients_3 : signal is "enclab:user:bch_skes_if:1.0 bch_skes_ch3_if ELPCoefficients";
  attribute X_CORE_INFO : string;
  attribute X_CORE_INFO of stub : architecture is "SharedKESTop,Vivado 2025.1";
begin
end;
