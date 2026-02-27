-- Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
-- Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.
-- --------------------------------------------------------------------------------
-- Tool Version: Vivado v.2025.1 (win64) Build 6140274 Thu May 22 00:12:29 MDT 2025
-- Date        : Tue Jul  8 14:28:00 2025
-- Host        : DESKTOP-JP2R5GF running 64-bit major release  (build 9200)
-- Command     : write_vhdl -force -mode synth_stub -rename_top sys_top_bch_sccs_256B_21B_13b_0_0 -prefix
--               sys_top_bch_sccs_256B_21B_13b_0_0_ sys_top_bch_sccs_256B_21B_13b_0_0_stub.vhdl
-- Design      : sys_top_bch_sccs_256B_21B_13b_0_0
-- Purpose     : Stub declaration of top-level module interface
-- Device      : xczu17eg-ffvc1760-2-e
-- --------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity sys_top_bch_sccs_256B_21B_13b_0_0 is
  Port ( 
    iClock : in STD_LOGIC;
    iReset : in STD_LOGIC;
    iToECCWOpcode : in STD_LOGIC_VECTOR ( 5 downto 0 );
    iToECCWTargetID : in STD_LOGIC_VECTOR ( 4 downto 0 );
    iToECCWSourceID : in STD_LOGIC_VECTOR ( 4 downto 0 );
    iToECCWAddress : in STD_LOGIC_VECTOR ( 31 downto 0 );
    iToECCWLength : in STD_LOGIC_VECTOR ( 15 downto 0 );
    iToECCWCmdValid : in STD_LOGIC;
    oToECCWCmdReady : out STD_LOGIC;
    iToECCWData : in STD_LOGIC_VECTOR ( 31 downto 0 );
    iToECCWValid : in STD_LOGIC;
    iToECCWLast : in STD_LOGIC;
    oToECCWReady : out STD_LOGIC;
    iToECCROpcode : in STD_LOGIC_VECTOR ( 5 downto 0 );
    iToECCRTargetID : in STD_LOGIC_VECTOR ( 4 downto 0 );
    iToECCRSourceID : in STD_LOGIC_VECTOR ( 4 downto 0 );
    iToECCRAddress : in STD_LOGIC_VECTOR ( 31 downto 0 );
    iToECCRLength : in STD_LOGIC_VECTOR ( 15 downto 0 );
    iToECCRCmdValid : in STD_LOGIC;
    oToECCRCmdReady : out STD_LOGIC;
    oToECCRData : out STD_LOGIC_VECTOR ( 31 downto 0 );
    oToECCRValid : out STD_LOGIC;
    oToECCRLast : out STD_LOGIC;
    iToECCRReady : in STD_LOGIC;
    ofromECCWOpcode : out STD_LOGIC_VECTOR ( 5 downto 0 );
    ofromECCWTargetID : out STD_LOGIC_VECTOR ( 4 downto 0 );
    ofromECCWSourceID : out STD_LOGIC_VECTOR ( 4 downto 0 );
    ofromECCWAddress : out STD_LOGIC_VECTOR ( 31 downto 0 );
    ofromECCWLength : out STD_LOGIC_VECTOR ( 15 downto 0 );
    ofromECCWCmdValid : out STD_LOGIC;
    ifromECCWCmdReady : in STD_LOGIC;
    ofromECCWData : out STD_LOGIC_VECTOR ( 31 downto 0 );
    ofromECCWValid : out STD_LOGIC;
    ofromECCWLast : out STD_LOGIC;
    ifromECCWReady : in STD_LOGIC;
    ofromECCROpcode : out STD_LOGIC_VECTOR ( 5 downto 0 );
    ofromECCRTargetID : out STD_LOGIC_VECTOR ( 4 downto 0 );
    ofromECCRSourceID : out STD_LOGIC_VECTOR ( 4 downto 0 );
    ofromECCRAddress : out STD_LOGIC_VECTOR ( 31 downto 0 );
    ofromECCRLength : out STD_LOGIC_VECTOR ( 15 downto 0 );
    ofromECCRCmdValid : out STD_LOGIC;
    ifromECCRCmdReady : in STD_LOGIC;
    ifromECCRData : in STD_LOGIC_VECTOR ( 31 downto 0 );
    ifromECCRValid : in STD_LOGIC;
    ifromECCRLast : in STD_LOGIC;
    ofromECCRReady : out STD_LOGIC;
    iSharedKESReady : in STD_LOGIC;
    oErrorDetectionEnd : out STD_LOGIC_VECTOR ( 1 downto 0 );
    oDecodeNeeded : out STD_LOGIC_VECTOR ( 1 downto 0 );
    oSyndromes : out STD_LOGIC_VECTOR ( 647 downto 0 );
    iIntraSharedKESEnd : in STD_LOGIC;
    iErroredChunk : in STD_LOGIC_VECTOR ( 1 downto 0 );
    iCorrectionFail : in STD_LOGIC_VECTOR ( 1 downto 0 );
    iErrorCount : in STD_LOGIC_VECTOR ( 17 downto 0 );
    iELPCoefficients : in STD_LOGIC_VECTOR ( 359 downto 0 );
    oCSAvailable : out STD_LOGIC
  );

  attribute CHECK_LICENSE_TYPE : string;
  attribute CHECK_LICENSE_TYPE of sys_top_bch_sccs_256B_21B_13b_0_0 : entity is "sys_top_bch_sccs_256B_21B_13b_0_0,BCHSCCS,{}";
  attribute DowngradeIPIdentifiedWarnings : string;
  attribute DowngradeIPIdentifiedWarnings of sys_top_bch_sccs_256B_21B_13b_0_0 : entity is "yes";
  attribute IP_DEFINITION_SOURCE : string;
  attribute IP_DEFINITION_SOURCE of sys_top_bch_sccs_256B_21B_13b_0_0 : entity is "package_project";
end sys_top_bch_sccs_256B_21B_13b_0_0;

architecture stub of sys_top_bch_sccs_256B_21B_13b_0_0 is
  attribute syn_black_box : boolean;
  attribute black_box_pad_pin : string;
  attribute syn_black_box of stub : architecture is true;
  attribute black_box_pad_pin of stub : architecture is "iClock,iReset,iToECCWOpcode[5:0],iToECCWTargetID[4:0],iToECCWSourceID[4:0],iToECCWAddress[31:0],iToECCWLength[15:0],iToECCWCmdValid,oToECCWCmdReady,iToECCWData[31:0],iToECCWValid,iToECCWLast,oToECCWReady,iToECCROpcode[5:0],iToECCRTargetID[4:0],iToECCRSourceID[4:0],iToECCRAddress[31:0],iToECCRLength[15:0],iToECCRCmdValid,oToECCRCmdReady,oToECCRData[31:0],oToECCRValid,oToECCRLast,iToECCRReady,ofromECCWOpcode[5:0],ofromECCWTargetID[4:0],ofromECCWSourceID[4:0],ofromECCWAddress[31:0],ofromECCWLength[15:0],ofromECCWCmdValid,ifromECCWCmdReady,ofromECCWData[31:0],ofromECCWValid,ofromECCWLast,ifromECCWReady,ofromECCROpcode[5:0],ofromECCRTargetID[4:0],ofromECCRSourceID[4:0],ofromECCRAddress[31:0],ofromECCRLength[15:0],ofromECCRCmdValid,ifromECCRCmdReady,ifromECCRData[31:0],ifromECCRValid,ifromECCRLast,ofromECCRReady,iSharedKESReady,oErrorDetectionEnd[1:0],oDecodeNeeded[1:0],oSyndromes[647:0],iIntraSharedKESEnd,iErroredChunk[1:0],iCorrectionFail[1:0],iErrorCount[17:0],iELPCoefficients[359:0],oCSAvailable";
  attribute X_INTERFACE_INFO : string;
  attribute X_INTERFACE_INFO of iClock : signal is "xilinx.com:signal:clock:1.0 iClock CLK";
  attribute X_INTERFACE_MODE : string;
  attribute X_INTERFACE_MODE of iClock : signal is "slave";
  attribute X_INTERFACE_PARAMETER : string;
  attribute X_INTERFACE_PARAMETER of iClock : signal is "XIL_INTERFACENAME iClock, ASSOCIATED_RESET iReset, ASSOCIATED_BUSIF to_ecc_if:from_ecc_if:bch_skes_if, FREQ_HZ 66666000, PHASE 0.0, CLK_DOMAIN /pll_bank10_clk_out1, INSERT_VIP 0";
  attribute X_INTERFACE_INFO of iReset : signal is "xilinx.com:signal:reset:1.0 iReset RST";
  attribute X_INTERFACE_MODE of iReset : signal is "slave";
  attribute X_INTERFACE_PARAMETER of iReset : signal is "XIL_INTERFACENAME iReset, POLARITY ACTIVE_HIGH, INSERT_VIP 0";
  attribute X_INTERFACE_INFO of iToECCWOpcode : signal is "enclab:user:ecc_if:1.0 to_ecc_if WOpcode";
  attribute X_INTERFACE_MODE of iToECCWOpcode : signal is "slave";
  attribute X_INTERFACE_INFO of iToECCWTargetID : signal is "enclab:user:ecc_if:1.0 to_ecc_if WTargetID";
  attribute X_INTERFACE_INFO of iToECCWSourceID : signal is "enclab:user:ecc_if:1.0 to_ecc_if WSourceID";
  attribute X_INTERFACE_INFO of iToECCWAddress : signal is "enclab:user:ecc_if:1.0 to_ecc_if WAddress";
  attribute X_INTERFACE_INFO of iToECCWLength : signal is "enclab:user:ecc_if:1.0 to_ecc_if WLength";
  attribute X_INTERFACE_INFO of iToECCWCmdValid : signal is "enclab:user:ecc_if:1.0 to_ecc_if WCmdValid";
  attribute X_INTERFACE_INFO of oToECCWCmdReady : signal is "enclab:user:ecc_if:1.0 to_ecc_if WCmdReady";
  attribute X_INTERFACE_INFO of iToECCWData : signal is "enclab:user:ecc_if:1.0 to_ecc_if WData";
  attribute X_INTERFACE_INFO of iToECCWValid : signal is "enclab:user:ecc_if:1.0 to_ecc_if WValid";
  attribute X_INTERFACE_INFO of iToECCWLast : signal is "enclab:user:ecc_if:1.0 to_ecc_if WLast";
  attribute X_INTERFACE_INFO of oToECCWReady : signal is "enclab:user:ecc_if:1.0 to_ecc_if WReady";
  attribute X_INTERFACE_INFO of iToECCROpcode : signal is "enclab:user:ecc_if:1.0 to_ecc_if ROpcode";
  attribute X_INTERFACE_INFO of iToECCRTargetID : signal is "enclab:user:ecc_if:1.0 to_ecc_if RTargetID";
  attribute X_INTERFACE_INFO of iToECCRSourceID : signal is "enclab:user:ecc_if:1.0 to_ecc_if RSourceID";
  attribute X_INTERFACE_INFO of iToECCRAddress : signal is "enclab:user:ecc_if:1.0 to_ecc_if RAddress";
  attribute X_INTERFACE_INFO of iToECCRLength : signal is "enclab:user:ecc_if:1.0 to_ecc_if RLength";
  attribute X_INTERFACE_INFO of iToECCRCmdValid : signal is "enclab:user:ecc_if:1.0 to_ecc_if RCmdValid";
  attribute X_INTERFACE_INFO of oToECCRCmdReady : signal is "enclab:user:ecc_if:1.0 to_ecc_if RCmdReady";
  attribute X_INTERFACE_INFO of oToECCRData : signal is "enclab:user:ecc_if:1.0 to_ecc_if RData";
  attribute X_INTERFACE_INFO of oToECCRValid : signal is "enclab:user:ecc_if:1.0 to_ecc_if RValid";
  attribute X_INTERFACE_INFO of oToECCRLast : signal is "enclab:user:ecc_if:1.0 to_ecc_if RLast";
  attribute X_INTERFACE_INFO of iToECCRReady : signal is "enclab:user:ecc_if:1.0 to_ecc_if RReady";
  attribute X_INTERFACE_INFO of ofromECCWOpcode : signal is "enclab:user:ecc_if:1.0 from_ecc_if WOpcode";
  attribute X_INTERFACE_MODE of ofromECCWOpcode : signal is "master";
  attribute X_INTERFACE_INFO of ofromECCWTargetID : signal is "enclab:user:ecc_if:1.0 from_ecc_if WTargetID";
  attribute X_INTERFACE_INFO of ofromECCWSourceID : signal is "enclab:user:ecc_if:1.0 from_ecc_if WSourceID";
  attribute X_INTERFACE_INFO of ofromECCWAddress : signal is "enclab:user:ecc_if:1.0 from_ecc_if WAddress";
  attribute X_INTERFACE_INFO of ofromECCWLength : signal is "enclab:user:ecc_if:1.0 from_ecc_if WLength";
  attribute X_INTERFACE_INFO of ofromECCWCmdValid : signal is "enclab:user:ecc_if:1.0 from_ecc_if WCmdValid";
  attribute X_INTERFACE_INFO of ifromECCWCmdReady : signal is "enclab:user:ecc_if:1.0 from_ecc_if WCmdReady";
  attribute X_INTERFACE_INFO of ofromECCWData : signal is "enclab:user:ecc_if:1.0 from_ecc_if WData";
  attribute X_INTERFACE_INFO of ofromECCWValid : signal is "enclab:user:ecc_if:1.0 from_ecc_if WValid";
  attribute X_INTERFACE_INFO of ofromECCWLast : signal is "enclab:user:ecc_if:1.0 from_ecc_if WLast";
  attribute X_INTERFACE_INFO of ifromECCWReady : signal is "enclab:user:ecc_if:1.0 from_ecc_if WReady";
  attribute X_INTERFACE_INFO of ofromECCROpcode : signal is "enclab:user:ecc_if:1.0 from_ecc_if ROpcode";
  attribute X_INTERFACE_INFO of ofromECCRTargetID : signal is "enclab:user:ecc_if:1.0 from_ecc_if RTargetID";
  attribute X_INTERFACE_INFO of ofromECCRSourceID : signal is "enclab:user:ecc_if:1.0 from_ecc_if RSourceID";
  attribute X_INTERFACE_INFO of ofromECCRAddress : signal is "enclab:user:ecc_if:1.0 from_ecc_if RAddress";
  attribute X_INTERFACE_INFO of ofromECCRLength : signal is "enclab:user:ecc_if:1.0 from_ecc_if RLength";
  attribute X_INTERFACE_INFO of ofromECCRCmdValid : signal is "enclab:user:ecc_if:1.0 from_ecc_if RCmdValid";
  attribute X_INTERFACE_INFO of ifromECCRCmdReady : signal is "enclab:user:ecc_if:1.0 from_ecc_if RCmdReady";
  attribute X_INTERFACE_INFO of ifromECCRData : signal is "enclab:user:ecc_if:1.0 from_ecc_if RData";
  attribute X_INTERFACE_INFO of ifromECCRValid : signal is "enclab:user:ecc_if:1.0 from_ecc_if RValid";
  attribute X_INTERFACE_INFO of ifromECCRLast : signal is "enclab:user:ecc_if:1.0 from_ecc_if RLast";
  attribute X_INTERFACE_INFO of ofromECCRReady : signal is "enclab:user:ecc_if:1.0 from_ecc_if RReady";
  attribute X_INTERFACE_INFO of iSharedKESReady : signal is "enclab:user:bch_skes_if:1.0 bch_skes_if SharedKESReady";
  attribute X_INTERFACE_MODE of iSharedKESReady : signal is "master";
  attribute X_INTERFACE_INFO of oErrorDetectionEnd : signal is "enclab:user:bch_skes_if:1.0 bch_skes_if ErrorDetectionEnd";
  attribute X_INTERFACE_INFO of oDecodeNeeded : signal is "enclab:user:bch_skes_if:1.0 bch_skes_if DecodeNeeded";
  attribute X_INTERFACE_INFO of oSyndromes : signal is "enclab:user:bch_skes_if:1.0 bch_skes_if Syndromes";
  attribute X_INTERFACE_INFO of iIntraSharedKESEnd : signal is "enclab:user:bch_skes_if:1.0 bch_skes_if IntraSharedKESEnd";
  attribute X_INTERFACE_INFO of iErroredChunk : signal is "enclab:user:bch_skes_if:1.0 bch_skes_if ErroredChunk";
  attribute X_INTERFACE_INFO of iCorrectionFail : signal is "enclab:user:bch_skes_if:1.0 bch_skes_if CorrectionFail";
  attribute X_INTERFACE_INFO of iErrorCount : signal is "enclab:user:bch_skes_if:1.0 bch_skes_if ErrorCount";
  attribute X_INTERFACE_INFO of iELPCoefficients : signal is "enclab:user:bch_skes_if:1.0 bch_skes_if ELPCoefficients";
  attribute X_INTERFACE_INFO of oCSAvailable : signal is "enclab:user:bch_skes_if:1.0 bch_skes_if CSAvailable";
  attribute X_CORE_INFO : string;
  attribute X_CORE_INFO of stub : architecture is "BCHSCCS,Vivado 2025.1";
begin
end;
