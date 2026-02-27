// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2025.1 (win64) Build 6140274 Thu May 22 00:12:29 MDT 2025
// Date        : Tue Jul  8 14:28:46 2025
// Host        : DESKTOP-JP2R5GF running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode synth_stub -rename_top decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix -prefix
//               decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_ sys_top_bch_skes_256B_21B_13b_0_0_stub.v
// Design      : sys_top_bch_skes_256B_21B_13b_0_0
// Purpose     : Stub declaration of top-level module interface
// Device      : xczu17eg-ffvc1760-2-e
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* CHECK_LICENSE_TYPE = "sys_top_bch_skes_256B_21B_13b_0_0,SharedKESTop,{}" *) (* DowngradeIPIdentifiedWarnings = "yes" *) (* X_CORE_INFO = "SharedKESTop,Vivado 2025.1" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix(iClock, iReset, oSharedKESReady_0, 
  iErrorDetectionEnd_0, iDecodeNeeded_0, iSyndromes_0, iCSAvailable_0, 
  oIntraSharedKESEnd_0, oErroredChunk_0, oCorrectionFail_0, oClusterErrorCount_0, 
  oELPCoefficients_0, oSharedKESReady_1, iErrorDetectionEnd_1, iDecodeNeeded_1, 
  iSyndromes_1, iCSAvailable_1, oIntraSharedKESEnd_1, oErroredChunk_1, oCorrectionFail_1, 
  oClusterErrorCount_1, oELPCoefficients_1, oSharedKESReady_2, iErrorDetectionEnd_2, 
  iDecodeNeeded_2, iSyndromes_2, iCSAvailable_2, oIntraSharedKESEnd_2, oErroredChunk_2, 
  oCorrectionFail_2, oClusterErrorCount_2, oELPCoefficients_2, oSharedKESReady_3, 
  iErrorDetectionEnd_3, iDecodeNeeded_3, iSyndromes_3, iCSAvailable_3, 
  oIntraSharedKESEnd_3, oErroredChunk_3, oCorrectionFail_3, oClusterErrorCount_3, 
  oELPCoefficients_3)
/* synthesis syn_black_box black_box_pad_pin="iReset,oSharedKESReady_0,iErrorDetectionEnd_0[1:0],iDecodeNeeded_0[1:0],iSyndromes_0[647:0],iCSAvailable_0,oIntraSharedKESEnd_0,oErroredChunk_0[1:0],oCorrectionFail_0[1:0],oClusterErrorCount_0[17:0],oELPCoefficients_0[359:0],oSharedKESReady_1,iErrorDetectionEnd_1[1:0],iDecodeNeeded_1[1:0],iSyndromes_1[647:0],iCSAvailable_1,oIntraSharedKESEnd_1,oErroredChunk_1[1:0],oCorrectionFail_1[1:0],oClusterErrorCount_1[17:0],oELPCoefficients_1[359:0],oSharedKESReady_2,iErrorDetectionEnd_2[1:0],iDecodeNeeded_2[1:0],iSyndromes_2[647:0],iCSAvailable_2,oIntraSharedKESEnd_2,oErroredChunk_2[1:0],oCorrectionFail_2[1:0],oClusterErrorCount_2[17:0],oELPCoefficients_2[359:0],oSharedKESReady_3,iErrorDetectionEnd_3[1:0],iDecodeNeeded_3[1:0],iSyndromes_3[647:0],iCSAvailable_3,oIntraSharedKESEnd_3,oErroredChunk_3[1:0],oCorrectionFail_3[1:0],oClusterErrorCount_3[17:0],oELPCoefficients_3[359:0]" */
/* synthesis syn_force_seq_prim="iClock" */;
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 signal_clock CLK" *) (* X_INTERFACE_MODE = "slave" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME signal_clock, ASSOCIATED_RESET iReset, ASSOCIATED_BUSIF bch_skes_ch0_if:bch_skes_ch1_if:bch_skes_ch2_if:bch_skes_ch3_if, FREQ_HZ 66666000, PHASE 0.0, CLK_DOMAIN sys_top_zynq_ultra_ps_e_0_0_pl_clk0, INSERT_VIP 0" *) input iClock /* synthesis syn_isclock = 1 */;
  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 signal_reset RST" *) (* X_INTERFACE_MODE = "slave" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME signal_reset, POLARITY ACTIVE_HIGH, INSERT_VIP 0" *) input iReset;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch0_if SharedKESReady" *) (* X_INTERFACE_MODE = "slave" *) output oSharedKESReady_0;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch0_if ErrorDetectionEnd" *) input [1:0]iErrorDetectionEnd_0;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch0_if DecodeNeeded" *) input [1:0]iDecodeNeeded_0;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch0_if Syndromes" *) input [647:0]iSyndromes_0;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch0_if CSAvailable" *) input iCSAvailable_0;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch0_if IntraSharedKESEnd" *) output oIntraSharedKESEnd_0;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch0_if ErroredChunk" *) output [1:0]oErroredChunk_0;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch0_if CorrectionFail" *) output [1:0]oCorrectionFail_0;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch0_if ErrorCount" *) output [17:0]oClusterErrorCount_0;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch0_if ELPCoefficients" *) output [359:0]oELPCoefficients_0;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch1_if SharedKESReady" *) (* X_INTERFACE_MODE = "slave" *) output oSharedKESReady_1;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch1_if ErrorDetectionEnd" *) input [1:0]iErrorDetectionEnd_1;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch1_if DecodeNeeded" *) input [1:0]iDecodeNeeded_1;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch1_if Syndromes" *) input [647:0]iSyndromes_1;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch1_if CSAvailable" *) input iCSAvailable_1;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch1_if IntraSharedKESEnd" *) output oIntraSharedKESEnd_1;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch1_if ErroredChunk" *) output [1:0]oErroredChunk_1;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch1_if CorrectionFail" *) output [1:0]oCorrectionFail_1;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch1_if ErrorCount" *) output [17:0]oClusterErrorCount_1;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch1_if ELPCoefficients" *) output [359:0]oELPCoefficients_1;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch2_if SharedKESReady" *) (* X_INTERFACE_MODE = "slave" *) output oSharedKESReady_2;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch2_if ErrorDetectionEnd" *) input [1:0]iErrorDetectionEnd_2;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch2_if DecodeNeeded" *) input [1:0]iDecodeNeeded_2;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch2_if Syndromes" *) input [647:0]iSyndromes_2;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch2_if CSAvailable" *) input iCSAvailable_2;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch2_if IntraSharedKESEnd" *) output oIntraSharedKESEnd_2;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch2_if ErroredChunk" *) output [1:0]oErroredChunk_2;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch2_if CorrectionFail" *) output [1:0]oCorrectionFail_2;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch2_if ErrorCount" *) output [17:0]oClusterErrorCount_2;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch2_if ELPCoefficients" *) output [359:0]oELPCoefficients_2;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch3_if SharedKESReady" *) (* X_INTERFACE_MODE = "slave" *) output oSharedKESReady_3;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch3_if ErrorDetectionEnd" *) input [1:0]iErrorDetectionEnd_3;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch3_if DecodeNeeded" *) input [1:0]iDecodeNeeded_3;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch3_if Syndromes" *) input [647:0]iSyndromes_3;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch3_if CSAvailable" *) input iCSAvailable_3;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch3_if IntraSharedKESEnd" *) output oIntraSharedKESEnd_3;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch3_if ErroredChunk" *) output [1:0]oErroredChunk_3;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch3_if CorrectionFail" *) output [1:0]oCorrectionFail_3;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch3_if ErrorCount" *) output [17:0]oClusterErrorCount_3;
  (* X_INTERFACE_INFO = "enclab:user:bch_skes_if:1.0 bch_skes_ch3_if ELPCoefficients" *) output [359:0]oELPCoefficients_3;
endmodule
