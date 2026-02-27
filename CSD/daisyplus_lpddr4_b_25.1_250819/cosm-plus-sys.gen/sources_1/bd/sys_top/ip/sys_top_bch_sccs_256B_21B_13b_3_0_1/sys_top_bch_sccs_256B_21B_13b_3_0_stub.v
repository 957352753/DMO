// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2025.1 (win64) Build 6140274 Thu May 22 00:12:29 MDT 2025
// Date        : Tue Jul  8 14:28:00 2025
// Host        : DESKTOP-JP2R5GF running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode synth_stub -rename_top sys_top_bch_sccs_256B_21B_13b_3_0 -prefix
//               sys_top_bch_sccs_256B_21B_13b_3_0_ sys_top_bch_sccs_256B_21B_13b_0_0_stub.v
// Design      : sys_top_bch_sccs_256B_21B_13b_0_0
// Purpose     : Stub declaration of top-level module interface
// Device      : xczu17eg-ffvc1760-2-e
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* CHECK_LICENSE_TYPE = "sys_top_bch_sccs_256B_21B_13b_0_0,BCHSCCS,{}" *) (* DowngradeIPIdentifiedWarnings = "yes" *) (* IP_DEFINITION_SOURCE = "package_project" *) 
(* X_CORE_INFO = "BCHSCCS,Vivado 2025.1" *) 
module sys_top_bch_sccs_256B_21B_13b_3_0(iClock, iReset, iToECCWOpcode, iToECCWTargetID, 
  iToECCWSourceID, iToECCWAddress, iToECCWLength, iToECCWCmdValid, oToECCWCmdReady, 
  iToECCWData, iToECCWValid, iToECCWLast, oToECCWReady, iToECCROpcode, iToECCRTargetID, 
  iToECCRSourceID, iToECCRAddress, iToECCRLength, iToECCRCmdValid, oToECCRCmdReady, 
  oToECCRData, oToECCRValid, oToECCRLast, iToECCRReady, ofromECCWOpcode, ofromECCWTargetID, 
  ofromECCWSourceID, ofromECCWAddress, ofromECCWLength, ofromECCWCmdValid, 
  ifromECCWCmdReady, ofromECCWData, ofromECCWValid, ofromECCWLast, ifromECCWReady, 
  ofromECCROpcode, ofromECCRTargetID, ofromECCRSourceID, ofromECCRAddress, ofromECCRLength, 
  ofromECCRCmdValid, ifromECCRCmdReady, ifromECCRData, ifromECCRValid, ifromECCRLast, 
  ofromECCRReady, iSharedKESReady, oErrorDetectionEnd, oDecodeNeeded, oSyndromes, 
  iIntraSharedKESEnd, iErroredChunk, iCorrectionFail, iErrorCount, iELPCoefficients, 
  oCSAvailable)
/* synthesis syn_black_box black_box_pad_pin="iReset,iToECCWOpcode[5:0],iToECCWTargetID[4:0],iToECCWSourceID[4:0],iToECCWAddress[31:0],iToECCWLength[15:0],iToECCWCmdValid,oToECCWCmdReady,iToECCWData[31:0],iToECCWValid,iToECCWLast,oToECCWReady,iToECCROpcode[5:0],iToECCRTargetID[4:0],iToECCRSourceID[4:0],iToECCRAddress[31:0],iToECCRLength[15:0],iToECCRCmdValid,oToECCRCmdReady,oToECCRData[31:0],oToECCRValid,oToECCRLast,iToECCRReady,ofromECCWOpcode[5:0],ofromECCWTargetID[4:0],ofromECCWSourceID[4:0],ofromECCWAddress[31:0],ofromECCWLength[15:0],ofromECCWCmdValid,ifromECCWCmdReady,ofromECCWData[31:0],ofromECCWValid,ofromECCWLast,ifromECCWReady,ofromECCROpcode[5:0],ofromECCRTargetID[4:0],ofromECCRSourceID[4:0],ofromECCRAddress[31:0],ofromECCRLength[15:0],ofromECCRCmdValid,ifromECCRCmdReady,ifromECCRData[31:0],ifromECCRValid,ifromECCRLast,ofromECCRReady,iSharedKESReady,oErrorDetectionEnd[1:0],oDecodeNeeded[1:0],oSyndromes[647:0],iIntraSharedKESEnd,iErroredChunk[1:0],iCorrectionFail[1:0],iErrorCount[17:0],iELPCoefficients[359:0],oCSAvailable" */
/* synthesis syn_force_seq_prim="iClock" */;
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 iClock CLK" *) (* X_INTERFACE_MODE = "slave" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME iClock, ASSOCIATED_RESET iReset, ASSOCIATED_BUSIF to_ecc_if:from_ecc_if:bch_skes_if, FREQ_HZ 66666000, PHASE 0.0, CLK_DOMAIN /pll_bank10_clk_out1, INSERT_VIP 0" *) input iClock /* synthesis syn_isclock = 1 */;
  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 iReset RST" *) (* X_INTERFACE_MODE = "slave" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME iReset, POLARITY ACTIVE_HIGH, INSERT_VIP 0" *) input iReset;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if WOpcode" *) (* X_INTERFACE_MODE = "slave" *) input [5:0]iToECCWOpcode;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if WTargetID" *) input [4:0]iToECCWTargetID;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if WSourceID" *) input [4:0]iToECCWSourceID;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if WAddress" *) input [31:0]iToECCWAddress;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if WLength" *) input [15:0]iToECCWLength;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if WCmdValid" *) input iToECCWCmdValid;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if WCmdReady" *) output oToECCWCmdReady;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if WData" *) input [31:0]iToECCWData;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if WValid" *) input iToECCWValid;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if WLast" *) input iToECCWLast;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if WReady" *) output oToECCWReady;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if ROpcode" *) input [5:0]iToECCROpcode;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if RTargetID" *) input [4:0]iToECCRTargetID;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if RSourceID" *) input [4:0]iToECCRSourceID;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if RAddress" *) input [31:0]iToECCRAddress;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if RLength" *) input [15:0]iToECCRLength;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if RCmdValid" *) input iToECCRCmdValid;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if RCmdReady" *) output oToECCRCmdReady;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if RData" *) output [31:0]oToECCRData;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if RValid" *) output oToECCRValid;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if RLast" *) output oToECCRLast;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 to_ecc_if RReady" *) input iToECCRReady;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if WOpcode" *) (* X_INTERFACE_MODE = "master" *) output [5:0]ofromECCWOpcode;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if WTargetID" *) output [4:0]ofromECCWTargetID;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if WSourceID" *) output [4:0]ofromECCWSourceID;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if WAddress" *) output [31:0]ofromECCWAddress;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if WLength" *) output [15:0]ofromECCWLength;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if WCmdValid" *) output ofromECCWCmdValid;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if WCmdReady" *) input ifromECCWCmdReady;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if WData" *) output [31:0]ofromECCWData;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if WValid" *) output ofromECCWValid;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if WLast" *) output ofromECCWLast;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if WReady" *) input ifromECCWReady;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if ROpcode" *) output [5:0]ofromECCROpcode;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if RTargetID" *) output [4:0]ofromECCRTargetID;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if RSourceID" *) output [4:0]ofromECCRSourceID;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if RAddress" *) output [31:0]ofromECCRAddress;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if RLength" *) output [15:0]ofromECCRLength;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if RCmdValid" *) output ofromECCRCmdValid;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if RCmdReady" *) input ifromECCRCmdReady;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if RData" *) input [31:0]ifromECCRData;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if RValid" *) input ifromECCRValid;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if RLast" *) input ifromECCRLast;
  (* X_INTERFACE_INFO = "enclab:user:ecc_if:1.0 from_ecc_if RReady" *) output ofromECCRReady;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_if SharedKESReady" *) (* X_INTERFACE_MODE = "master" *) input iSharedKESReady;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_if ErrorDetectionEnd" *) output [1:0]oErrorDetectionEnd;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_if DecodeNeeded" *) output [1:0]oDecodeNeeded;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_if Syndromes" *) output [647:0]oSyndromes;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_if IntraSharedKESEnd" *) input iIntraSharedKESEnd;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_if ErroredChunk" *) input [1:0]iErroredChunk;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_if CorrectionFail" *) input [1:0]iCorrectionFail;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_if ErrorCount" *) input [17:0]iErrorCount;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_if ELPCoefficients" *) input [359:0]iELPCoefficients;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_if CSAvailable" *) output oCSAvailable;
endmodule
