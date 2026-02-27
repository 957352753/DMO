// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2025.1 (win64) Build 6140274 Thu May 22 00:12:29 MDT 2025
// Date        : Tue Jul  8 14:20:03 2025
// Host        : DESKTOP-JP2R5GF running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode funcsim -rename_top sys_top_blk_mem_gen_2_0 -prefix
//               sys_top_blk_mem_gen_2_0_ sys_top_blk_mem_gen_0_0_sim_netlist.v
// Design      : sys_top_blk_mem_gen_0_0
// Purpose     : This verilog netlist is a functional simulation representation of the design and should not be modified
//               or synthesized. This netlist cannot be used for SDF annotated simulation.
// Device      : xczu17eg-ffvc1760-2-e
// --------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

(* CHECK_LICENSE_TYPE = "sys_top_blk_mem_gen_0_0,blk_mem_gen_v8_4_11,{}" *) (* downgradeipidentifiedwarnings = "yes" *) (* x_core_info = "blk_mem_gen_v8_4_11,Vivado 2025.1" *) 
(* NotValidForBitStream *)
module sys_top_blk_mem_gen_2_0
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
  sys_top_blk_mem_gen_2_0_blk_mem_gen_v8_4_11 U0
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
QuP88vFLZQ==
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
