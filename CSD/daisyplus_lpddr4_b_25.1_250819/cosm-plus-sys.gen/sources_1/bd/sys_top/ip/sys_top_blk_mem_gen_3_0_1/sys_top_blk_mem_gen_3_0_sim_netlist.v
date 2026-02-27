// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2025.1 (win64) Build 6140274 Thu May 22 00:12:29 MDT 2025
// Date        : Tue Jul  8 14:20:03 2025
// Host        : DESKTOP-JP2R5GF running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode funcsim -rename_top sys_top_blk_mem_gen_3_0 -prefix
//               sys_top_blk_mem_gen_3_0_ sys_top_blk_mem_gen_0_0_sim_netlist.v
// Design      : sys_top_blk_mem_gen_0_0
// Purpose     : This verilog netlist is a functional simulation representation of the design and should not be modified
//               or synthesized. This netlist cannot be used for SDF annotated simulation.
// Device      : xczu17eg-ffvc1760-2-e
// --------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

(* CHECK_LICENSE_TYPE = "sys_top_blk_mem_gen_0_0,blk_mem_gen_v8_4_11,{}" *) (* downgradeipidentifiedwarnings = "yes" *) (* x_core_info = "blk_mem_gen_v8_4_11,Vivado 2025.1" *) 
(* NotValidForBitStream *)
module sys_top_blk_mem_gen_3_0
   (clka,
    rsta,
    ena,
    wea,
    addra,
    dina,
    douta,
    clkb,
    rstb,
    enb,
    web,
    addrb,
    dinb,
    doutb);
  (* x_interface_info = "xilinx.com:interface:bram:1.0 BRAM_PORTA CLK" *) (* x_interface_mode = "slave BRAM_PORTA" *) (* x_interface_parameter = "XIL_INTERFACENAME BRAM_PORTA, MEM_ADDRESS_MODE BYTE_ADDRESS, MEM_SIZE 8192, MEM_WIDTH 32, MEM_ECC NONE, MASTER_TYPE OTHER, READ_WRITE_MODE READ_WRITE, READ_LATENCY 1" *) input clka;
  (* x_interface_info = "xilinx.com:interface:bram:1.0 BRAM_PORTA RST" *) input rsta;
  (* x_interface_info = "xilinx.com:interface:bram:1.0 BRAM_PORTA EN" *) input ena;
  (* x_interface_info = "xilinx.com:interface:bram:1.0 BRAM_PORTA WE" *) input [3:0]wea;
  (* x_interface_info = "xilinx.com:interface:bram:1.0 BRAM_PORTA ADDR" *) input [31:0]addra;
  (* x_interface_info = "xilinx.com:interface:bram:1.0 BRAM_PORTA DIN" *) input [31:0]dina;
  (* x_interface_info = "xilinx.com:interface:bram:1.0 BRAM_PORTA DOUT" *) output [31:0]douta;
  (* x_interface_info = "xilinx.com:interface:bram:1.0 BRAM_PORTB CLK" *) (* x_interface_mode = "slave BRAM_PORTB" *) (* x_interface_parameter = "XIL_INTERFACENAME BRAM_PORTB, MEM_ADDRESS_MODE BYTE_ADDRESS, MEM_SIZE 4096, MEM_WIDTH 32, MEM_ECC NONE, MASTER_TYPE OTHER, READ_WRITE_MODE READ_WRITE, READ_LATENCY 1" *) input clkb;
  (* x_interface_info = "xilinx.com:interface:bram:1.0 BRAM_PORTB RST" *) input rstb;
  (* x_interface_info = "xilinx.com:interface:bram:1.0 BRAM_PORTB EN" *) input enb;
  (* x_interface_info = "xilinx.com:interface:bram:1.0 BRAM_PORTB WE" *) input [3:0]web;
  (* x_interface_info = "xilinx.com:interface:bram:1.0 BRAM_PORTB ADDR" *) input [31:0]addrb;
  (* x_interface_info = "xilinx.com:interface:bram:1.0 BRAM_PORTB DIN" *) input [31:0]dinb;
  (* x_interface_info = "xilinx.com:interface:bram:1.0 BRAM_PORTB DOUT" *) output [31:0]doutb;

  wire [31:0]addra;
  wire [31:0]addrb;
  wire clka;
  wire [31:0]dina;
  wire [31:0]dinb;
  wire [31:0]douta;
  wire [31:0]doutb;
  wire ena;
  wire enb;
  wire rsta;
  wire rstb;
  wire [3:0]wea;
  wire [3:0]web;
  wire NLW_U0_dbiterr_UNCONNECTED;
  wire NLW_U0_rsta_busy_UNCONNECTED;
  wire NLW_U0_rstb_busy_UNCONNECTED;
  wire NLW_U0_s_axi_arready_UNCONNECTED;
  wire NLW_U0_s_axi_awready_UNCONNECTED;
  wire NLW_U0_s_axi_bvalid_UNCONNECTED;
  wire NLW_U0_s_axi_dbiterr_UNCONNECTED;
  wire NLW_U0_s_axi_rlast_UNCONNECTED;
  wire NLW_U0_s_axi_rvalid_UNCONNECTED;
  wire NLW_U0_s_axi_sbiterr_UNCONNECTED;
  wire NLW_U0_s_axi_wready_UNCONNECTED;
  wire NLW_U0_sbiterr_UNCONNECTED;
  wire [31:0]NLW_U0_rdaddrecc_UNCONNECTED;
  wire [3:0]NLW_U0_s_axi_bid_UNCONNECTED;
  wire [1:0]NLW_U0_s_axi_bresp_UNCONNECTED;
  wire [31:0]NLW_U0_s_axi_rdaddrecc_UNCONNECTED;
  wire [31:0]NLW_U0_s_axi_rdata_UNCONNECTED;
  wire [3:0]NLW_U0_s_axi_rid_UNCONNECTED;
  wire [1:0]NLW_U0_s_axi_rresp_UNCONNECTED;

  (* C_ADDRA_WIDTH = "32" *) 
  (* C_ADDRB_WIDTH = "32" *) 
  (* C_ALGORITHM = "1" *) 
  (* C_AXI_ID_WIDTH = "4" *) 
  (* C_AXI_SLAVE_TYPE = "0" *) 
  (* C_AXI_TYPE = "1" *) 
  (* C_BYTE_SIZE = "8" *) 
  (* C_COMMON_CLK = "1" *) 
  (* C_COUNT_18K_BRAM = "0" *) 
  (* C_COUNT_36K_BRAM = "2" *) 
  (* C_CTRL_ECC_ALGO = "NONE" *) 
  (* C_DEFAULT_DATA = "0" *) 
  (* C_DISABLE_WARN_BHV_COLL = "0" *) 
  (* C_DISABLE_WARN_BHV_RANGE = "0" *) 
  (* C_ELABORATION_DIR = "./" *) 
  (* C_ENABLE_32BIT_ADDRESS = "1" *) 
  (* C_EN_DEEPSLEEP_PIN = "0" *) 
  (* C_EN_ECC_PIPE = "0" *) 
  (* C_EN_RDADDRA_CHG = "0" *) 
  (* C_EN_RDADDRB_CHG = "0" *) 
  (* C_EN_SAFETY_CKT = "0" *) 
  (* C_EN_SHUTDOWN_PIN = "0" *) 
  (* C_EN_SLEEP_PIN = "0" *) 
  (* C_EST_POWER_SUMMARY = "Estimated Power for IP     :     7.734465 mW" *) 
  (* C_FAMILY = "zynquplus" *) 
  (* C_HAS_AXI_ID = "0" *) 
  (* C_HAS_ENA = "1" *) 
  (* C_HAS_ENB = "1" *) 
  (* C_HAS_INJECTERR = "0" *) 
  (* C_HAS_MEM_OUTPUT_REGS_A = "0" *) 
  (* C_HAS_MEM_OUTPUT_REGS_B = "0" *) 
  (* C_HAS_MUX_OUTPUT_REGS_A = "0" *) 
  (* C_HAS_MUX_OUTPUT_REGS_B = "0" *) 
  (* C_HAS_REGCEA = "0" *) 
  (* C_HAS_REGCEB = "0" *) 
  (* C_HAS_RSTA = "1" *) 
  (* C_HAS_RSTB = "1" *) 
  (* C_HAS_SOFTECC_INPUT_REGS_A = "0" *) 
  (* C_HAS_SOFTECC_OUTPUT_REGS_B = "0" *) 
  (* C_INITA_VAL = "0" *) 
  (* C_INITB_VAL = "0" *) 
  (* C_INIT_FILE = "NONE" *) 
  (* C_INIT_FILE_NAME = "no_coe_file_loaded" *) 
  (* C_INTERFACE_TYPE = "0" *) 
  (* C_LOAD_INIT_FILE = "0" *) 
  (* C_MEM_TYPE = "2" *) 
  (* C_MUX_PIPELINE_STAGES = "0" *) 
  (* C_PRIM_TYPE = "1" *) 
  (* C_READ_DEPTH_A = "2048" *) 
  (* C_READ_DEPTH_B = "2048" *) 
  (* C_READ_LATENCY_A = "1" *) 
  (* C_READ_LATENCY_B = "1" *) 
  (* C_READ_WIDTH_A = "32" *) 
  (* C_READ_WIDTH_B = "32" *) 
  (* C_RSTRAM_A = "0" *) 
  (* C_RSTRAM_B = "0" *) 
  (* C_RST_PRIORITY_A = "CE" *) 
  (* C_RST_PRIORITY_B = "CE" *) 
  (* C_SIM_COLLISION_CHECK = "ALL" *) 
  (* C_USE_BRAM_BLOCK = "0" *) 
  (* C_USE_BYTE_WEA = "1" *) 
  (* C_USE_BYTE_WEB = "1" *) 
  (* C_USE_DEFAULT_DATA = "0" *) 
  (* C_USE_ECC = "0" *) 
  (* C_USE_SOFTECC = "0" *) 
  (* C_USE_URAM = "0" *) 
  (* C_WEA_WIDTH = "4" *) 
  (* C_WEB_WIDTH = "4" *) 
  (* C_WRITE_DEPTH_A = "2048" *) 
  (* C_WRITE_DEPTH_B = "2048" *) 
  (* C_WRITE_MODE_A = "WRITE_FIRST" *) 
  (* C_WRITE_MODE_B = "WRITE_FIRST" *) 
  (* C_WRITE_WIDTH_A = "32" *) 
  (* C_WRITE_WIDTH_B = "32" *) 
  (* C_XDEVICEFAMILY = "zynquplus" *) 
  (* downgradeipidentifiedwarnings = "yes" *) 
  (* is_du_within_envelope = "true" *) 
  sys_top_blk_mem_gen_3_0_blk_mem_gen_v8_4_11 U0
       (.addra({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,addra[12:2],1'b0,1'b0}),
        .addrb({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,addrb[12:2],1'b0,1'b0}),
        .clka(clka),
        .clkb(1'b0),
        .dbiterr(NLW_U0_dbiterr_UNCONNECTED),
        .deepsleep(1'b0),
        .dina(dina),
        .dinb(dinb),
        .douta(douta),
        .doutb(doutb),
        .eccpipece(1'b0),
        .ena(ena),
        .enb(enb),
        .injectdbiterr(1'b0),
        .injectsbiterr(1'b0),
        .rdaddrecc(NLW_U0_rdaddrecc_UNCONNECTED[31:0]),
        .regcea(1'b1),
        .regceb(1'b1),
        .rsta(rsta),
        .rsta_busy(NLW_U0_rsta_busy_UNCONNECTED),
        .rstb(rstb),
        .rstb_busy(NLW_U0_rstb_busy_UNCONNECTED),
        .s_aclk(1'b0),
        .s_aresetn(1'b0),
        .s_axi_araddr({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .s_axi_arburst({1'b0,1'b0}),
        .s_axi_arid({1'b0,1'b0,1'b0,1'b0}),
        .s_axi_arlen({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .s_axi_arready(NLW_U0_s_axi_arready_UNCONNECTED),
        .s_axi_arsize({1'b0,1'b0,1'b0}),
        .s_axi_arvalid(1'b0),
        .s_axi_awaddr({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .s_axi_awburst({1'b0,1'b0}),
        .s_axi_awid({1'b0,1'b0,1'b0,1'b0}),
        .s_axi_awlen({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .s_axi_awready(NLW_U0_s_axi_awready_UNCONNECTED),
        .s_axi_awsize({1'b0,1'b0,1'b0}),
        .s_axi_awvalid(1'b0),
        .s_axi_bid(NLW_U0_s_axi_bid_UNCONNECTED[3:0]),
        .s_axi_bready(1'b0),
        .s_axi_bresp(NLW_U0_s_axi_bresp_UNCONNECTED[1:0]),
        .s_axi_bvalid(NLW_U0_s_axi_bvalid_UNCONNECTED),
        .s_axi_dbiterr(NLW_U0_s_axi_dbiterr_UNCONNECTED),
        .s_axi_injectdbiterr(1'b0),
        .s_axi_injectsbiterr(1'b0),
        .s_axi_rdaddrecc(NLW_U0_s_axi_rdaddrecc_UNCONNECTED[31:0]),
        .s_axi_rdata(NLW_U0_s_axi_rdata_UNCONNECTED[31:0]),
        .s_axi_rid(NLW_U0_s_axi_rid_UNCONNECTED[3:0]),
        .s_axi_rlast(NLW_U0_s_axi_rlast_UNCONNECTED),
        .s_axi_rready(1'b0),
        .s_axi_rresp(NLW_U0_s_axi_rresp_UNCONNECTED[1:0]),
        .s_axi_rvalid(NLW_U0_s_axi_rvalid_UNCONNECTED),
        .s_axi_sbiterr(NLW_U0_s_axi_sbiterr_UNCONNECTED),
        .s_axi_wdata({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .s_axi_wlast(1'b0),
        .s_axi_wready(NLW_U0_s_axi_wready_UNCONNECTED),
        .s_axi_wstrb({1'b0,1'b0,1'b0,1'b0}),
        .s_axi_wvalid(1'b0),
        .sbiterr(NLW_U0_sbiterr_UNCONNECTED),
        .shutdown(1'b0),
        .sleep(1'b0),
        .wea(wea),
        .web(web));
endmodule
`pragma protect begin_protected
`pragma protect version = 1
`pragma protect encrypt_agent = "XILINX"
`pragma protect encrypt_agent_info = "Xilinx Encryption Tool 2025.1"
`pragma protect key_keyowner="Synopsys", key_keyname="SNPS-VCS-RSA-2", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=128)
`pragma protect key_block
gydSV72FvW4hnoyUt6yZFJHfJqjRQWPUfYIuDKP0fpjrPOkLRbJGBr4Z9msYTvoIHRlYtXJ2YMY0
d1TIQb+FK4gKsTRru9wr397OxuFBsTRf4e+ZjpYZEdsnqYWcgMSzhN4yhPvO06GyZO15y/LKBxa8
3OKwxVlOLYXhv+sxdXg=

`pragma protect key_keyowner="Aldec", key_keyname="ALDEC15_001", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
WHB6Zbfa5Qi47krP9T4L8UnPOlr881dWx7UcYaZfNGIQQM0gadcoXbhucIpRaUuyOKxv6yhKveRN
h0l+N9+KX6rbZ6+TRhP9JAMuPhlpI7T42QtRv5zx9+m3ct5S0NMszbFaK8zeTAYra5BGP7BHmtkr
MpKfLK5sFyaTE/A7ACtAace9MwFTHDZdl9uUs4aY6KJlm6GaypKduiqkNugukJp5vlFPX/ZapJqG
KMtMhI6grhcuYb1FJrwRZ4jW7hs9HxddSdGLzsZ0HsBcO/qaCPTst+ZA0YIQfd5ULlFmPqq39FfO
p1P+2hEH2n+LycbMj5cn4Dxfqv2R8eucM78R3w==

`pragma protect key_keyowner="Mentor Graphics Corporation", key_keyname="MGC-VELOCE-RSA", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=128)
`pragma protect key_block
SmAzQA1VEuJXtJi5vXa2Jg7YvRqAJs6PX9HTZ1YqrJw4VfonBW3726gJ81BjlizpMkcf/Uk5sFIK
aPedVhEs4xCIZylz7gXYDshtytOA/pXUID2qV9nXr8qfI+FydSADUF3ScYDZmlkclFqlZrGq6DQ7
da3lJAzt2h/iR+cczrA=

`pragma protect key_keyowner="Mentor Graphics Corporation", key_keyname="MGC-VERIF-SIM-RSA-2", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
iAph5JWb/chMQpLPX1UoLjQDxN5l2I8McM/k2xN5wRht7HXoE6F5yV8luDjn3zkI6vnfUYo7BaI1
mogRRx+R3XcwxvhHr+lngh4+/YLVex1TFncl+kiUMAsu3M/FjFSiqGMVMdKTNLDqr35DuZJVyuiF
lTwXob/KkbQDJiJjBEoxbt+968rKRKRyJGcqIjm4mqRBdqMcgo3HOJFG74SFsWAQrxvXfBhdLSG3
OfoLfls9XDojBjp7G83k0h82g1eeWgBfydm/OcX9o48Pst93NvI4ua8WShZL8MCvRWYqWZrrjrWi
cfUjXAF5SDACjq1/OU6arz/Idz6/a7AP/jmexw==

`pragma protect key_keyowner="Real Intent", key_keyname="RI-RSA-KEY-1", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
BY49GZBxBT/gjZDPyaSWlti/sctckoR7jK6NuWdhnF9tiyNfVU7BqjjwxSnyMi0Uucv1BKHXC18h
8hQbFWnNtrq71ilURotXux7sssHlVJ2i1CsJWU18DOcBWxm2ai89uwvxDJh3TJkBJixB5KPvsDhL
lWOjTvZWPoR+Ixy+Tzo+U5Vx7z7SOakRwTrn3u7+c3vmCEBphE+HKeJExhBAoOEd0SXK5iwXaByW
D7Wb7zq6NNUmnCyaJ2BG9kGxLVsf+md7SlocuaFsYyaRZhwPyTucxIlz1tLYwcytKzx0ovoax3no
nYgzlzP/F0/PDWk9BqXgr/tuclc4EZYX0cf4ng==

`pragma protect key_keyowner="Xilinx", key_keyname="xilinxt_2025.1-2029.x", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
qGnCvL35qO7cbUEKCL50yDv1UvezcqBz601zctKop1954QlcjemzZWZHg1zJ00nJaToNdH2S8AKX
n8hNJvbQ+x5HEGL5DoSU9m5qjXd8xxocnZ0yzuZX/dGCT8kDn3gWJR2Gz13pT+w2LQUno1fX+MsC
ehgwvjBBT6GeYjdxHi+aybQUP9AblSxX/z3vh857SGCPohEWvghOgORCHAe45YD+ZWnL62FLxMM2
c+Ozq/Au/Q4q1Yzlzcfv8Mnsvg7OqOeEamQHbuYOfdkJUuYqOwsskEWW348u7FXtsf8m7P3pZyyz
IWyTDAW4igGguMPLHfbtK/twZx8ScJQmOKzglg==

`pragma protect key_keyowner="Metrics Technologies Inc.", key_keyname="DSim", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
Hz+6K8+wh5/fukU4ZWNDXGsq6hreSVCSPP67nA6kUz9Vpjy4TtTnOrrl1BWY0ivEC7Ldyw8VI60A
VO/WPlt409LdAZdMZGsEZ1JuTZ0m9LPcgu9CPCyoMECctmd8LHE+otY6etTmYABB9syY61rk2hrv
RgbcyT/HCK9TzWxSm+XMqvx2nvagCLkMDPh/JZv51fj2zcKaBPnxsz8rnDipaeo0fEyVRC3Y1F/V
U3RmXojBjIumPHSJkQ537dENJEIA0Ra65u8EM/+ItUn1bcryLcIbKy1xGadrHmHdHRUoRcAodO2C
B48bNVeL0VnGg8P9ACIB04lMNzn5p6A1tPOb4Q==

`pragma protect key_keyowner="Atrenta", key_keyname="ATR-SG-RSA-1", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=384)
`pragma protect key_block
YDpb+UeT0rJ543Q8wCo2xSS3gpVAT+JoStgBlV5IMjJoUOWkiOPn691FGChmDi3BTq5NxC73KHHR
1galACCjeTGq6cv+0Zc2Ocm1oobdrnSPHp7TMDr5Zle8FX6WywJCiGdoWBODggZSlbOASIK/PVfY
cZM2z60M6RSvzsi3TnYHiKYHpju8THVoSgRd6r31GcbiSy9TjjARERXan0OVc79jGuAg90mmDEEq
91eqmn6NZ9yLI2fgBjFUZbtFCpmJ8WGxOL1h39niWnRK3ZXnk8jcpnZUlxLbYTPO0Z3vVr1zrvcn
RVQloU0OLqg7M95zSs7NtX5Vzvb6jGbMehWV+WMMyxWmxL2XOwsAwPSeX2dI2r77pioY7X6VzH7f
/JxMAnq9udra3WGPsUkD1G0CvPkCC3zdxjpVaflY37ztX9UONhKtzMQa8lJc1IL8GhXRY3R9Lg2c
HIeXSGkpNNuFDqKT6Khe/6Casq+SjFJq+IH9IUtz6RUZTkbFb0Xhgm2P

`pragma protect key_keyowner="Cadence Design Systems.", key_keyname="CDS_RSA_KEY_VER_1", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
Q+63zFEYw/LeMgxa7g8g79GGvSyIKDKD8RvvC4DHDQuGObf6n9OGZX4e17v/E/+EDEwUhsWQHFDI
Lp/aH+6fNRmhu9BEWVjxq2WRrQSl4eQjfIaSOXu2dlYh3JjRJwiUp4LteVh8RFAf5t5sRQO4dRIK
x+h28yliSgibaWEAv5FaJQ1EFbNwmgedAaSYjgf2A3afBUcBh5Uy9VHbW/zRzdhhJdsVNBjZYcFy
CVLOcf1toCRp8J4U5FlnFMOzFegUbdXFQhq2VmIhPRxWjrfTk6iR4BcMEN9UMij/5IHRAeBdksyD
CqEKsyFxosbI5KVMRZ1Ln75Zipn0JdsGekHkxg==

`pragma protect key_keyowner="Synplicity", key_keyname="SYNP15_1", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
DPUa5DLPYRWvbPnX0U412yoWvvvHyuq43DrYmDJGTK0cR5U4U6th8icYgizC1/hUAEzt19kM/hVa
zZh7bXSWACYLpcfhPY8dRTVGDZVjpbkraw0ceBryLP7jc6Jt5JdNw88tZtZpprCB7nQ25lUL82Hf
WTwL1ZqgGIvtfHhxO0JF5L5ES5giedwQ6u5ffXG3UB6ELcpQD1NvpW5lAz4mfXyvVDCAPZN581TF
tlAy79iKbPKlJ2zFn1BS2cuRIHHe2JRxwPo+0n5VD5CXVgg+lCYxTnCxI8CdyFaTumbs4IfAKwVI
wSN/btbwDUhW9hAHWHIRo+BpdJ4qeGcTDPKtsA==

`pragma protect key_keyowner="Mentor Graphics Corporation", key_keyname="MGC-PREC-RSA", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
mf5hcf6JE6yLm0jNCQnHMVmogjLlPz6re0FwG67yvOJ3FuEorru0emIeAKEwgOoxjUYNWvcM7QAH
/UEeB2EIdjLl6glPAUda0HjtaCU2rdncVdM8k6DSMBggc4yo18Qx5F+1TD/RoBgoo0jNkMdDy6wJ
JHjqlN+R01z3yYIMQ9f2z6ZaYncbBYEp4+YAb7g1D7CSMxP5cFRpQznRpYp0JwqJfT9CHzlKgdab
8B288NxeLM66iYodiTS+GSRGLGtDWXpz9yeiuiPe6kJxae2GJyHIMSfluO/0Slc3m24DQNdbojf8
jdc0G2UnrDe5mCUTfYiDmpOWTUJOdYo0FK0N2g==

`pragma protect data_method = "AES128-CBC"
`pragma protect encoding = (enctype = "BASE64", line_length = 76, bytes = 54784)
`pragma protect data_block
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
83bJaRMJhg==
`pragma protect end_protected
`ifndef GLBL
`define GLBL
`timescale  1 ps / 1 ps

module glbl ();

    parameter ROC_WIDTH = 100000;
    parameter TOC_WIDTH = 0;
    parameter GRES_WIDTH = 10000;
    parameter GRES_START = 10000;

//--------   STARTUP Globals --------------
    wire GSR;
    wire GTS;
    wire GWE;
    wire PRLD;
    wire GRESTORE;
    tri1 p_up_tmp;
    tri (weak1, strong0) PLL_LOCKG = p_up_tmp;

    wire PROGB_GLBL;
    wire CCLKO_GLBL;
    wire FCSBO_GLBL;
    wire [3:0] DO_GLBL;
    wire [3:0] DI_GLBL;
   
    reg GSR_int;
    reg GTS_int;
    reg PRLD_int;
    reg GRESTORE_int;

//--------   JTAG Globals --------------
    wire JTAG_TDO_GLBL;
    wire JTAG_TCK_GLBL;
    wire JTAG_TDI_GLBL;
    wire JTAG_TMS_GLBL;
    wire JTAG_TRST_GLBL;

    reg JTAG_CAPTURE_GLBL;
    reg JTAG_RESET_GLBL;
    reg JTAG_SHIFT_GLBL;
    reg JTAG_UPDATE_GLBL;
    reg JTAG_RUNTEST_GLBL;

    reg JTAG_SEL1_GLBL = 0;
    reg JTAG_SEL2_GLBL = 0 ;
    reg JTAG_SEL3_GLBL = 0;
    reg JTAG_SEL4_GLBL = 0;

    reg JTAG_USER_TDO1_GLBL = 1'bz;
    reg JTAG_USER_TDO2_GLBL = 1'bz;
    reg JTAG_USER_TDO3_GLBL = 1'bz;
    reg JTAG_USER_TDO4_GLBL = 1'bz;

    assign (strong1, weak0) GSR = GSR_int;
    assign (strong1, weak0) GTS = GTS_int;
    assign (weak1, weak0) PRLD = PRLD_int;
    assign (strong1, weak0) GRESTORE = GRESTORE_int;

    initial begin
	GSR_int = 1'b1;
	PRLD_int = 1'b1;
	#(ROC_WIDTH)
	GSR_int = 1'b0;
	PRLD_int = 1'b0;
    end

    initial begin
	GTS_int = 1'b1;
	#(TOC_WIDTH)
	GTS_int = 1'b0;
    end

    initial begin 
	GRESTORE_int = 1'b0;
	#(GRES_START);
	GRESTORE_int = 1'b1;
	#(GRES_WIDTH);
	GRESTORE_int = 1'b0;
    end

endmodule
`endif
