// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2025.1 (win64) Build 6140274 Thu May 22 00:12:29 MDT 2025
// Date        : Tue Jul  8 14:20:03 2025
// Host        : DESKTOP-JP2R5GF running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode funcsim -rename_top sys_top_blk_mem_gen_0_0 -prefix
//               sys_top_blk_mem_gen_0_0_ sys_top_blk_mem_gen_0_0_sim_netlist.v
// Design      : sys_top_blk_mem_gen_0_0
// Purpose     : This verilog netlist is a functional simulation representation of the design and should not be modified
//               or synthesized. This netlist cannot be used for SDF annotated simulation.
// Device      : xczu17eg-ffvc1760-2-e
// --------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

(* CHECK_LICENSE_TYPE = "sys_top_blk_mem_gen_0_0,blk_mem_gen_v8_4_11,{}" *) (* downgradeipidentifiedwarnings = "yes" *) (* x_core_info = "blk_mem_gen_v8_4_11,Vivado 2025.1" *) 
(* NotValidForBitStream *)
module sys_top_blk_mem_gen_0_0
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
  sys_top_blk_mem_gen_0_0_blk_mem_gen_v8_4_11 U0
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
gQ/R0xt5Rw==
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
