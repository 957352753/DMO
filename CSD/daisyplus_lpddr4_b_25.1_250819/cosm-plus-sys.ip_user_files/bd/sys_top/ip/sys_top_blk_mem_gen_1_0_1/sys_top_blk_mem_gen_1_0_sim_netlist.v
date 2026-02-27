// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2025.1 (win64) Build 6140274 Thu May 22 00:12:29 MDT 2025
// Date        : Tue Jul  8 14:20:03 2025
// Host        : DESKTOP-JP2R5GF running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode funcsim -rename_top sys_top_blk_mem_gen_1_0 -prefix
//               sys_top_blk_mem_gen_1_0_ sys_top_blk_mem_gen_0_0_sim_netlist.v
// Design      : sys_top_blk_mem_gen_0_0
// Purpose     : This verilog netlist is a functional simulation representation of the design and should not be modified
//               or synthesized. This netlist cannot be used for SDF annotated simulation.
// Device      : xczu17eg-ffvc1760-2-e
// --------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

(* CHECK_LICENSE_TYPE = "sys_top_blk_mem_gen_0_0,blk_mem_gen_v8_4_11,{}" *) (* downgradeipidentifiedwarnings = "yes" *) (* x_core_info = "blk_mem_gen_v8_4_11,Vivado 2025.1" *) 
(* NotValidForBitStream *)
module sys_top_blk_mem_gen_1_0
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
  sys_top_blk_mem_gen_1_0_blk_mem_gen_v8_4_11 U0
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
2F6pyfhwWw==
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
