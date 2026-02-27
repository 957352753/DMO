// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2025.1 (win64) Build 6140274 Thu May 22 00:12:29 MDT 2025
// Date        : Tue Jul 15 21:35:14 2025
// Host        : DESKTOP-JP2R5GF running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode synth_stub
//               d:/sources/DaisyPlus/2025.1/daisyplus_openssd_micron_4c2w_20251_20250708/cosm-plus-sys.gen/sources_1/bd/sys_top/ip/sys_top_v2nfc_0_1/sys_top_v2nfc_0_1_stub.v
// Design      : sys_top_v2nfc_0_1
// Purpose     : Stub declaration of top-level module interface
// Device      : xczu17eg-ffvc1760-2-e
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* CHECK_LICENSE_TYPE = "sys_top_v2nfc_0_1,NFC_Toggle_Top_DDR100,{}" *) (* DowngradeIPIdentifiedWarnings = "yes" *) (* IP_DEFINITION_SOURCE = "package_project" *) 
(* X_CORE_INFO = "NFC_Toggle_Top_DDR100,Vivado 2025.1" *) 
module sys_top_v2nfc_0_1(iSystemClock, iDelayRefClock, 
  iOutputDrivingClock, iOutputStrobeClock, iReset, iOpcode, iTargetID, iSourceID, iAddress, 
  iLength, iCMDValid, oCMDReady, iWriteData, iWriteLast, iWriteValid, oWriteReady, oReadData, 
  oReadLast, oReadValid, iReadReady, oReadyBusy, IO_NAND_DQS_P, IO_NAND_DQS_N, IO_NAND_DQ, 
  O_NAND_CE, O_NAND_WE, O_NAND_RE_P, O_NAND_RE_N, O_NAND_ALE, O_NAND_CLE, I_NAND_RB, O_NAND_WP, 
  iDQSIDelayTap, iDQSIDelayTapLoad, iDQ0IDelayTap, iDQ0IDelayTapLoad, iDQ1IDelayTap, 
  iDQ1IDelayTapLoad, iDQ2IDelayTap, iDQ2IDelayTapLoad, iDQ3IDelayTap, iDQ3IDelayTapLoad, 
  iDQ4IDelayTap, iDQ4IDelayTapLoad, iDQ5IDelayTap, iDQ5IDelayTapLoad, iDQ6IDelayTap, 
  iDQ6IDelayTapLoad, iDQ7IDelayTap, iDQ7IDelayTapLoad)
/* synthesis syn_black_box black_box_pad_pin="iReset,iOpcode[5:0],iTargetID[4:0],iSourceID[4:0],iAddress[31:0],iLength[15:0],iCMDValid,oCMDReady,iWriteData[31:0],iWriteLast,iWriteValid,oWriteReady,oReadData[31:0],oReadLast,oReadValid,iReadReady,oReadyBusy[7:0],IO_NAND_DQS_P,IO_NAND_DQS_N,IO_NAND_DQ[7:0],O_NAND_CE[7:0],O_NAND_WE,O_NAND_RE_P,O_NAND_RE_N,O_NAND_ALE,O_NAND_CLE,I_NAND_RB[7:0],O_NAND_WP,iDQSIDelayTap[8:0],iDQSIDelayTapLoad[1:0],iDQ0IDelayTap[8:0],iDQ0IDelayTapLoad[1:0],iDQ1IDelayTap[8:0],iDQ1IDelayTapLoad[1:0],iDQ2IDelayTap[8:0],iDQ2IDelayTapLoad[1:0],iDQ3IDelayTap[8:0],iDQ3IDelayTapLoad[1:0],iDQ4IDelayTap[8:0],iDQ4IDelayTapLoad[1:0],iDQ5IDelayTap[8:0],iDQ5IDelayTapLoad[1:0],iDQ6IDelayTap[8:0],iDQ6IDelayTapLoad[1:0],iDQ7IDelayTap[8:0],iDQ7IDelayTapLoad[1:0]" */
/* synthesis syn_force_seq_prim="iSystemClock" */
/* synthesis syn_force_seq_prim="iDelayRefClock" */
/* synthesis syn_force_seq_prim="iOutputDrivingClock" */
/* synthesis syn_force_seq_prim="iOutputStrobeClock" */;
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 iSystemClock CLK" *) (* X_INTERFACE_MODE = "slave" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME iSystemClock, ASSOCIATED_RESET iReset, ASSOCIATED_BUSIF v2nfc_if, FREQ_HZ 66666000, FREQ_TOLERANCE_HZ 0, PHASE 0.0, CLK_DOMAIN /pll_bank11_clk_out1, INSERT_VIP 0" *) input iSystemClock /* synthesis syn_isclock = 1 */;
  input iDelayRefClock /* synthesis syn_isclock = 1 */;
  input iOutputDrivingClock /* synthesis syn_isclock = 1 */;
  input iOutputStrobeClock /* synthesis syn_isclock = 1 */;
  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 iReset RST" *) (* X_INTERFACE_MODE = "slave" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME iReset, POLARITY ACTIVE_HIGH, INSERT_VIP 0" *) input iReset;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if Opcode" *) (* X_INTERFACE_MODE = "slave" *) input [5:0]iOpcode;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if TargetID" *) input [4:0]iTargetID;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if SourceID" *) input [4:0]iSourceID;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if Address" *) input [31:0]iAddress;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if Length" *) input [15:0]iLength;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if CMDValid" *) input iCMDValid;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if CMDReady" *) output oCMDReady;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if WriteData" *) input [31:0]iWriteData;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if WriteLast" *) input iWriteLast;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if WriteValid" *) input iWriteValid;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if WriteReady" *) output oWriteReady;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if ReadData" *) output [31:0]oReadData;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if ReadLast" *) output oReadLast;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if ReadValid" *) output oReadValid;
  (* X_INTERFACE_INFO = "enclab:user:v2nfc_if:1.0 v2nfc_if ReadReady" *) input iReadReady;
  output [7:0]oReadyBusy;
  (* X_INTERFACE_INFO = "enclab:user:nand_if:1.0 nand_if NAND_DQS_P" *) (* X_INTERFACE_MODE = "master" *) inout IO_NAND_DQS_P;
  (* X_INTERFACE_INFO = "enclab:user:nand_if:1.0 nand_if NAND_DQS_N" *) inout IO_NAND_DQS_N;
  (* X_INTERFACE_INFO = "enclab:user:nand_if:1.0 nand_if NAND_DQ" *) inout [7:0]IO_NAND_DQ;
  (* X_INTERFACE_INFO = "enclab:user:nand_if:1.0 nand_if NAND_CE" *) output [7:0]O_NAND_CE;
  (* X_INTERFACE_INFO = "enclab:user:nand_if:1.0 nand_if NAND_WE" *) output O_NAND_WE;
  (* X_INTERFACE_INFO = "enclab:user:nand_if:1.0 nand_if NAND_RE_P" *) output O_NAND_RE_P;
  (* X_INTERFACE_INFO = "enclab:user:nand_if:1.0 nand_if NAND_RE_N" *) output O_NAND_RE_N;
  (* X_INTERFACE_INFO = "enclab:user:nand_if:1.0 nand_if NAND_ALE" *) output O_NAND_ALE;
  (* X_INTERFACE_INFO = "enclab:user:nand_if:1.0 nand_if NAND_CLE" *) output O_NAND_CLE;
  (* X_INTERFACE_INFO = "enclab:user:nand_if:1.0 nand_if NAND_RB" *) input [7:0]I_NAND_RB;
  (* X_INTERFACE_INFO = "enclab:user:nand_if:1.0 nand_if NAND_WP" *) output O_NAND_WP;
  input [8:0]iDQSIDelayTap;
  input [1:0]iDQSIDelayTapLoad;
  input [8:0]iDQ0IDelayTap;
  input [1:0]iDQ0IDelayTapLoad;
  input [8:0]iDQ1IDelayTap;
  input [1:0]iDQ1IDelayTapLoad;
  input [8:0]iDQ2IDelayTap;
  input [1:0]iDQ2IDelayTapLoad;
  input [8:0]iDQ3IDelayTap;
  input [1:0]iDQ3IDelayTapLoad;
  input [8:0]iDQ4IDelayTap;
  input [1:0]iDQ4IDelayTapLoad;
  input [8:0]iDQ5IDelayTap;
  input [1:0]iDQ5IDelayTapLoad;
  input [8:0]iDQ6IDelayTap;
  input [1:0]iDQ6IDelayTapLoad;
  input [8:0]iDQ7IDelayTap;
  input [1:0]iDQ7IDelayTapLoad;
endmodule
