`timescale 1ns / 1ps

module filter_accel #
(
    parameter integer C_S_AXI_ADDR_WIDTH = 8,
    parameter integer C_S_AXI_DATA_WIDTH = 32,
    parameter integer C_M_AXI_ADDR_WIDTH = 32,
    parameter integer C_M_AXI_DATA_WIDTH = 32
)
(
    input  wire                                aclk,
    input  wire                                aresetn,

    input  wire [C_S_AXI_ADDR_WIDTH-1:0]       s_axi_awaddr,
    input  wire [2:0]                          s_axi_awprot,
    input  wire                                s_axi_awvalid,
    output reg                                 s_axi_awready,
    input  wire [C_S_AXI_DATA_WIDTH-1:0]       s_axi_wdata,
    input  wire [(C_S_AXI_DATA_WIDTH/8)-1:0]   s_axi_wstrb,
    input  wire                                s_axi_wvalid,
    output reg                                 s_axi_wready,
    output reg  [1:0]                          s_axi_bresp,
    output reg                                 s_axi_bvalid,
    input  wire                                s_axi_bready,
    input  wire [C_S_AXI_ADDR_WIDTH-1:0]       s_axi_araddr,
    input  wire [2:0]                          s_axi_arprot,
    input  wire                                s_axi_arvalid,
    output reg                                 s_axi_arready,
    output reg  [C_S_AXI_DATA_WIDTH-1:0]       s_axi_rdata,
    output reg  [1:0]                          s_axi_rresp,
    output reg                                 s_axi_rvalid,
    input  wire                                s_axi_rready,

    output reg  [C_M_AXI_ADDR_WIDTH-1:0]       m_axi_awaddr,
    output wire [7:0]                          m_axi_awlen,
    output wire [2:0]                          m_axi_awsize,
    output wire [1:0]                          m_axi_awburst,
    output wire [3:0]                          m_axi_awcache,
    output wire [2:0]                          m_axi_awprot,
    output reg                                 m_axi_awvalid,
    input  wire                                m_axi_awready,
    output reg  [C_M_AXI_DATA_WIDTH-1:0]       m_axi_wdata,
    output wire [(C_M_AXI_DATA_WIDTH/8)-1:0]   m_axi_wstrb,
    output wire                                m_axi_wlast,
    output reg                                 m_axi_wvalid,
    input  wire                                m_axi_wready,
    input  wire [1:0]                          m_axi_bresp,
    input  wire                                m_axi_bvalid,
    output reg                                 m_axi_bready,
    output reg  [C_M_AXI_ADDR_WIDTH-1:0]       m_axi_araddr,
    output wire [7:0]                          m_axi_arlen,
    output wire [2:0]                          m_axi_arsize,
    output wire [1:0]                          m_axi_arburst,
    output wire [3:0]                          m_axi_arcache,
    output wire [2:0]                          m_axi_arprot,
    output reg                                 m_axi_arvalid,
    input  wire                                m_axi_arready,
    input  wire [C_M_AXI_DATA_WIDTH-1:0]       m_axi_rdata,
    input  wire [1:0]                          m_axi_rresp,
    input  wire                                m_axi_rlast,
    input  wire                                m_axi_rvalid,
    output reg                                 m_axi_rready
);

localparam REG_CONTROL      = 8'h00;
localparam REG_STATUS       = 8'h04;
localparam REG_PAGE_ADDR    = 8'h08;
localparam REG_TUPDESC_ADDR = 8'h0c;
localparam REG_FILTER_ADDR  = 8'h10;
localparam REG_QUAL_ADDR    = 8'h14;
localparam REG_HIT_ADDR     = 8'h18;
localparam REG_PAGE_ID      = 8'h1c;
localparam REG_NATTS        = 8'h20;
localparam REG_FILTER_CNT   = 8'h24;
localparam REG_TUPLE_CNT    = 8'h28;
localparam REG_HIT_COUNT    = 8'h2c;
localparam REG_ERROR        = 8'h30;
localparam REG_VERSION      = 8'h34;
localparam REG_PAGE_STRIDE  = 8'h38;
localparam REG_PAGE_COUNT   = 8'h3c;
localparam REG_HIT_STRIDE   = 8'h40;
localparam REG_TUPLE_CNT_ADDR = 8'h44;
localparam REG_PAGE_DONE    = 8'h48;
localparam REG_PAGE_ADDR_TABLE_ADDR = 8'h4c;
localparam REG_RESULT_MODE  = 8'h50;
localparam REG_PROJECTION_ADDR   = 8'h54;
localparam REG_PROJECTION_STRIDE = 8'h58;
localparam REG_PROJECTION_CONFIG = 8'h5c;
localparam REG_PROJECTION_COLS0  = 8'h60;
localparam REG_PROJECTION_COLS1  = 8'h64;
localparam REG_PROJECTION_COLS2  = 8'h68;
localparam REG_PROJECTION_COLS3  = 8'h6c;
localparam REG_COMPUTE_ADDR      = 8'h70;
localparam REG_COMPUTE_STRIDE    = 8'h74;
localparam REG_COMPUTE_CONFIG    = 8'h78;
localparam REG_COMPUTE_SEED      = 8'h7c;

localparam STATUS_DONE        = 32'h00000001;
localparam STATUS_BUSY        = 32'h00000002;
localparam STATUS_UNSUPPORTED = 32'h00000004;
localparam STATUS_ERROR       = 32'h00000008;

localparam ERR_NONE            = 32'd0;
localparam ERR_AXI_RESP        = 32'd1;
localparam ERR_UNSUPPORTED     = 32'd2;
localparam ERR_BOOL_STACK      = 32'd3;
localparam ERR_BAD_TUPLE_COUNT = 32'd4;
localparam ERR_PROJECTION_CONFIG = 32'd5;
localparam ERR_PROJECTION_OVERFLOW = 32'd6;
localparam ERR_COMPUTE_CONFIG    = 32'd7;

localparam T_VAR      = 8'd104;
localparam T_CONST    = 8'd105;
localparam T_OPEXPR   = 8'd113;
localparam T_BOOLEXPR = 8'd117;
localparam OP_AND     = 16'd0;
localparam OP_OR      = 16'd1;
localparam C_INT      = 8'd0;
localparam C_FLOAT8   = 8'd1;
localparam C_STRING   = 8'd2;

localparam FLOAT8_EQ = 16'd293;
localparam FLOAT8_NE = 16'd294;
localparam FLOAT8_LT = 16'd295;
localparam FLOAT8_LE = 16'd296;
localparam FLOAT8_GT = 16'd297;
localparam FLOAT8_GE = 16'd298;
localparam DATE_EQ = 16'd1086;
localparam DATE_LT = 16'd1087;
localparam DATE_LE = 16'd1088;
localparam DATE_GT = 16'd1089;
localparam DATE_GE = 16'd1090;
localparam DATE_NE = 16'd1091;
localparam INT4_EQ = 16'd64;
localparam INT4_LT = 16'd66;
localparam INT4_NE = 16'd144;
localparam INT4_LE = 16'd149;
localparam INT4_GT = 16'd147;
localparam INT4_GE = 16'd150;
localparam TEXT_EQ = 16'd67;
localparam TEXT_LT = 16'd740;
localparam TEXT_LE = 16'd741;
localparam TEXT_GT = 16'd742;
localparam TEXT_GE = 16'd743;
localparam TEXT_NE = 16'd157;
localparam BPCHAR_EQ = 16'd1048;
localparam BPCHAR_LT = 16'd1049;
localparam BPCHAR_LE = 16'd1050;
localparam BPCHAR_GT = 16'd1051;
localparam BPCHAR_GE = 16'd1052;
localparam BPCHAR_NE = 16'd1053;

localparam LP_NORMAL = 2'd1;
localparam PG_PAGE_SIZE_BYTES = 16'd8192;
localparam HIT_DESC_OFFSET = 32'h00000200;

localparam ST_IDLE              = 8'd0;
localparam ST_READ_HDR          = 8'd1;
localparam ST_READ_TUPDESC_LEN  = 8'd2;
localparam ST_READ_TUPDESC_ALN  = 8'd3;
localparam ST_TUPLE_ITEM        = 8'd4;
localparam ST_TUPLE_INIT        = 8'd5;
localparam ST_ATTR_PREP         = 8'd6;
localparam ST_ATTR_CALC         = 8'd7;
localparam ST_FILTER_INIT       = 8'd8;
localparam ST_READ_OP0          = 8'd9;
localparam ST_READ_OP1          = 8'd10;
localparam ST_READ_OP2          = 8'd11;
localparam ST_READ_OP3          = 8'd12;
localparam ST_ARG0              = 8'd13;
localparam ST_ARG1              = 8'd14;
localparam ST_EVAL_OP           = 8'd15;
localparam ST_EVAL_BOOL         = 8'd16;
localparam ST_HIT_CHECK         = 8'd17;
localparam ST_NEXT_TUPLE        = 8'd18;
localparam ST_DONE              = 8'd19;
localparam ST_FAIL              = 8'd20;
localparam ST_ATTR_VARLEN_CHECK = 8'd21;
localparam ST_ATTR_VARLEN_READ  = 8'd22;
localparam ST_ATTR_VARLEN_CROSS = 8'd23;
localparam ST_PAGE_START        = 8'd24;
localparam ST_READ_TUPLE_CNT    = 8'd25;
localparam ST_WRITE_PAGE_COUNT  = 8'd26;
localparam ST_NEXT_PAGE         = 8'd27;
localparam ST_LOAD_CLAUSE0      = 8'd28;
localparam ST_LOAD_CLAUSE1      = 8'd29;
localparam ST_LOAD_CLAUSE2      = 8'd30;
localparam ST_LOAD_CLAUSE3      = 8'd31;
localparam ST_LOAD_CLAUSE4      = 8'd32;
localparam ST_LOAD_CONST1       = 8'd33;
localparam ST_EVAL_OP_READ      = 8'd34;
localparam ST_LOAD_CONST2       = 8'd35;
localparam ST_ARG0_FLOAT_LO     = 8'd36;
localparam ST_ARG0_FLOAT_HI     = 8'd37;
localparam ST_ARG1_FLOAT_LO     = 8'd38;
localparam ST_ARG1_FLOAT_HI     = 8'd39;
localparam ST_STRING_READ0      = 8'd40;
localparam ST_STRING_READ0_NEXT = 8'd41;
localparam ST_STRING_READ1      = 8'd42;
localparam ST_STRING_READ1_NEXT = 8'd43;
localparam ST_STRING_EVAL       = 8'd44;
localparam ST_FAST_TUPLE_ITEM   = 8'd45;
localparam ST_FAST_TUPLE_INIT   = 8'd46;
localparam ST_FAST_TUPLE_HDR    = 8'd47;
localparam ST_FAST_INT_READ     = 8'd48;
localparam ST_FAST_FLOAT_LO     = 8'd49;
localparam ST_FAST_FLOAT_HI     = 8'd50;
localparam ST_FAST_TAG_HDR      = 8'd51;
localparam ST_FAST_TAG_DATA0    = 8'd52;
localparam ST_FAST_TAG_DATA1    = 8'd53;
localparam ST_FAST_HIT_CHECK    = 8'd54;
localparam ST_LOAD_STR_CONST0   = 8'd55;
localparam ST_LOAD_STR_CONST1   = 8'd56;
localparam ST_FAST_NEXT_TUPLE   = 8'd57;
localparam ST_READ_PAGE_ADDR    = 8'd58;
localparam ST_FAST_LAYOUT_FIRST = 8'd59;
localparam ST_FAST_LAYOUT_LAST  = 8'd60;
localparam ST_FAST_LAYOUT_HDR   = 8'd61;
localparam ST_FAST_LAYOUT_TUPLE = 8'd62;
localparam ST_FAST_LAYOUT_STR0  = 8'd63;
localparam ST_FAST_INT_PIPE_INIT = 8'd64;
localparam ST_FAST_INT_PIPE_RUN  = 8'd65;
localparam ST_FAST_INT_PIPE_WRITE = 8'd66;
localparam ST_FAST_INT_PIPE_WRITE_NEXT = 8'd67;
localparam ST_PROJ_ROW_BEGIN      = 8'd68;
localparam ST_PROJ_ROW_META       = 8'd69;
localparam ST_PROJ_DESC_OFFSET    = 8'd70;
localparam ST_PROJ_DESC_META      = 8'd71;
localparam ST_PROJ_DESC_NEXT      = 8'd72;
localparam ST_PROJ_DATA_PREP      = 8'd73;
localparam ST_PROJ_DATA_READ0     = 8'd74;
localparam ST_PROJ_DATA_READ1     = 8'd75;
localparam ST_PROJ_DATA_ADVANCE   = 8'd76;
localparam ST_COMP_PREP           = 8'd77;
localparam ST_COMP_READ_LO        = 8'd78;
localparam ST_COMP_READ_HI        = 8'd79;
localparam ST_COMP_FLUSH_INIT     = 8'd80;
localparam ST_COMP_FLUSH_WRITE    = 8'd81;
localparam ST_COMP_FLUSH_NEXT     = 8'd82;
localparam ST_COMP_HASH_STEP      = 8'd83;
localparam ST_WRITE_DRAIN         = 8'd84;
localparam ST_FAST_COMP_LAYOUT_PREP = 8'd85;
localparam ST_MATCH_ALL_BURST_INIT = 8'd86;
localparam ST_MATCH_ALL_BURST_RUN  = 8'd87;
localparam ST_MATCH_ALL_BURST_DRAIN = 8'd88;

localparam FAST_NONE            = 3'd0;
localparam FAST_INT_SCORE_LT    = 3'd1;
localparam FAST_FLOAT_SCORE_LT  = 3'd2;
localparam FAST_STRING_TAG_EQ   = 3'd3;
localparam FAST_COMPUTE_INT32   = 3'd4;
localparam FAST_MATCH_ALL       = 3'd5;
localparam FAST_PIPE_MAX_OUTSTANDING = 6'd32;
localparam WRITE_MAX_OUTSTANDING = 7'd16;

reg [31:0] reg_page_addr;
reg [31:0] reg_tupdesc_addr;
reg [31:0] reg_filter_addr;
reg [31:0] reg_qual_addr;
reg [31:0] reg_hit_addr;
reg [31:0] reg_page_id;
reg [31:0] reg_natts;
reg [31:0] reg_filter_cnt;
reg [31:0] reg_tuple_cnt;
reg [31:0] reg_page_stride;
reg [31:0] reg_page_count;
reg [31:0] reg_hit_stride;
reg [31:0] reg_tuple_cnt_addr;
reg [31:0] reg_page_addr_table_addr;
reg [31:0] reg_result_mode;
reg [31:0] reg_projection_addr;
reg [31:0] reg_projection_stride;
reg [31:0] reg_projection_config;
reg [31:0] reg_projection_cols0;
reg [31:0] reg_projection_cols1;
reg [31:0] reg_projection_cols2;
reg [31:0] reg_projection_cols3;
reg [31:0] reg_compute_addr;
reg [31:0] reg_compute_stride;
reg [31:0] reg_compute_config;
reg [31:0] reg_compute_seed;

reg [31:0] status_reg;
reg [31:0] error_reg;
reg [31:0] hit_count_reg;
reg [31:0] page_done_reg;
reg start_req;
reg clear_req;
reg aw_hold_valid;
reg w_hold_valid;
reg [C_S_AXI_ADDR_WIDTH-1:0] aw_hold_addr;
reg [C_S_AXI_DATA_WIDTH-1:0] w_hold_data;

reg [7:0] state;
reg [7:0] ret_state;
reg [7:0] write_ret_state;
reg read_pending;
reg write_pending;
reg [6:0] write_outstanding;
reg [7:0] read_burst_len;
reg [31:0] read_data;

reg [7:0] page_idx;
reg [31:0] current_page_addr;
reg [31:0] page_hit_count;
reg [15:0] tuple_count_override;
reg desc_loaded;
reg [7:0] tuple_idx;
reg [15:0] tuple_count;
reg [15:0] page_lower;
reg [15:0] tuple_off;
reg [15:0] tuple_len;
reg [1:0] tuple_flags;
reg [15:0] pos_index;
reg [7:0] attr_idx;
reg [7:0] scan_natts;
reg [7:0] clause_idx;
reg [7:0] load_clause_idx;
reg [2:0] fast_mode;
reg fast_const_valid;
reg fast_hit;
reg fast_layout_active;
reg [15:0] fast_layout_first_off;
reg [15:0] fast_layout_tuple_len;
reg [15:0] fast_layout_tuple_span;
reg [15:0] fast_layout_last_expected;
reg [7:0] fast_pipe_issue_idx;
reg [7:0] fast_pipe_resp_idx;
reg [7:0] fast_pipe_write_idx;
reg [7:0] fast_pipe_hit_count;
reg [5:0] fast_pipe_outstanding;
reg fast_pipe_issue_fire;
reg fast_pipe_resp_fire;
reg fast_pipe_issue_word;
reg fast_pipe_resp_word;
reg [31:0] fast_pipe_word0;
reg fast_pipe_tuple_hit;
reg fast_pipe_unsupported;
reg [15:0] fast_compute_attr_offset;
reg [15:0] fast_compute_offset_work;

reg signed [15:0] desc_len [0:15];
reg [15:0] desc_align [0:15];
reg [15:0] att_pos [0:15];
reg [15:0] att_len [0:15];
reg [31:0] fast_pipe_hit_desc [0:127];
reg bool_stack [0:63];
reg [6:0] bool_top;
reg [31:0] clause_word0 [0:63];
reg [31:0] clause_word1 [0:63];
reg [31:0] clause_word2 [0:63];
reg [31:0] clause_word3 [0:63];
reg [31:0] clause_const1_lo [0:63];
reg [31:0] clause_const1_hi [0:63];
reg clause_const1_valid [0:63];

reg [31:0] op0;
reg [31:0] op1;
reg [31:0] op2;
reg [31:0] op3;
reg [7:0] op_type;
reg [7:0] op_class;
reg [15:0] op_func_id;
reg [7:0] op_nargs;
reg [7:0] op_arg0_tag;
reg [15:0] op_arg0_index;
reg [15:0] op_arg0_len;
reg [7:0] op_arg1_tag;
reg [15:0] op_arg1_index;
reg [15:0] op_arg1_len;
reg signed [31:0] arg0;
reg signed [31:0] arg1;
reg [63:0] arg0_float;
reg [63:0] arg1_float;
reg [31:0] arg0_addr;
reg [31:0] arg1_addr;
reg op_bool;
reg [7:0] bool_sum;
reg unsupported_func;

reg [31:0] str0_base;
reg [31:0] str1_base;
reg [15:0] str0_len;
reg [15:0] str1_len;
reg [15:0] str_idx;
reg [7:0] str0_byte;
reg [1:0] str0_sel;
reg [1:0] str1_sel;
reg str_equal;
reg str_byte_equal;
reg [2:0] str_chunk_len;
reg [31:0] str0_word0;
reg [31:0] str0_word1;
reg [31:0] str1_word0;
reg [31:0] str1_word1;
reg [31:0] str0_chunk;
reg [31:0] str1_chunk;
reg [31:0] str_chunk_mask;
reg str_chunk_equal;
reg [31:0] fast_const_lo;
reg [31:0] fast_const_hi;
reg [15:0] fast_tag_header_pos;
reg [15:0] fast_tag_data_pos;
reg [15:0] fast_tag_len;
reg [1:0] fast_tag_sel;

reg [15:0] aligned_pos;
reg [15:0] var_start_pos;
reg [15:0] var_aligned_pos;
reg [15:0] var_data_pos;
reg [1:0] var_byte_sel;
reg [7:0] var_first_byte;
reg [15:0] var_header16;
reg [15:0] var_total_len;
reg [15:0] var_meta_len;
reg [31:0] read_addr;
reg [31:0] write_addr;
reg [31:0] write_data;
reg [31:0] projection_page_cursor;
reg [31:0] projection_row_base;
reg [31:0] projection_row_bytes;
reg [31:0] projection_data_cursor;
reg [7:0] projection_col_idx;
reg [7:0] projection_selected_attr;
reg [31:0] projection_src_addr;
reg [31:0] projection_dst_addr;
reg [15:0] projection_data_remaining;
reg [1:0] projection_src_sel;
reg [2:0] projection_chunk_len;
reg [31:0] projection_word0;
reg [31:0] projection_row_size_work;
reg [31:0] projection_chunk_work;
reg [7:0] projection_attr_work;
reg projection_attr_invalid_work;
reg [7:0] compute_hit_index;
reg [31:0] compute_word_lo;
reg [63:0] compute_nonnull_count;
reg [63:0] compute_null_count;
reg signed [63:0] compute_sum;
reg [63:0] compute_min;
reg [63:0] compute_max;
reg compute_initialized;
reg compute_overflow;
reg [4:0] compute_flush_index;
reg [63:0] compute_value_work;
reg signed [63:0] compute_signed_work;
reg signed [63:0] compute_sum_work;
reg [31:0] compute_hash_work;
reg [63:0] compute_hash_value;
reg [3:0] compute_hash_index;
reg [7:0] compute_attr_work;
integer i;

wire fast_wrapped_single_clause;
wire fast_pipe_two_word_mode;
wire write_addr_fire;
wire write_data_fire;
wire write_issue_complete;
wire write_response_fire;

assign fast_wrapped_single_clause =
    (reg_filter_cnt[7:0] == 8'd2 &&
     clause_word0[1][7:0] == T_BOOLEXPR &&
     clause_word0[1][31:16] == OP_AND &&
     clause_word1[1][7:0] == 8'd1);

assign fast_pipe_two_word_mode =
    (fast_mode == FAST_FLOAT_SCORE_LT ||
     fast_mode == FAST_STRING_TAG_EQ ||
     fast_mode == FAST_COMPUTE_INT32);

assign write_addr_fire = m_axi_awvalid && m_axi_awready;
assign write_data_fire = m_axi_wvalid && m_axi_wready;
assign write_issue_complete = write_pending &&
    (!m_axi_awvalid || write_addr_fire) &&
    (!m_axi_wvalid || write_data_fire);
assign write_response_fire = m_axi_bvalid && m_axi_bready;

assign m_axi_awlen   = 8'd0;
assign m_axi_awsize  = 3'd2;
assign m_axi_awburst = 2'b01;
assign m_axi_awcache = 4'b0011;
assign m_axi_awprot  = 3'b000;
assign m_axi_wstrb   = 4'hf;
assign m_axi_wlast   = 1'b1;
assign m_axi_arlen   = read_burst_len;
assign m_axi_arsize  = 3'd2;
assign m_axi_arburst = 2'b01;
assign m_axi_arcache = 4'b0011;
assign m_axi_arprot  = 3'b000;

always @(posedge aclk) begin
    if (!aresetn) begin
        s_axi_awready <= 1'b0;
        s_axi_wready <= 1'b0;
        s_axi_bresp <= 2'b00;
        s_axi_bvalid <= 1'b0;
        s_axi_arready <= 1'b0;
        s_axi_rdata <= 32'd0;
        s_axi_rresp <= 2'b00;
        s_axi_rvalid <= 1'b0;
        reg_page_addr <= 32'd0;
        reg_tupdesc_addr <= 32'd0;
        reg_filter_addr <= 32'd0;
        reg_qual_addr <= 32'd0;
        reg_hit_addr <= 32'd0;
        reg_page_id <= 32'd0;
        reg_natts <= 32'd0;
        reg_filter_cnt <= 32'd0;
        reg_tuple_cnt <= 32'd0;
        reg_page_stride <= 32'd8192;
        reg_page_count <= 32'd1;
        reg_hit_stride <= 32'd0;
        reg_tuple_cnt_addr <= 32'd0;
        reg_page_addr_table_addr <= 32'd0;
        reg_result_mode <= 32'd0;
        reg_projection_addr <= 32'd0;
        reg_projection_stride <= 32'd8192;
        reg_projection_config <= 32'd0;
        reg_projection_cols0 <= 32'd0;
        reg_projection_cols1 <= 32'd0;
        reg_projection_cols2 <= 32'd0;
        reg_projection_cols3 <= 32'd0;
        reg_compute_addr <= 32'd0;
        reg_compute_stride <= 32'd8192;
        reg_compute_config <= 32'd0;
        reg_compute_seed <= 32'd0;
        start_req <= 1'b0;
        clear_req <= 1'b0;
        aw_hold_valid <= 1'b0;
        w_hold_valid <= 1'b0;
        aw_hold_addr <= {C_S_AXI_ADDR_WIDTH{1'b0}};
        w_hold_data <= {C_S_AXI_DATA_WIDTH{1'b0}};
    end else begin
        s_axi_awready <= 1'b0;
        s_axi_wready <= 1'b0;
        s_axi_arready <= 1'b0;
        start_req <= 1'b0;
        clear_req <= 1'b0;

        if (!s_axi_bvalid && !aw_hold_valid && s_axi_awvalid) begin
            s_axi_awready <= 1'b1;
            aw_hold_valid <= 1'b1;
            aw_hold_addr <= s_axi_awaddr;
        end

        if (!s_axi_bvalid && !w_hold_valid && s_axi_wvalid) begin
            s_axi_wready <= 1'b1;
            w_hold_valid <= 1'b1;
            w_hold_data <= s_axi_wdata;
        end

        if (!s_axi_bvalid &&
            ((aw_hold_valid || (s_axi_awvalid && !aw_hold_valid)) &&
             (w_hold_valid || (s_axi_wvalid && !w_hold_valid)))) begin
            s_axi_bvalid <= 1'b1;
            s_axi_bresp <= 2'b00;
            aw_hold_valid <= 1'b0;
            w_hold_valid <= 1'b0;

            case ((aw_hold_valid ? aw_hold_addr : s_axi_awaddr) & 8'hfc)
                REG_CONTROL: begin
                    if (w_hold_valid) begin
                        start_req <= w_hold_data[0];
                        clear_req <= w_hold_data[1];
                    end else begin
                        start_req <= s_axi_wdata[0];
                        clear_req <= s_axi_wdata[1];
                    end
                end
                REG_PAGE_ADDR:    reg_page_addr <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_TUPDESC_ADDR: reg_tupdesc_addr <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_FILTER_ADDR:  reg_filter_addr <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_QUAL_ADDR:    reg_qual_addr <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_HIT_ADDR:     reg_hit_addr <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_PAGE_ID:      reg_page_id <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_NATTS:        reg_natts <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_FILTER_CNT:   reg_filter_cnt <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_TUPLE_CNT:    reg_tuple_cnt <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_PAGE_STRIDE:  reg_page_stride <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_PAGE_COUNT:   reg_page_count <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_HIT_STRIDE:   reg_hit_stride <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_TUPLE_CNT_ADDR: reg_tuple_cnt_addr <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_PAGE_ADDR_TABLE_ADDR: reg_page_addr_table_addr <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_RESULT_MODE: reg_result_mode <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_PROJECTION_ADDR: reg_projection_addr <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_PROJECTION_STRIDE: reg_projection_stride <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_PROJECTION_CONFIG: reg_projection_config <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_PROJECTION_COLS0: reg_projection_cols0 <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_PROJECTION_COLS1: reg_projection_cols1 <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_PROJECTION_COLS2: reg_projection_cols2 <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_PROJECTION_COLS3: reg_projection_cols3 <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_COMPUTE_ADDR: reg_compute_addr <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_COMPUTE_STRIDE: reg_compute_stride <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_COMPUTE_CONFIG: reg_compute_config <= w_hold_valid ? w_hold_data : s_axi_wdata;
                REG_COMPUTE_SEED: reg_compute_seed <= w_hold_valid ? w_hold_data : s_axi_wdata;
                default: begin end
            endcase
        end

        if (s_axi_bvalid && s_axi_bready)
            s_axi_bvalid <= 1'b0;

        if (!s_axi_rvalid && s_axi_arvalid) begin
            s_axi_arready <= 1'b1;
            s_axi_rvalid <= 1'b1;
            s_axi_rresp <= 2'b00;
            case (s_axi_araddr & 8'hfc)
                REG_CONTROL:      s_axi_rdata <= 32'd0;
                REG_STATUS:       s_axi_rdata <= status_reg;
                REG_PAGE_ADDR:    s_axi_rdata <= reg_page_addr;
                REG_TUPDESC_ADDR: s_axi_rdata <= reg_tupdesc_addr;
                REG_FILTER_ADDR:  s_axi_rdata <= reg_filter_addr;
                REG_QUAL_ADDR:    s_axi_rdata <= reg_qual_addr;
                REG_HIT_ADDR:     s_axi_rdata <= reg_hit_addr;
                REG_PAGE_ID:      s_axi_rdata <= reg_page_id;
                REG_NATTS:        s_axi_rdata <= reg_natts;
                REG_FILTER_CNT:   s_axi_rdata <= reg_filter_cnt;
                REG_TUPLE_CNT:    s_axi_rdata <= reg_tuple_cnt;
                REG_HIT_COUNT:    s_axi_rdata <= hit_count_reg;
                REG_ERROR:        s_axi_rdata <= error_reg;
                REG_VERSION:      s_axi_rdata <= 32'h0001000a;
                REG_PAGE_STRIDE:  s_axi_rdata <= reg_page_stride;
                REG_PAGE_COUNT:   s_axi_rdata <= reg_page_count;
                REG_HIT_STRIDE:   s_axi_rdata <= reg_hit_stride;
                REG_TUPLE_CNT_ADDR: s_axi_rdata <= reg_tuple_cnt_addr;
                REG_PAGE_DONE:    s_axi_rdata <= page_done_reg;
                REG_PAGE_ADDR_TABLE_ADDR: s_axi_rdata <= reg_page_addr_table_addr;
                REG_RESULT_MODE:  s_axi_rdata <= reg_result_mode;
                REG_PROJECTION_ADDR: s_axi_rdata <= reg_projection_addr;
                REG_PROJECTION_STRIDE: s_axi_rdata <= reg_projection_stride;
                REG_PROJECTION_CONFIG: s_axi_rdata <= reg_projection_config;
                REG_PROJECTION_COLS0: s_axi_rdata <= reg_projection_cols0;
                REG_PROJECTION_COLS1: s_axi_rdata <= reg_projection_cols1;
                REG_PROJECTION_COLS2: s_axi_rdata <= reg_projection_cols2;
                REG_PROJECTION_COLS3: s_axi_rdata <= reg_projection_cols3;
                REG_COMPUTE_ADDR: s_axi_rdata <= reg_compute_addr;
                REG_COMPUTE_STRIDE: s_axi_rdata <= reg_compute_stride;
                REG_COMPUTE_CONFIG: s_axi_rdata <= reg_compute_config;
                REG_COMPUTE_SEED: s_axi_rdata <= reg_compute_seed;
                default:          s_axi_rdata <= 32'd0;
            endcase
        end

        if (s_axi_rvalid && s_axi_rready)
            s_axi_rvalid <= 1'b0;
    end
end

task start_read32;
    input [31:0] addr;
    input [7:0] next_state;
    begin
        m_axi_araddr <= addr;
        read_burst_len <= 8'd0;
        m_axi_arvalid <= 1'b1;
        m_axi_rready <= 1'b0;
        ret_state <= next_state;
        read_pending <= 1'b1;
    end
endtask

task start_write32;
    input [31:0] addr;
    input [31:0] data;
    input [7:0] next_state;
    begin
        if (write_outstanding < WRITE_MAX_OUTSTANDING) begin
            m_axi_awaddr <= addr;
            m_axi_wdata <= data;
            m_axi_awvalid <= 1'b1;
            m_axi_wvalid <= 1'b1;
            write_ret_state <= next_state;
            write_pending <= 1'b1;
        end
    end
endtask

function [7:0] select_byte32;
    input [31:0] word;
    input [1:0] sel;
    begin
        case (sel)
            2'd0: select_byte32 = word[7:0];
            2'd1: select_byte32 = word[15:8];
            2'd2: select_byte32 = word[23:16];
            default: select_byte32 = word[31:24];
        endcase
end
endfunction

function [31:0] string_chunk32;
    input [31:0] word0;
    input [31:0] word1;
    input [1:0] sel;
    begin
        case (sel)
            2'd0: string_chunk32 = word0;
            2'd1: string_chunk32 = {word1[7:0],  word0[31:8]};
            2'd2: string_chunk32 = {word1[15:0], word0[31:16]};
            default: string_chunk32 = {word1[23:0], word0[31:24]};
        endcase
    end
endfunction

function [31:0] string_chunk_mask32;
    input [2:0] len;
    begin
        case (len)
            3'd1: string_chunk_mask32 = 32'h000000ff;
            3'd2: string_chunk_mask32 = 32'h0000ffff;
            3'd3: string_chunk_mask32 = 32'h00ffffff;
            default: string_chunk_mask32 = 32'hffffffff;
        endcase
    end
endfunction

function [2:0] string_chunk_len4;
    input [15:0] remaining;
    begin
        if (remaining > 16'd4)
            string_chunk_len4 = 3'd4;
        else
            string_chunk_len4 = remaining[2:0];
end
endfunction

function [7:0] projection_attr_index;
    input [3:0] index;
    begin
        case (index)
            4'd0:  projection_attr_index = reg_projection_cols0[7:0];
            4'd1:  projection_attr_index = reg_projection_cols0[15:8];
            4'd2:  projection_attr_index = reg_projection_cols0[23:16];
            4'd3:  projection_attr_index = reg_projection_cols0[31:24];
            4'd4:  projection_attr_index = reg_projection_cols1[7:0];
            4'd5:  projection_attr_index = reg_projection_cols1[15:8];
            4'd6:  projection_attr_index = reg_projection_cols1[23:16];
            4'd7:  projection_attr_index = reg_projection_cols1[31:24];
            4'd8:  projection_attr_index = reg_projection_cols2[7:0];
            4'd9:  projection_attr_index = reg_projection_cols2[15:8];
            4'd10: projection_attr_index = reg_projection_cols2[23:16];
            4'd11: projection_attr_index = reg_projection_cols2[31:24];
            4'd12: projection_attr_index = reg_projection_cols3[7:0];
            4'd13: projection_attr_index = reg_projection_cols3[15:8];
            4'd14: projection_attr_index = reg_projection_cols3[23:16];
            default: projection_attr_index = reg_projection_cols3[31:24];
        endcase
    end
endfunction

function string_eq_upto8_no_cross;
    input [31:0] word0;
    input [31:0] word1;
    input [1:0] sel;
    input [15:0] len;
    input [31:0] const0;
    input [31:0] const1;
    reg [31:0] chunk0;
    reg [31:0] chunk1;
    reg [31:0] mask0;
    reg [31:0] mask1;
    reg [15:0] rem;
    reg [2:0] avail_after_first;
    begin
        if (len == 16'd0) begin
            string_eq_upto8_no_cross = 1'b1;
        end else if (len <= 16'd4) begin
            chunk0 = string_chunk32(word0, word1, sel);
            mask0 = string_chunk_mask32(len[2:0]);
            string_eq_upto8_no_cross = ((chunk0 & mask0) == (const0 & mask0));
        end else if (len > 16'd8) begin
            string_eq_upto8_no_cross = 1'b0;
        end else begin
            rem = len - 16'd4;
            avail_after_first = 3'd4 - {1'b0, sel};
            chunk0 = string_chunk32(word0, word1, sel);
            mask0 = 32'hffffffff;
            case (sel)
                2'd0: chunk1 = word1;
                2'd1: chunk1 = {8'd0, word1[31:8]};
                2'd2: chunk1 = {16'd0, word1[31:16]};
                default: chunk1 = {24'd0, word1[31:24]};
            endcase
            mask1 = string_chunk_mask32(rem[2:0]);
            string_eq_upto8_no_cross =
                (rem <= {13'd0, avail_after_first}) &&
                ((chunk0 & mask0) == (const0 & mask0)) &&
                ((chunk1 & mask1) == (const1 & mask1));
        end
    end
endfunction

function float64_is_nan;
    input [63:0] value;
    begin
        float64_is_nan = (value[62:52] == 11'h7ff) && (value[51:0] != 52'd0);
    end
endfunction

function float64_eq_cmp;
    input [63:0] lhs;
    input [63:0] rhs;
    begin
        if (float64_is_nan(lhs) || float64_is_nan(rhs))
            float64_eq_cmp = 1'b0;
        else if (lhs[62:0] == 63'd0 && rhs[62:0] == 63'd0)
            float64_eq_cmp = 1'b1;
        else
            float64_eq_cmp = (lhs == rhs);
    end
endfunction

function float64_lt_cmp;
    input [63:0] lhs;
    input [63:0] rhs;
    begin
        if (float64_is_nan(lhs) || float64_is_nan(rhs) || float64_eq_cmp(lhs, rhs)) begin
            float64_lt_cmp = 1'b0;
        end else if (lhs[63] != rhs[63]) begin
            float64_lt_cmp = lhs[63];
        end else if (!lhs[63]) begin
            float64_lt_cmp = (lhs[62:0] < rhs[62:0]);
        end else begin
            float64_lt_cmp = (lhs[62:0] > rhs[62:0]);
        end
    end
endfunction

function [31:0] compute_fnv1a_byte;
    input [31:0] hash;
    input [7:0] value;
    begin
        compute_fnv1a_byte = (hash ^ {24'd0, value}) * 32'd16777619;
    end
endfunction

function [7:0] compute_hash_value_byte;
    input [3:0] index;
    input [3:0] value_type;
    input [63:0] value;
    begin
        case (index)
            4'd0: compute_hash_value_byte = {4'd0, value_type};
            4'd1: compute_hash_value_byte = value[7:0];
            4'd2: compute_hash_value_byte = value[15:8];
            4'd3: compute_hash_value_byte = value[23:16];
            4'd4: compute_hash_value_byte = value[31:24];
            4'd5: compute_hash_value_byte = value[39:32];
            4'd6: compute_hash_value_byte = value[47:40];
            4'd7: compute_hash_value_byte = value[55:48];
            default: compute_hash_value_byte = value[63:56];
        endcase
    end
endfunction

wire [7:0] compute_hash_byte = compute_hash_value_byte(
    compute_hash_index, reg_compute_config[7:4], compute_hash_value);
wire [31:0] compute_hash_next = compute_fnv1a_byte(
    compute_hash_work, compute_hash_byte);

function [31:0] compute_partial_word;
    input [4:0] index;
    begin
        if (reg_compute_config[3:0] == 4'd2) begin
            case (index)
                5'd0: compute_partial_word = 32'h31485346;
                5'd1: compute_partial_word = 32'h00100001;
                5'd2: compute_partial_word = 32'd0;
                default: compute_partial_word = page_hit_count;
            endcase
        end else begin
            case (index)
                5'd0: compute_partial_word = 32'h31414746;
                5'd1: compute_partial_word = 32'h00400001;
                5'd2: compute_partial_word = {31'd0, compute_overflow};
                5'd3: compute_partial_word =
                    {8'd0, reg_compute_config[23:16],
                     reg_compute_config[15:8], 4'd0, reg_compute_config[7:4]};
                5'd4: compute_partial_word = compute_nonnull_count[31:0];
                5'd5: compute_partial_word = compute_nonnull_count[63:32];
                5'd6: compute_partial_word = compute_null_count[31:0];
                5'd7: compute_partial_word = compute_null_count[63:32];
                5'd8: compute_partial_word = compute_sum[31:0];
                5'd9: compute_partial_word = compute_sum[63:32];
                5'd10: compute_partial_word = compute_min[31:0];
                5'd11: compute_partial_word = compute_min[63:32];
                5'd12: compute_partial_word = compute_max[31:0];
                5'd13: compute_partial_word = compute_max[63:32];
                default: compute_partial_word = 32'd0;
            endcase
        end
    end
endfunction

task push_bool_result;
    input value;
    begin
        if (bool_top >= 7'd64) begin
            status_reg <= STATUS_DONE | STATUS_ERROR;
            error_reg <= ERR_BOOL_STACK;
            state <= ST_FAIL;
        end else begin
            bool_stack[bool_top] <= value;
            bool_top <= bool_top + 7'd1;
            clause_idx <= clause_idx + 8'd1;
            state <= ST_READ_OP0;
        end
    end
endtask

task finish_varlen_attr;
    input [15:0] data_pos;
    input [15:0] header16;
    begin
        if (header16[0]) begin
            var_total_len = {9'd0, header16[7:1]};
            var_meta_len = 16'd1;
        end else begin
            var_total_len = {2'd0, header16[15:2]};
            var_meta_len = 16'd4;
        end

        if (var_total_len < var_meta_len ||
            ({1'b0, data_pos} + {1'b0, var_total_len}) > {1'b0, tuple_len}) begin
            state <= ST_NEXT_TUPLE;
        end else begin
            att_pos[attr_idx] <= data_pos + var_meta_len;
            att_len[attr_idx] <= var_total_len - var_meta_len;
            pos_index <= data_pos + var_total_len;
            attr_idx <= attr_idx + 8'd1;
            state <= ST_ATTR_CALC;
        end
    end
endtask

task prepare_varlen_header;
    input [15:0] data_pos;
    begin
        if (({1'b0, data_pos} + 17'd2) > {1'b0, tuple_len}) begin
            state <= ST_NEXT_TUPLE;
        end else begin
            var_data_pos <= data_pos;
            var_byte_sel = (tuple_off + data_pos) & 16'd3;
            if (((tuple_off + data_pos) & 16'd3) == 16'd0) begin
                var_header16 = read_data[15:0];
                finish_varlen_attr(data_pos, read_data[15:0]);
            end else if (((tuple_off + data_pos) & 16'd3) == 16'd1) begin
                var_header16 = read_data[23:8];
                finish_varlen_attr(data_pos, read_data[23:8]);
            end else if (((tuple_off + data_pos) & 16'd3) == 16'd2) begin
                var_header16 = read_data[31:16];
                finish_varlen_attr(data_pos, read_data[31:16]);
            end else begin
                var_first_byte <= read_data[31:24];
                start_read32((current_page_addr + {16'd0, tuple_off + data_pos + 16'd1}) & 32'hfffffffc,
                             ST_ATTR_VARLEN_CROSS);
            end
        end
    end
endtask

always @(posedge aclk) begin
    if (!aresetn) begin
        m_axi_awaddr <= 32'd0;
        m_axi_awvalid <= 1'b0;
        m_axi_wdata <= 32'd0;
        m_axi_wvalid <= 1'b0;
        m_axi_bready <= 1'b0;
        m_axi_araddr <= 32'd0;
        m_axi_arvalid <= 1'b0;
        m_axi_rready <= 1'b0;
        read_pending <= 1'b0;
        write_pending <= 1'b0;
        write_outstanding <= 7'd0;
        read_burst_len <= 8'd0;
        read_data <= 32'd0;
        ret_state <= ST_IDLE;
        write_ret_state <= ST_IDLE;
        state <= ST_IDLE;
        status_reg <= STATUS_DONE;
        error_reg <= ERR_NONE;
        hit_count_reg <= 32'd0;
        page_done_reg <= 32'd0;
        page_idx <= 8'd0;
        current_page_addr <= 32'd0;
        page_hit_count <= 32'd0;
        tuple_count_override <= 16'd0;
        desc_loaded <= 1'b0;
        tuple_idx <= 8'd0;
        tuple_count <= 16'd0;
        page_lower <= 16'd0;
        tuple_off <= 16'd0;
        tuple_len <= 16'd0;
        tuple_flags <= 2'd0;
        pos_index <= 16'd0;
        attr_idx <= 8'd0;
        scan_natts <= 8'd0;
        clause_idx <= 8'd0;
        load_clause_idx <= 8'd0;
        fast_mode <= FAST_NONE;
        fast_const_valid <= 1'b0;
        fast_hit <= 1'b0;
        fast_layout_active <= 1'b0;
        fast_layout_first_off <= 16'd0;
        fast_layout_tuple_len <= 16'd0;
        fast_layout_tuple_span <= 16'd0;
        fast_layout_last_expected <= 16'd0;
        fast_pipe_issue_idx <= 8'd0;
        fast_pipe_resp_idx <= 8'd0;
        fast_pipe_write_idx <= 8'd0;
        fast_pipe_hit_count <= 8'd0;
        fast_pipe_outstanding <= 6'd0;
        fast_pipe_issue_fire <= 1'b0;
        fast_pipe_resp_fire <= 1'b0;
        fast_pipe_issue_word <= 1'b0;
        fast_pipe_resp_word <= 1'b0;
        fast_pipe_word0 <= 32'd0;
        fast_pipe_tuple_hit <= 1'b0;
        fast_pipe_unsupported <= 1'b0;
        fast_compute_attr_offset <= 16'd0;
        fast_compute_offset_work <= 16'd0;
        bool_top <= 7'd0;
        aligned_pos <= 16'd0;
        var_start_pos <= 16'd0;
        var_aligned_pos <= 16'd0;
        var_data_pos <= 16'd0;
        var_byte_sel <= 2'd0;
        var_first_byte <= 8'd0;
        var_header16 <= 16'd0;
        var_total_len <= 16'd0;
        var_meta_len <= 16'd0;
        arg0_float <= 64'd0;
        arg1_float <= 64'd0;
        arg0_addr <= 32'd0;
        arg1_addr <= 32'd0;
        str0_base <= 32'd0;
        str1_base <= 32'd0;
        str0_len <= 16'd0;
        str1_len <= 16'd0;
        str_idx <= 16'd0;
        str0_byte <= 8'd0;
        str0_sel <= 2'd0;
        str1_sel <= 2'd0;
        str_equal <= 1'b1;
        str_byte_equal <= 1'b1;
        str_chunk_len <= 3'd0;
        str0_word0 <= 32'd0;
        str0_word1 <= 32'd0;
        str1_word0 <= 32'd0;
        str1_word1 <= 32'd0;
        str0_chunk <= 32'd0;
        str1_chunk <= 32'd0;
        str_chunk_mask <= 32'd0;
        str_chunk_equal <= 1'b1;
        fast_const_lo <= 32'd0;
        fast_const_hi <= 32'd0;
        fast_tag_header_pos <= 16'd0;
        fast_tag_data_pos <= 16'd0;
        fast_tag_len <= 16'd0;
        fast_tag_sel <= 2'd0;
        projection_page_cursor <= 32'd0;
        projection_row_base <= 32'd0;
        projection_row_bytes <= 32'd0;
        projection_data_cursor <= 32'd0;
        projection_col_idx <= 8'd0;
        projection_selected_attr <= 8'd0;
        projection_src_addr <= 32'd0;
        projection_dst_addr <= 32'd0;
        projection_data_remaining <= 16'd0;
        projection_src_sel <= 2'd0;
        projection_chunk_len <= 3'd0;
        projection_word0 <= 32'd0;
        projection_row_size_work <= 32'd0;
        projection_chunk_work <= 32'd0;
        projection_attr_work <= 8'd0;
        projection_attr_invalid_work <= 1'b0;
        compute_hit_index <= 8'd0;
        compute_word_lo <= 32'd0;
        compute_nonnull_count <= 64'd0;
        compute_null_count <= 64'd0;
        compute_sum <= 64'sd0;
        compute_min <= 64'd0;
        compute_max <= 64'd0;
        compute_initialized <= 1'b0;
        compute_overflow <= 1'b0;
        compute_flush_index <= 5'd0;
        compute_value_work <= 64'd0;
        compute_signed_work <= 64'sd0;
        compute_sum_work <= 64'sd0;
        compute_hash_work <= 32'd0;
        compute_hash_value <= 64'd0;
        compute_hash_index <= 4'd0;
        compute_attr_work <= 8'd0;
        for (i = 0; i < 16; i = i + 1) begin
            desc_len[i] <= 16'd0;
            desc_align[i] <= 16'd0;
            att_pos[i] <= 16'd0;
            att_len[i] <= 16'd0;
        end
        for (i = 0; i < 64; i = i + 1) begin
            bool_stack[i] <= 1'b0;
            clause_word0[i] <= 32'd0;
            clause_word1[i] <= 32'd0;
            clause_word2[i] <= 32'd0;
            clause_word3[i] <= 32'd0;
            clause_const1_lo[i] <= 32'd0;
            clause_const1_hi[i] <= 32'd0;
            clause_const1_valid[i] <= 1'b0;
        end
    end else begin
        m_axi_bready <= 1'b1;

        case ({write_issue_complete, write_response_fire})
            2'b10: write_outstanding <= write_outstanding + 7'd1;
            2'b01: begin
                if (write_outstanding != 7'd0)
                    write_outstanding <= write_outstanding - 7'd1;
            end
            default: write_outstanding <= write_outstanding;
        endcase

        if (clear_req && state == ST_IDLE) begin
            status_reg <= STATUS_DONE;
            error_reg <= ERR_NONE;
            hit_count_reg <= 32'd0;
            page_done_reg <= 32'd0;
        end

        if (read_pending) begin
            if (m_axi_arvalid && m_axi_arready) begin
                m_axi_arvalid <= 1'b0;
                m_axi_rready <= 1'b1;
            end
            if (m_axi_rvalid && m_axi_rready) begin
                m_axi_rready <= 1'b0;
                read_pending <= 1'b0;
                read_data <= m_axi_rdata;
                if (m_axi_rresp != 2'b00) begin
                    status_reg <= STATUS_DONE | STATUS_ERROR;
                    error_reg <= ERR_AXI_RESP;
                    state <= ST_FAIL;
                end else begin
                    state <= ret_state;
                end
            end
        end else if (write_pending) begin
            if (m_axi_awvalid && m_axi_awready)
                m_axi_awvalid <= 1'b0;
            if (m_axi_wvalid && m_axi_wready)
                m_axi_wvalid <= 1'b0;
            if (write_issue_complete) begin
                write_pending <= 1'b0;
                state <= write_ret_state;
            end
        end else begin
            case (state)
                ST_IDLE: begin
                    if (start_req) begin
                        status_reg <= STATUS_BUSY;
                        error_reg <= ERR_NONE;
                        hit_count_reg <= 32'd0;
                        page_done_reg <= 32'd0;
                        page_idx <= 8'd0;
                        current_page_addr <= reg_page_addr;
                        page_hit_count <= 32'd0;
                        tuple_count_override <= 16'd0;
                        desc_loaded <= 1'b0;
                        tuple_idx <= 8'd0;
                        attr_idx <= 8'd0;
                        scan_natts <= reg_natts[7:0];
                        clause_idx <= 8'd0;
                        load_clause_idx <= 8'd0;
                        fast_mode <= FAST_NONE;
                        fast_const_valid <= 1'b0;
                        fast_hit <= 1'b0;
                        fast_layout_active <= 1'b0;
                        fast_layout_first_off <= 16'd0;
                        fast_layout_tuple_len <= 16'd0;
                        fast_layout_tuple_span <= 16'd0;
                        fast_layout_last_expected <= 16'd0;
                        fast_pipe_issue_idx <= 8'd0;
                        fast_pipe_resp_idx <= 8'd0;
                        fast_pipe_write_idx <= 8'd0;
                        fast_pipe_hit_count <= 8'd0;
                        fast_pipe_outstanding <= 6'd0;
                        fast_pipe_issue_fire <= 1'b0;
                        fast_pipe_resp_fire <= 1'b0;
                        fast_pipe_issue_word <= 1'b0;
                        fast_pipe_resp_word <= 1'b0;
                        fast_pipe_word0 <= 32'd0;
                        fast_pipe_tuple_hit <= 1'b0;
                        fast_pipe_unsupported <= 1'b0;
                        fast_compute_attr_offset <= 16'd0;
                        projection_page_cursor <= 32'd0;
                        projection_row_base <= 32'd0;
                        projection_row_bytes <= 32'd0;
                        projection_data_cursor <= 32'd0;
                        projection_col_idx <= 8'd0;
                        projection_selected_attr <= 8'd0;
                        projection_src_addr <= 32'd0;
                        projection_dst_addr <= 32'd0;
                        projection_data_remaining <= 16'd0;
                        projection_src_sel <= 2'd0;
                        projection_chunk_len <= 3'd0;
                        projection_word0 <= 32'd0;
                        compute_hit_index <= 8'd0;
                        compute_word_lo <= 32'd0;
                        compute_nonnull_count <= 64'd0;
                        compute_null_count <= 64'd0;
                        compute_sum <= 64'sd0;
                        compute_min <= 64'd0;
                        compute_max <= 64'd0;
                        compute_initialized <= 1'b0;
                        compute_overflow <= 1'b0;
                        compute_flush_index <= 5'd0;
                        compute_hash_work <= 32'd0;
                        compute_hash_value <= 64'd0;
                        compute_hash_index <= 4'd0;
                        bool_top <= 7'd0;
                        if (reg_filter_cnt == 0 && reg_result_mode[1] &&
                            reg_projection_config[15:8] != 8'd0) begin
                            scan_natts <= reg_projection_config[15:8];
                        end else if (reg_filter_cnt == 0 &&
                                     reg_result_mode[2]) begin
                            scan_natts <= reg_compute_config[15:8] + 8'd1;
                        end
                        if (reg_natts > 16 || reg_filter_cnt > 64 ||
                            reg_page_count == 0 || reg_page_count > 64 ||
                            (reg_result_mode[1] &&
                             (reg_result_mode[0] || reg_projection_addr == 0 ||
                              reg_projection_stride < 32'd256 ||
                              reg_projection_stride[1:0] != 2'd0 ||
                              reg_projection_config[7:0] == 0 ||
                              reg_projection_config[7:0] > 8'd16 ||
                              reg_projection_config[15:8] == 8'd0 ||
                              reg_projection_config[15:8] > reg_natts[7:0] ||
                             reg_projection_config[31:16] <
                                  (16'd8 + ({8'd0, reg_projection_config[7:0]} << 3)) ||
                              {16'd0, reg_projection_config[31:16]} > reg_projection_stride))) begin
                            status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                            error_reg <= ERR_UNSUPPORTED;
                            state <= ST_FAIL;
                        end else if (reg_result_mode[2] &&
                            (reg_result_mode[1:0] != 2'd0 ||
                             reg_compute_addr == 0 ||
                             reg_compute_stride[1:0] != 2'd0 ||
                             reg_compute_stride < 32'd64 ||
                             reg_compute_config[15:8] >= reg_natts[7:0] ||
                             (reg_compute_config[3:0] == 4'd1 &&
                              (reg_compute_config[23:16] == 8'd0 ||
                               reg_compute_config[23:19] != 5'd0 ||
                               !((reg_compute_config[7:4] == 4'd1) ||
                                 ((reg_compute_config[7:4] == 4'd2 ||
                                   reg_compute_config[7:4] == 4'd3) &&
                                  !reg_compute_config[16])))) ||
                             (reg_compute_config[3:0] == 4'd2 &&
                              (reg_compute_stride < 32'd528 ||
                               !(reg_compute_config[7:4] == 4'd1 ||
                                 reg_compute_config[7:4] == 4'd2))) ||
                             (reg_compute_config[3:0] != 4'd1 &&
                              reg_compute_config[3:0] != 4'd2))) begin
                            status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                            error_reg <= ERR_COMPUTE_CONFIG;
                            state <= ST_FAIL;
                        end else begin
                            if (reg_filter_cnt == 0) begin
                                if (!reg_result_mode[1] && !reg_result_mode[2])
                                    fast_mode <= FAST_MATCH_ALL;
                                state <= ST_PAGE_START;
                            end else begin
                                load_clause_idx <= 8'd0;
                                start_read32(reg_filter_addr, ST_LOAD_CLAUSE0);
                            end
                        end
                    end
                end

                ST_LOAD_CLAUSE0: begin
                    clause_word0[load_clause_idx] <= read_data;
                    start_read32(reg_filter_addr + ({24'd0, load_clause_idx} << 4) + 32'd4, ST_LOAD_CLAUSE1);
                end

                ST_LOAD_CLAUSE1: begin
                    clause_word1[load_clause_idx] <= read_data;
                    start_read32(reg_filter_addr + ({24'd0, load_clause_idx} << 4) + 32'd8, ST_LOAD_CLAUSE2);
                end

                ST_LOAD_CLAUSE2: begin
                    clause_word2[load_clause_idx] <= read_data;
                    start_read32(reg_filter_addr + ({24'd0, load_clause_idx} << 4) + 32'd12, ST_LOAD_CLAUSE3);
                end

                ST_LOAD_CLAUSE3: begin
                    clause_word3[load_clause_idx] <= read_data;
                    clause_const1_valid[load_clause_idx] <= 1'b0;
                    if (clause_word0[load_clause_idx][7:0] == T_OPEXPR &&
                        clause_word2[load_clause_idx][23:16] == T_CONST &&
                        clause_word0[load_clause_idx][15:8] == C_INT &&
                        read_data[15:0] < 16'd128) begin
                        start_read32(reg_filter_addr + 32'd1536 + ({16'd0, read_data[15:0]} << 2), ST_LOAD_CONST1);
                    end else if (clause_word0[load_clause_idx][7:0] == T_OPEXPR &&
                        clause_word2[load_clause_idx][23:16] == T_CONST &&
                        clause_word0[load_clause_idx][15:8] == C_FLOAT8 &&
                        read_data[15:0] < 16'd64) begin
                        start_read32(reg_filter_addr + 32'd1024 + ({16'd0, read_data[15:0]} << 3), ST_LOAD_CONST1);
                    end else if (clause_word0[load_clause_idx][7:0] == T_OPEXPR &&
                        clause_word2[load_clause_idx][23:16] == T_CONST &&
                        clause_word0[load_clause_idx][15:8] == C_STRING &&
                        read_data[31:16] <= 16'd8 &&
                        read_data[15:0] <= 16'd1016 &&
                        read_data[1:0] == 2'd0) begin
                        start_read32(reg_filter_addr + 32'd2048 + {16'd0, read_data[15:0]},
                                     ST_LOAD_STR_CONST0);
                    end else begin
                        state <= ST_LOAD_CLAUSE4;
                    end
                end

                ST_LOAD_CONST1: begin
                    clause_const1_lo[load_clause_idx] <= read_data;
                    if (clause_word0[load_clause_idx][15:8] == C_FLOAT8) begin
                        start_read32(reg_filter_addr + 32'd1024 + ({16'd0, clause_word3[load_clause_idx][15:0]} << 3) + 32'd4,
                                     ST_LOAD_CONST2);
                    end else begin
                        clause_const1_valid[load_clause_idx] <= 1'b1;
                        state <= ST_LOAD_CLAUSE4;
                    end
                end

                ST_LOAD_CONST2: begin
                    clause_const1_hi[load_clause_idx] <= read_data;
                    clause_const1_valid[load_clause_idx] <= 1'b1;
                    state <= ST_LOAD_CLAUSE4;
                end

                ST_LOAD_STR_CONST0: begin
                    fast_const_lo <= read_data;
                    start_read32(reg_filter_addr + 32'd2048 + {16'd0, clause_word3[load_clause_idx][15:0]} + 32'd4,
                                 ST_LOAD_STR_CONST1);
                end

                ST_LOAD_STR_CONST1: begin
                    fast_const_hi <= read_data;
                    fast_const_valid <= 1'b1;
                    state <= ST_LOAD_CLAUSE4;
                end

                ST_LOAD_CLAUSE4: begin
                    if ((load_clause_idx + 8'd1) >= reg_filter_cnt[7:0]) begin
                        if (!reg_result_mode[1] && !reg_result_mode[2] &&
                            (reg_filter_cnt[7:0] == 8'd1 || fast_wrapped_single_clause) &&
                            clause_word0[0][7:0] == T_OPEXPR &&
                            clause_word1[0][7:0] == 8'd2 &&
                            clause_word1[0][15:8] == T_VAR &&
                            clause_word2[0][23:16] == T_CONST) begin
                            if (clause_word0[0][15:8] == C_INT &&
                                (clause_word0[0][31:16] == INT4_LT ||
                                 clause_word0[0][31:16] == DATE_LT) &&
                                clause_word1[0][19:16] == 4'd1 &&
                                clause_const1_valid[0]) begin
                                fast_mode <= FAST_INT_SCORE_LT;
                            end else if (clause_word0[0][15:8] == C_FLOAT8 &&
                                clause_word0[0][31:16] == FLOAT8_LT &&
                                clause_word1[0][19:16] == 4'd2 &&
                                clause_const1_valid[0]) begin
                                fast_mode <= FAST_FLOAT_SCORE_LT;
                            end else if (clause_word0[0][15:8] == C_STRING &&
                                (clause_word0[0][31:16] == TEXT_EQ ||
                                 clause_word0[0][31:16] == BPCHAR_EQ) &&
                                clause_word1[0][19:16] == 4'd3 &&
                                clause_word3[0][31:16] <= 16'd7 &&
                                fast_const_valid) begin
                                fast_mode <= FAST_STRING_TAG_EQ;
                            end else begin
                                fast_mode <= FAST_NONE;
                            end
                        end else begin
                            fast_mode <= FAST_NONE;
                        end
                        state <= ST_PAGE_START;
                    end else begin
                        load_clause_idx <= load_clause_idx + 8'd1;
                        start_read32(reg_filter_addr + ({24'd0, load_clause_idx + 8'd1} << 4), ST_LOAD_CLAUSE0);
                    end
                end

                ST_PAGE_START: begin
                    current_page_addr <= reg_page_addr + ({24'd0, page_idx} * reg_page_stride);
                    page_hit_count <= 32'd0;
                    projection_page_cursor <= 32'd0;
                    compute_hit_index <= 8'd0;
                    compute_nonnull_count <= 64'd0;
                    compute_null_count <= 64'd0;
                    compute_sum <= 64'sd0;
                    compute_min <= 64'd0;
                    compute_max <= 64'd0;
                    compute_initialized <= 1'b0;
                    compute_overflow <= 1'b0;
                    compute_flush_index <= 5'd0;
                    tuple_idx <= 8'd0;
                    attr_idx <= 8'd0;
                    clause_idx <= 8'd0;
                    bool_top <= 7'd0;
                    if (reg_page_addr_table_addr != 0) begin
                        start_read32(reg_page_addr_table_addr + ({24'd0, page_idx} << 2),
                                     ST_READ_PAGE_ADDR);
                    end else if (reg_page_count > 1 && reg_tuple_cnt_addr != 0) begin
                        start_read32(reg_tuple_cnt_addr + ({24'd0, page_idx} << 2), ST_READ_TUPLE_CNT);
                    end else begin
                        tuple_count_override <= reg_tuple_cnt[15:0];
                        start_read32(reg_page_addr + ({24'd0, page_idx} * reg_page_stride) + 32'd12, ST_READ_HDR);
                    end
                end

                ST_READ_PAGE_ADDR: begin
                    current_page_addr <= read_data;
                    if (reg_page_count > 1 && reg_tuple_cnt_addr != 0) begin
                        start_read32(reg_tuple_cnt_addr + ({24'd0, page_idx} << 2), ST_READ_TUPLE_CNT);
                    end else begin
                        tuple_count_override <= reg_tuple_cnt[15:0];
                        start_read32(read_data + 32'd12, ST_READ_HDR);
                    end
                end

                ST_READ_TUPLE_CNT: begin
                    tuple_count_override <= read_data[15:0];
                    start_read32(current_page_addr + 32'd12, ST_READ_HDR);
                end

                ST_READ_HDR: begin
                    page_lower <= read_data[15:0];
                    if (tuple_count_override != 0) begin
                        tuple_count <= tuple_count_override;
                    end else if (read_data[15:0] > 16'd24) begin
                        tuple_count <= (read_data[15:0] - 16'd24) >> 2;
                    end else begin
                        tuple_count <= 16'd0;
                    end

                    if ((tuple_count_override > 128) ||
                        (tuple_count_override == 0 && read_data[15:0] > 16'd24 &&
                         (((read_data[15:0] - 16'd24) >> 2) > 128))) begin
                        status_reg <= STATUS_DONE | STATUS_ERROR;
                        error_reg <= ERR_BAD_TUPLE_COUNT;
                        state <= ST_FAIL;
                    end else if (fast_mode == FAST_MATCH_ALL) begin
                        tuple_idx <= 8'd0;
                        state <= ST_MATCH_ALL_BURST_INIT;
                    end else if (fast_mode != FAST_NONE) begin
                        tuple_idx <= 8'd0;
                        state <= ST_FAST_LAYOUT_FIRST;
                    end else if (desc_loaded) begin
                        tuple_idx <= 8'd0;
                        state <= ST_TUPLE_ITEM;
                    end else begin
                        attr_idx <= 8'd0;
                        start_read32(reg_tupdesc_addr, ST_READ_TUPDESC_LEN);
                    end
                end

                ST_READ_TUPDESC_LEN: begin
                    desc_len[attr_idx] <= read_data[15:0];
                    if ((attr_idx + 8'd1) < scan_natts)
                        desc_len[attr_idx + 8'd1] <= read_data[31:16];

                    if ((attr_idx + 8'd2) < scan_natts) begin
                        attr_idx <= attr_idx + 8'd2;
                        start_read32(reg_tupdesc_addr + (({24'd0, attr_idx} + 32'd2) << 1), ST_READ_TUPDESC_LEN);
                    end else begin
                        attr_idx <= 8'd0;
                        start_read32(reg_tupdesc_addr + 32'd32, ST_READ_TUPDESC_ALN);
                    end
                end

                ST_READ_TUPDESC_ALN: begin
                    desc_align[attr_idx] <= read_data[15:0];
                    if ((attr_idx + 8'd1) < scan_natts)
                        desc_align[attr_idx + 8'd1] <= read_data[31:16];

                    if ((attr_idx + 8'd2) < scan_natts) begin
                        attr_idx <= attr_idx + 8'd2;
                        start_read32(reg_tupdesc_addr + 32'd32 + (({24'd0, attr_idx} + 32'd2) << 1), ST_READ_TUPDESC_ALN);
                    end else begin
                        desc_loaded <= 1'b1;
                        tuple_idx <= 8'd0;
                        if (reg_filter_cnt == 0 && reg_result_mode[2] &&
                            reg_compute_config[3:0] == 4'd1 &&
                            reg_compute_config[7:4] == 4'd1) begin
                            attr_idx <= 8'd0;
                            fast_compute_offset_work <= 16'd24;
                            state <= ST_FAST_COMP_LAYOUT_PREP;
                        end else begin
                            state <= ST_TUPLE_ITEM;
                        end
                    end
                end

                ST_FAST_COMP_LAYOUT_PREP: begin
                    aligned_pos = (fast_compute_offset_work +
                                   desc_align[attr_idx]) &
                                  ~desc_align[attr_idx];
                    if (attr_idx >= reg_natts[7:0] ||
                        desc_len[attr_idx] <= 0 ||
                        desc_align[attr_idx] > 16'd7) begin
                        tuple_idx <= 8'd0;
                        state <= ST_TUPLE_ITEM;
                    end else if (attr_idx == reg_compute_config[15:8]) begin
                        tuple_idx <= 8'd0;
                        if (desc_len[attr_idx] == 16'sd4) begin
                            fast_compute_attr_offset <= aligned_pos;
                            fast_mode <= FAST_COMPUTE_INT32;
                            state <= ST_FAST_LAYOUT_FIRST;
                        end else begin
                            state <= ST_TUPLE_ITEM;
                        end
                    end else begin
                        fast_compute_offset_work <=
                            aligned_pos + desc_len[attr_idx];
                        attr_idx <= attr_idx + 8'd1;
                    end
                end

                ST_MATCH_ALL_BURST_INIT: begin
                    fast_pipe_resp_idx <= 8'd0;
                    fast_pipe_write_idx <= 8'd0;
                    fast_pipe_hit_count <= 8'd0;
                    if (tuple_count == 16'd0) begin
                        state <= ST_DONE;
                    end else begin
                        m_axi_araddr <= current_page_addr + 32'd24;
                        read_burst_len <= tuple_count[7:0] - 8'd1;
                        m_axi_arvalid <= 1'b1;
                        m_axi_rready <= 1'b0;
                        state <= ST_MATCH_ALL_BURST_RUN;
                    end
                end

                ST_MATCH_ALL_BURST_RUN: begin
                    if (m_axi_arvalid && m_axi_arready) begin
                        m_axi_arvalid <= 1'b0;
                        m_axi_rready <= 1'b1;
                    end
                    if (m_axi_rvalid && m_axi_rready) begin
                        if (m_axi_rresp != 2'b00 ||
                            (m_axi_rlast !=
                             ({8'd0, fast_pipe_resp_idx + 8'd1} ==
                              tuple_count))) begin
                            m_axi_arvalid <= 1'b0;
                            read_burst_len <= 8'd0;
                            error_reg <= ERR_AXI_RESP;
                            if (m_axi_rlast) begin
                                m_axi_rready <= 1'b0;
                                status_reg <= STATUS_DONE | STATUS_ERROR;
                                state <= ST_FAIL;
                            end else begin
                                status_reg <= STATUS_BUSY | STATUS_ERROR;
                                state <= ST_MATCH_ALL_BURST_DRAIN;
                            end
                        end else begin
                            if (m_axi_rdata[16:15] == LP_NORMAL &&
                                m_axi_rdata[14:0] >= page_lower &&
                                m_axi_rdata[31:17] != 15'd0 &&
                                ({1'b0, m_axi_rdata[14:0]} +
                                 {2'd0, m_axi_rdata[31:17]}) <=
                                    {1'b0, PG_PAGE_SIZE_BYTES}) begin
                                if (!reg_result_mode[0])
                                    fast_pipe_hit_desc[fast_pipe_hit_count] <=
                                        m_axi_rdata;
                                fast_pipe_hit_count <=
                                    fast_pipe_hit_count + 8'd1;
                                hit_count_reg <= hit_count_reg + 32'd1;
                                page_hit_count <= page_hit_count + 32'd1;
                            end
                            fast_pipe_resp_idx <= fast_pipe_resp_idx + 8'd1;
                            if (m_axi_rlast) begin
                                m_axi_rready <= 1'b0;
                                read_burst_len <= 8'd0;
                                if (reg_result_mode[0]) begin
                                    state <= ST_DONE;
                                end else begin
                                    fast_pipe_write_idx <= 8'd0;
                                    state <= ST_FAST_INT_PIPE_WRITE;
                                end
                            end
                        end
                    end
                end

                ST_MATCH_ALL_BURST_DRAIN: begin
                    m_axi_arvalid <= 1'b0;
                    m_axi_rready <= 1'b1;
                    if (m_axi_rvalid && m_axi_rready && m_axi_rlast) begin
                        m_axi_rready <= 1'b0;
                        status_reg <= STATUS_DONE | STATUS_ERROR;
                        state <= ST_FAIL;
                    end
                end

                ST_FAST_LAYOUT_FIRST: begin
                    fast_layout_active <= 1'b0;
                    if (tuple_count == 16'd0) begin
                        state <= (fast_mode == FAST_COMPUTE_INT32) ?
                            ST_COMP_FLUSH_INIT : ST_DONE;
                    end else begin
                        start_read32(current_page_addr + 32'd24, ST_FAST_LAYOUT_LAST);
                    end
                end

                ST_FAST_LAYOUT_LAST: begin
                    aligned_pos = ({1'b0, read_data[31:17]} + 16'd7) & 16'hfff8;
                    if (read_data[16:15] != LP_NORMAL ||
                        read_data[14:0] < page_lower ||
                        read_data[31:17] == 15'd0 ||
                         ({1'b0, read_data[14:0]} + {2'd0, read_data[31:17]}) > {1'b0, PG_PAGE_SIZE_BYTES}) begin
                        tuple_idx <= 8'd0;
                        if (fast_mode == FAST_COMPUTE_INT32) begin
                            fast_mode <= FAST_NONE;
                            state <= ST_TUPLE_ITEM;
                        end else begin
                            state <= ST_FAST_TUPLE_ITEM;
                        end
                    end else begin
                        fast_layout_first_off <= read_data[14:0];
                        fast_layout_tuple_len <= {1'b0, read_data[31:17]};
                        fast_layout_tuple_span <= aligned_pos;
                        fast_layout_last_expected <= read_data[14:0] -
                            ((tuple_count - 16'd1) * aligned_pos);
                        tuple_idx <= 8'd0;
                        if (tuple_count == 16'd1) begin
                            start_read32(current_page_addr + {17'd0, read_data[14:0]} + 32'd20,
                                         ST_FAST_LAYOUT_TUPLE);
                        end else begin
                            start_read32(current_page_addr + 32'd24 + (({16'd0, tuple_count} - 32'd1) << 2),
                                         ST_FAST_LAYOUT_HDR);
                        end
                    end
                end

                ST_FAST_LAYOUT_HDR: begin
                    if (read_data[16:15] != LP_NORMAL ||
                        read_data[14:0] != fast_layout_last_expected ||
                        {1'b0, read_data[31:17]} != fast_layout_tuple_len ||
                         ({1'b0, read_data[14:0]} + {1'b0, fast_layout_tuple_len}) > {1'b0, PG_PAGE_SIZE_BYTES}) begin
                        tuple_idx <= 8'd0;
                        if (fast_mode == FAST_COMPUTE_INT32) begin
                            fast_mode <= FAST_NONE;
                            state <= ST_TUPLE_ITEM;
                        end else begin
                            state <= ST_FAST_TUPLE_ITEM;
                        end
                    end else begin
                        start_read32(current_page_addr + {16'd0, fast_layout_first_off} + 32'd20,
                                     ST_FAST_LAYOUT_TUPLE);
                    end
                end

                ST_FAST_LAYOUT_TUPLE: begin
                    if (tuple_idx == 8'd0 && read_data[23:16] != 8'd24) begin
                        fast_layout_active <= 1'b0;
                        if (fast_mode == FAST_COMPUTE_INT32) begin
                            fast_mode <= FAST_NONE;
                            state <= ST_TUPLE_ITEM;
                        end else begin
                            state <= ST_FAST_TUPLE_ITEM;
                        end
                    end else if ({8'd0, tuple_idx} >= tuple_count) begin
                        fast_layout_active <= 1'b0;
                        state <= (fast_mode == FAST_COMPUTE_INT32) ?
                            ST_COMP_FLUSH_INIT : ST_DONE;
                    end else if (fast_mode == FAST_COMPUTE_INT32 &&
                                 (fast_compute_attr_offset + 16'd4) >
                                     fast_layout_tuple_len) begin
                        status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                        error_reg <= ERR_UNSUPPORTED;
                        state <= ST_FAIL;
                    end else if (fast_mode != FAST_NONE && tuple_idx == 8'd0) begin
                        fast_layout_active <= 1'b1;
                        tuple_off <= fast_layout_first_off;
                        tuple_len <= fast_layout_tuple_len;
                        fast_hit <= 1'b0;
                        state <= ST_FAST_INT_PIPE_INIT;
                    end else begin
                        fast_layout_active <= 1'b1;
                        tuple_off <= fast_layout_first_off -
                            ({8'd0, tuple_idx} * fast_layout_tuple_span);
                        tuple_len <= fast_layout_tuple_len;
                        fast_hit <= 1'b0;
                        if (fast_mode == FAST_INT_SCORE_LT) begin
                            start_read32(current_page_addr +
                                         {16'd0, fast_layout_first_off -
                                          ({8'd0, tuple_idx} * fast_layout_tuple_span) + 16'd28},
                                         ST_FAST_INT_READ);
                        end else if (fast_mode == FAST_FLOAT_SCORE_LT) begin
                            arg0_addr <= current_page_addr +
                                {16'd0, fast_layout_first_off -
                                 ({8'd0, tuple_idx} * fast_layout_tuple_span) + 16'd32};
                            start_read32(current_page_addr +
                                         {16'd0, fast_layout_first_off -
                                          ({8'd0, tuple_idx} * fast_layout_tuple_span) + 16'd32},
                                         ST_FAST_FLOAT_LO);
                        end else if (fast_mode == FAST_STRING_TAG_EQ) begin
                            start_read32((current_page_addr +
                                          {16'd0, fast_layout_first_off -
                                           ({8'd0, tuple_idx} * fast_layout_tuple_span) + 16'd40}) &
                                         32'hfffffffc,
                                         ST_FAST_LAYOUT_STR0);
                        end else begin
                            state <= ST_FAST_NEXT_TUPLE;
                        end
                    end
                end

                ST_FAST_LAYOUT_STR0: begin
                    var_first_byte = read_data[7:0];
                    if (!var_first_byte[0]) begin
                        status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                        error_reg <= ERR_UNSUPPORTED;
                        state <= ST_FAIL;
                    end else begin
                        var_total_len = {9'd0, var_first_byte[7:1]};
                        var_meta_len = 16'd1;
                        fast_tag_len <= var_total_len - var_meta_len;
                        fast_tag_sel <= 2'd1;
                        if (var_total_len < var_meta_len ||
                            (var_total_len - var_meta_len) != clause_word3[0][31:16]) begin
                            fast_hit <= 1'b0;
                            state <= ST_FAST_HIT_CHECK;
                        end else if ((var_total_len - var_meta_len) == 16'd0) begin
                            fast_hit <= 1'b1;
                            state <= ST_FAST_HIT_CHECK;
                        end else if ((16'd1 + (var_total_len - var_meta_len)) > 16'd4) begin
                            str0_word0 <= read_data;
                            start_read32((current_page_addr + {16'd0, tuple_off + 16'd40}) + 32'd4,
                                         ST_FAST_TAG_DATA1);
                        end else begin
                            str0_word0 = read_data;
                            str0_word1 = 32'd0;
                            fast_hit <= string_eq_upto8_no_cross(str0_word0, str0_word1,
                                                                 2'd1, var_total_len - var_meta_len,
                                                                 fast_const_lo, fast_const_hi);
                            state <= ST_FAST_HIT_CHECK;
                        end
                    end
                end

                ST_FAST_INT_PIPE_INIT: begin
                    fast_pipe_issue_idx <= 8'd0;
                    fast_pipe_resp_idx <= 8'd0;
                    fast_pipe_write_idx <= 8'd0;
                    fast_pipe_hit_count <= 8'd0;
                    fast_pipe_outstanding <= 6'd0;
                    fast_pipe_issue_fire <= 1'b0;
                    fast_pipe_resp_fire <= 1'b0;
                    fast_pipe_issue_word <= 1'b0;
                    fast_pipe_resp_word <= 1'b0;
                    fast_pipe_word0 <= 32'd0;
                    fast_pipe_tuple_hit <= 1'b0;
                    fast_pipe_unsupported <= 1'b0;
                    m_axi_arvalid <= 1'b0;
                    m_axi_rready <= 1'b1;
                    state <= ST_FAST_INT_PIPE_RUN;
                end

                ST_FAST_INT_PIPE_RUN: begin
                    fast_pipe_issue_fire = 1'b0;
                    fast_pipe_resp_fire = 1'b0;

                    if (m_axi_arvalid && m_axi_arready) begin
                        m_axi_arvalid <= 1'b0;
                        if (!fast_pipe_two_word_mode || fast_pipe_issue_word) begin
                            fast_pipe_issue_idx <= fast_pipe_issue_idx + 8'd1;
                            fast_pipe_issue_word <= 1'b0;
                        end else begin
                            fast_pipe_issue_word <= 1'b1;
                        end
                        fast_pipe_issue_fire = 1'b1;
                    end

                    if (m_axi_rvalid && m_axi_rready && m_axi_rresp != 2'b00) begin
                        m_axi_arvalid <= 1'b0;
                        m_axi_rready <= 1'b0;
                        status_reg <= STATUS_DONE | STATUS_ERROR;
                        error_reg <= ERR_AXI_RESP;
                        state <= ST_FAIL;
                    end else begin
                        if (m_axi_rvalid && m_axi_rready) begin
                            fast_pipe_resp_fire = 1'b1;
                            if (fast_pipe_two_word_mode && !fast_pipe_resp_word) begin
                                fast_pipe_word0 <= m_axi_rdata;
                                fast_pipe_resp_word <= 1'b1;
                            end else begin
                                fast_pipe_tuple_hit = 1'b0;
                                if (fast_mode == FAST_INT_SCORE_LT) begin
                                    fast_pipe_tuple_hit =
                                        ($signed(m_axi_rdata) < $signed(clause_const1_lo[0]));
                                end else if (fast_mode == FAST_FLOAT_SCORE_LT) begin
                                    fast_pipe_tuple_hit =
                                        float64_lt_cmp({m_axi_rdata, fast_pipe_word0},
                                                       {clause_const1_hi[0], clause_const1_lo[0]});
                                end else if (fast_mode == FAST_STRING_TAG_EQ) begin
                                    var_first_byte = fast_pipe_word0[7:0];
                                    if (!var_first_byte[0]) begin
                                        fast_pipe_unsupported <= 1'b1;
                                    end else begin
                                        var_total_len = {9'd0, var_first_byte[7:1]};
                                        if (var_total_len >= 16'd1 &&
                                            (var_total_len - 16'd1) == clause_word3[0][31:16]) begin
                                            fast_pipe_tuple_hit =
                                                string_eq_upto8_no_cross(
                                                    fast_pipe_word0, m_axi_rdata, 2'd1,
                                                    var_total_len - 16'd1,
                                                    fast_const_lo, fast_const_hi);
                                        end
                                    end
                                end else if (fast_mode == FAST_COMPUTE_INT32) begin
                                    if (fast_pipe_word0[23:16] != 8'd24 ||
                                        fast_pipe_word0[0]) begin
                                        fast_pipe_unsupported <= 1'b1;
                                    end else begin
                                        fast_pipe_tuple_hit = 1'b1;
                                        compute_signed_work =
                                            {{32{m_axi_rdata[31]}}, m_axi_rdata};
                                        compute_nonnull_count <=
                                            compute_nonnull_count + 64'd1;
                                        if (reg_compute_config[16]) begin
                                            compute_sum_work = compute_sum +
                                                compute_signed_work;
                                            if ((compute_sum[63] ==
                                                 compute_signed_work[63]) &&
                                                (compute_sum_work[63] !=
                                                 compute_sum[63]))
                                                compute_overflow <= 1'b1;
                                            compute_sum <= compute_sum_work;
                                        end
                                        if (!compute_initialized) begin
                                            compute_min <= compute_signed_work;
                                            compute_max <= compute_signed_work;
                                            compute_initialized <= 1'b1;
                                        end else begin
                                            if (reg_compute_config[17] &&
                                                compute_signed_work <
                                                    $signed(compute_min))
                                                compute_min <= compute_signed_work;
                                            if (reg_compute_config[18] &&
                                                compute_signed_work >
                                                    $signed(compute_max))
                                                compute_max <= compute_signed_work;
                                        end
                                    end
                                end else if (fast_mode == FAST_MATCH_ALL) begin
                                    if (m_axi_rdata[16:15] == LP_NORMAL &&
                                        m_axi_rdata[14:0] >= page_lower &&
                                        m_axi_rdata[31:17] != 15'd0 &&
                                        ({1'b0, m_axi_rdata[14:0]} +
                                         {2'd0, m_axi_rdata[31:17]}) <=
                                            {1'b0, PG_PAGE_SIZE_BYTES})
                                        fast_pipe_tuple_hit = 1'b1;
                                end

                                if (fast_pipe_tuple_hit) begin
                                    if (!reg_result_mode[0]) begin
                                        if (fast_mode == FAST_MATCH_ALL)
                                            fast_pipe_hit_desc[fast_pipe_hit_count] <=
                                                m_axi_rdata;
                                        else
                                            fast_pipe_hit_desc[fast_pipe_hit_count] <=
                                                {fast_layout_tuple_len,
                                                 fast_layout_first_off -
                                                 ({8'd0, fast_pipe_resp_idx} *
                                                  fast_layout_tuple_span)};
                                    end
                                    fast_pipe_hit_count <= fast_pipe_hit_count + 8'd1;
                                    hit_count_reg <= hit_count_reg + 32'd1;
                                    page_hit_count <= page_hit_count + 32'd1;
                                end
                                fast_pipe_resp_idx <= fast_pipe_resp_idx + 8'd1;
                                fast_pipe_resp_word <= 1'b0;
                            end
                        end

                        case ({fast_pipe_issue_fire, fast_pipe_resp_fire})
                            2'b10: fast_pipe_outstanding <= fast_pipe_outstanding + 6'd1;
                            2'b01: fast_pipe_outstanding <= fast_pipe_outstanding - 6'd1;
                            default: fast_pipe_outstanding <= fast_pipe_outstanding;
                        endcase

                        if (!m_axi_arvalid &&
                            {8'd0, fast_pipe_issue_idx} < tuple_count &&
                            fast_pipe_outstanding < FAST_PIPE_MAX_OUTSTANDING) begin
                            if (fast_mode == FAST_MATCH_ALL) begin
                                m_axi_araddr <= current_page_addr + 32'd24 +
                                    ({24'd0, fast_pipe_issue_idx} << 2);
                            end else if (fast_mode == FAST_INT_SCORE_LT) begin
                                m_axi_araddr <= current_page_addr +
                                    {16'd0, fast_layout_first_off -
                                     ({8'd0, fast_pipe_issue_idx} * fast_layout_tuple_span) + 16'd28};
                            end else if (fast_mode == FAST_FLOAT_SCORE_LT) begin
                                m_axi_araddr <= current_page_addr +
                                    {16'd0, fast_layout_first_off -
                                     ({8'd0, fast_pipe_issue_idx} * fast_layout_tuple_span) +
                                     16'd32 + (fast_pipe_issue_word ? 16'd4 : 16'd0)};
                            end else if (fast_mode == FAST_STRING_TAG_EQ) begin
                                m_axi_araddr <= current_page_addr +
                                    {16'd0, fast_layout_first_off -
                                     ({8'd0, fast_pipe_issue_idx} * fast_layout_tuple_span) +
                                     16'd40 + (fast_pipe_issue_word ? 16'd4 : 16'd0)};
                            end else begin
                                m_axi_araddr <= current_page_addr +
                                    {16'd0, fast_layout_first_off -
                                     ({8'd0, fast_pipe_issue_idx} * fast_layout_tuple_span) +
                                     (fast_pipe_issue_word ?
                                      fast_compute_attr_offset : 16'd20)};
                            end
                            m_axi_arvalid <= 1'b1;
                        end

                        if ({8'd0, fast_pipe_issue_idx} >= tuple_count &&
                            {8'd0, fast_pipe_resp_idx} >= tuple_count &&
                            fast_pipe_outstanding == 6'd0 &&
                            !m_axi_arvalid) begin
                            m_axi_rready <= 1'b0;
                            if (fast_pipe_unsupported) begin
                                status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                                error_reg <= ERR_UNSUPPORTED;
                                state <= ST_FAIL;
                            end else if (reg_result_mode[0]) begin
                                state <= ST_DONE;
                            end else begin
                                fast_pipe_write_idx <= 8'd0;
                                state <= ST_FAST_INT_PIPE_WRITE;
                            end
                        end
                    end
                end

                ST_FAST_INT_PIPE_WRITE: begin
                    if (fast_pipe_write_idx >= fast_pipe_hit_count) begin
                        state <= (fast_mode == FAST_COMPUTE_INT32) ?
                            ST_COMP_FLUSH_INIT : ST_DONE;
                    end else begin
                        start_write32((reg_page_count > 1) ?
                                      (reg_hit_addr + HIT_DESC_OFFSET +
                                       ({24'd0, page_idx} * reg_hit_stride) +
                                       ({24'd0, fast_pipe_write_idx} << 2)) :
                                      (reg_hit_addr + ((hit_count_reg - {24'd0, fast_pipe_hit_count} +
                                                       {24'd0, fast_pipe_write_idx}) << 2)),
                                      fast_pipe_hit_desc[fast_pipe_write_idx],
                                      ST_FAST_INT_PIPE_WRITE_NEXT);
                    end
                end

                ST_FAST_INT_PIPE_WRITE_NEXT: begin
                    fast_pipe_write_idx <= fast_pipe_write_idx + 8'd1;
                    state <= ST_FAST_INT_PIPE_WRITE;
                end

                ST_FAST_TUPLE_ITEM: begin
                    fast_layout_active <= 1'b0;
                    if ({8'd0, tuple_idx} >= tuple_count) begin
                        state <= ST_DONE;
                    end else begin
                        start_read32(current_page_addr + 32'd24 + ({24'd0, tuple_idx} << 2), ST_FAST_TUPLE_INIT);
                    end
                end

                ST_FAST_TUPLE_INIT: begin
                    tuple_off <= read_data[14:0];
                    tuple_flags <= read_data[16:15];
                    tuple_len <= read_data[31:17];
                    fast_hit <= 1'b0;
                    if (read_data[16:15] != LP_NORMAL ||
                        read_data[14:0] < page_lower ||
                        read_data[31:17] == 15'd0 ||
                        ({1'b0, read_data[14:0]} + {2'd0, read_data[31:17]}) > {1'b0, PG_PAGE_SIZE_BYTES}) begin
                        state <= ST_FAST_NEXT_TUPLE;
                    end else begin
                        start_read32(current_page_addr + {17'd0, read_data[14:0]} + 32'd20, ST_FAST_TUPLE_HDR);
                    end
                end

                ST_FAST_TUPLE_HDR: begin
                    if (read_data[23:16] < 8'd24 ||
                        {8'd0, read_data[23:16]} > tuple_len) begin
                        state <= ST_FAST_NEXT_TUPLE;
                    end else if (fast_mode == FAST_INT_SCORE_LT) begin
                        aligned_pos = ((({8'd0, read_data[23:16]} + 16'd3) & 16'hfffc) + 16'd4);
                        if (({1'b0, aligned_pos} + 17'd4) > {1'b0, tuple_len}) begin
                            state <= ST_FAST_NEXT_TUPLE;
                        end else begin
                            start_read32(current_page_addr + {16'd0, tuple_off + aligned_pos}, ST_FAST_INT_READ);
                        end
                    end else if (fast_mode == FAST_FLOAT_SCORE_LT) begin
                        aligned_pos = ((((({8'd0, read_data[23:16]} + 16'd3) & 16'hfffc) + 16'd8) + 16'd7) & 16'hfff8);
                        if (({1'b0, aligned_pos} + 17'd8) > {1'b0, tuple_len}) begin
                            state <= ST_FAST_NEXT_TUPLE;
                        end else begin
                            arg0_addr <= current_page_addr + {16'd0, tuple_off + aligned_pos};
                            start_read32(current_page_addr + {16'd0, tuple_off + aligned_pos}, ST_FAST_FLOAT_LO);
                        end
                    end else if (fast_mode == FAST_STRING_TAG_EQ) begin
                        aligned_pos = ((((({8'd0, read_data[23:16]} + 16'd3) & 16'hfffc) + 16'd8) + 16'd7) & 16'hfff8);
                        fast_tag_header_pos <= aligned_pos + 16'd8;
                        if (({1'b0, aligned_pos} + 17'd9) > {1'b0, tuple_len}) begin
                            state <= ST_FAST_NEXT_TUPLE;
                        end else begin
                            start_read32((current_page_addr + {16'd0, tuple_off + aligned_pos + 16'd8}) & 32'hfffffffc,
                                         ST_FAST_TAG_HDR);
                        end
                    end else begin
                        state <= ST_FAST_NEXT_TUPLE;
                    end
                end

                ST_FAST_INT_READ: begin
                    arg0 = read_data;
                    arg1 = clause_const1_lo[0];
                    fast_hit <= (arg0 < arg1);
                    state <= ST_FAST_HIT_CHECK;
                end

                ST_FAST_FLOAT_LO: begin
                    arg0_float[31:0] <= read_data;
                    start_read32(arg0_addr + 32'd4, ST_FAST_FLOAT_HI);
                end

                ST_FAST_FLOAT_HI: begin
                    fast_hit <= float64_lt_cmp({read_data, arg0_float[31:0]},
                                               {clause_const1_hi[0], clause_const1_lo[0]});
                    state <= ST_FAST_HIT_CHECK;
                end

                ST_FAST_TAG_HDR: begin
                    var_byte_sel = (tuple_off + fast_tag_header_pos) & 16'd3;
                    var_first_byte = select_byte32(read_data, var_byte_sel);
                    if (!var_first_byte[0]) begin
                        status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                        error_reg <= ERR_UNSUPPORTED;
                        state <= ST_FAIL;
                    end else begin
                        var_total_len = {9'd0, var_first_byte[7:1]};
                        var_meta_len = 16'd1;
                        fast_tag_len <= var_total_len - var_meta_len;
                        fast_tag_data_pos <= fast_tag_header_pos + var_meta_len;
                        if (var_total_len < var_meta_len ||
                            ({1'b0, fast_tag_header_pos} + {1'b0, var_total_len}) > {1'b0, tuple_len}) begin
                            state <= ST_FAST_NEXT_TUPLE;
                        end else if ((var_total_len - var_meta_len) != clause_word3[0][31:16]) begin
                            fast_hit <= 1'b0;
                            state <= ST_FAST_HIT_CHECK;
                        end else if ((var_total_len - var_meta_len) == 16'd0) begin
                            fast_hit <= 1'b1;
                            state <= ST_FAST_HIT_CHECK;
                        end else begin
                            fast_tag_sel <= (tuple_off + fast_tag_header_pos + var_meta_len) & 16'd3;
                            start_read32((current_page_addr + {16'd0, tuple_off + fast_tag_header_pos + var_meta_len}) & 32'hfffffffc,
                                         ST_FAST_TAG_DATA0);
                        end
                    end
                end

                ST_FAST_TAG_DATA0: begin
                    if (fast_tag_len > 16'd4 &&
                        ((fast_tag_len - 16'd4) > (16'd4 - {14'd0, fast_tag_sel}))) begin
                        status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                        error_reg <= ERR_UNSUPPORTED;
                        state <= ST_FAIL;
                    end else if (({14'd0, fast_tag_sel} + fast_tag_len) > 16'd4) begin
                        str0_word0 <= read_data;
                        start_read32(((current_page_addr + {16'd0, tuple_off + fast_tag_data_pos}) & 32'hfffffffc) + 32'd4,
                                     ST_FAST_TAG_DATA1);
                    end else begin
                        str0_word0 = read_data;
                        str0_word1 = 32'd0;
                        fast_hit <= string_eq_upto8_no_cross(str0_word0, str0_word1,
                                                             fast_tag_sel, fast_tag_len,
                                                             fast_const_lo, fast_const_hi);
                        state <= ST_FAST_HIT_CHECK;
                    end
                end

                ST_FAST_TAG_DATA1: begin
                    str0_word1 = read_data;
                    fast_hit <= string_eq_upto8_no_cross(str0_word0, str0_word1,
                                                         fast_tag_sel, fast_tag_len,
                                                         fast_const_lo, fast_const_hi);
                    state <= ST_FAST_HIT_CHECK;
                end

                ST_FAST_HIT_CHECK: begin
                    if (fast_hit) begin
                        hit_count_reg <= hit_count_reg + 32'd1;
                        page_hit_count <= page_hit_count + 32'd1;
                        if (reg_result_mode[0]) begin
                            state <= ST_FAST_NEXT_TUPLE;
                        end else begin
                            start_write32((reg_page_count > 1) ?
                                          (reg_hit_addr + HIT_DESC_OFFSET +
                                           ({24'd0, page_idx} * reg_hit_stride) +
                                           (page_hit_count << 2)) :
                                          (reg_hit_addr + (hit_count_reg << 2)),
                                          {tuple_len, tuple_off},
                                          ST_FAST_NEXT_TUPLE);
                        end
                    end else begin
                        state <= ST_FAST_NEXT_TUPLE;
                    end
                end

                ST_FAST_NEXT_TUPLE: begin
                    tuple_idx <= tuple_idx + 8'd1;
                    state <= fast_layout_active ? ST_FAST_LAYOUT_TUPLE : ST_FAST_TUPLE_ITEM;
                end

                ST_TUPLE_ITEM: begin
                    if ({8'd0, tuple_idx} >= tuple_count) begin
                        state <= reg_result_mode[2] ? ST_COMP_FLUSH_INIT : ST_DONE;
                    end else begin
                        start_read32(current_page_addr + 32'd24 + ({24'd0, tuple_idx} << 2), ST_TUPLE_INIT);
                    end
                end

                ST_TUPLE_INIT: begin
                    tuple_off <= read_data[14:0];
                    tuple_flags <= read_data[16:15];
                    tuple_len <= read_data[31:17];
                    if (read_data[16:15] != LP_NORMAL ||
                        read_data[14:0] < page_lower ||
                        read_data[31:17] == 15'd0 ||
                        ({1'b0, read_data[14:0]} + {2'd0, read_data[31:17]}) > {1'b0, PG_PAGE_SIZE_BYTES}) begin
                        state <= ST_NEXT_TUPLE;
                    end else begin
                        start_read32(current_page_addr + {17'd0, read_data[14:0]} + 32'd20, ST_ATTR_PREP);
                    end
                end

                ST_ATTR_PREP: begin
                    if ((reg_result_mode[1] || reg_result_mode[2]) && read_data[0]) begin
                        status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                        error_reg <= ERR_UNSUPPORTED;
                        state <= ST_FAIL;
                    end else if (read_data[23:16] < 8'd23 ||
                                 {8'd0, read_data[23:16]} > tuple_len) begin
                        state <= ST_NEXT_TUPLE;
                    end else begin
                        pos_index <= read_data[23:16];
                        attr_idx <= 8'd0;
                        state <= ST_ATTR_CALC;
                    end
                end

                ST_ATTR_CALC: begin
                    if (attr_idx >= scan_natts) begin
                        state <= ST_FILTER_INIT;
                    end else begin
                        aligned_pos = (pos_index + desc_align[attr_idx]) & (~desc_align[attr_idx]);
                        if (({1'b0, aligned_pos} > {1'b0, tuple_len}) ||
                            (desc_len[attr_idx] > 0 &&
                             ({1'b0, aligned_pos} + {1'b0, desc_len[attr_idx][15:0]}) > {1'b0, tuple_len})) begin
                            state <= ST_NEXT_TUPLE;
                        end else if (desc_len[attr_idx] > 0) begin
                            att_pos[attr_idx] <= aligned_pos;
                            att_len[attr_idx] <= desc_len[attr_idx][15:0];
                            pos_index <= aligned_pos + desc_len[attr_idx][15:0];
                            attr_idx <= attr_idx + 8'd1;
                        end else begin
                            var_start_pos <= pos_index;
                            var_aligned_pos <= aligned_pos;
                            if (pos_index >= tuple_len) begin
                                state <= ST_NEXT_TUPLE;
                            end else begin
                                start_read32((current_page_addr + {16'd0, tuple_off + pos_index}) & 32'hfffffffc,
                                             ST_ATTR_VARLEN_CHECK);
                            end
                        end
                    end
                end

                ST_ATTR_VARLEN_CHECK: begin
                    var_byte_sel = (tuple_off + var_start_pos) & 16'd3;
                    if (((tuple_off + var_start_pos) & 16'd3) == 16'd0)
                        var_first_byte = read_data[7:0];
                    else if (((tuple_off + var_start_pos) & 16'd3) == 16'd1)
                        var_first_byte = read_data[15:8];
                    else if (((tuple_off + var_start_pos) & 16'd3) == 16'd2)
                        var_first_byte = read_data[23:16];
                    else
                        var_first_byte = read_data[31:24];

                    if (var_first_byte == 8'd0) begin
                        if (var_aligned_pos >= tuple_len) begin
                            state <= ST_NEXT_TUPLE;
                        end else begin
                            start_read32((current_page_addr + {16'd0, tuple_off + var_aligned_pos}) & 32'hfffffffc,
                                         ST_ATTR_VARLEN_READ);
                        end
                    end else begin
                        prepare_varlen_header(var_start_pos);
                    end
                end

                ST_ATTR_VARLEN_READ: begin
                    prepare_varlen_header(var_aligned_pos);
                end

                ST_ATTR_VARLEN_CROSS: begin
                    var_header16 = {read_data[7:0], var_first_byte};
                    finish_varlen_attr(var_data_pos, {read_data[7:0], var_first_byte});
                end

                ST_FILTER_INIT: begin
                    clause_idx <= 8'd0;
                    bool_top <= 7'd0;
                    if (reg_filter_cnt[7:0] == 8'd0) begin
                        bool_stack[0] <= 1'b1;
                        bool_top <= 7'd1;
                        state <= ST_HIT_CHECK;
                    end else begin
                        state <= ST_READ_OP0;
                    end
                end

                ST_READ_OP0: begin
                    if (clause_idx >= reg_filter_cnt[7:0]) begin
                        state <= ST_HIT_CHECK;
                    end else begin
                        op0 <= clause_word0[clause_idx];
                        op1 <= clause_word1[clause_idx];
                        op2 <= clause_word2[clause_idx];
                        op3 <= clause_word3[clause_idx];
                        state <= ST_ARG0;
                    end
                end

                ST_READ_OP1: begin
                    op0 <= read_data;
                    start_read32(reg_filter_addr + ({24'd0, clause_idx} << 4) + 32'd4, ST_READ_OP2);
                end

                ST_READ_OP2: begin
                    op1 <= read_data;
                    start_read32(reg_filter_addr + ({24'd0, clause_idx} << 4) + 32'd8, ST_READ_OP3);
                end

                ST_READ_OP3: begin
                    op2 <= read_data;
                    start_read32(reg_filter_addr + ({24'd0, clause_idx} << 4) + 32'd12, ST_ARG0);
                end

                ST_ARG0: begin
                    op_type <= op0[7:0];
                    op_class <= op0[15:8];
                    op_func_id <= op0[31:16];
                    op_nargs <= op1[7:0];
                    op_arg0_tag <= op1[15:8];
                    op_arg0_index <= op1[31:16];
                    op_arg0_len <= op2[15:0];
                    op_arg1_tag <= op2[23:16];
                    op_arg1_index <= op3[15:0];
                    op_arg1_len <= op3[31:16];

                    if (op0[7:0] == T_OPEXPR) begin
                        if (op1[7:0] != 8'd2) begin
                            status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                            error_reg <= ERR_UNSUPPORTED;
                            state <= ST_FAIL;
                        end else if (op0[15:8] == C_INT) begin
                            if (op1[15:8] != T_VAR || op1[31:16] >= reg_natts[15:0]) begin
                                status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                                error_reg <= ERR_UNSUPPORTED;
                                state <= ST_FAIL;
                            end else begin
                                start_read32(current_page_addr + {16'd0, tuple_off + att_pos[op1[19:16]]}, ST_ARG1);
                            end
                        end else if (op0[15:8] == C_FLOAT8) begin
                            if (op1[15:8] != T_VAR || op1[31:16] >= reg_natts[15:0]) begin
                                status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                                error_reg <= ERR_UNSUPPORTED;
                                state <= ST_FAIL;
                            end else begin
                                arg0_addr <= current_page_addr + {16'd0, tuple_off + att_pos[op1[19:16]]};
                                start_read32(current_page_addr + {16'd0, tuple_off + att_pos[op1[19:16]]}, ST_ARG0_FLOAT_LO);
                            end
                        end else if (op0[15:8] == C_STRING) begin
                            if ((op0[31:16] != TEXT_EQ && op0[31:16] != TEXT_NE &&
                                 op0[31:16] != BPCHAR_EQ && op0[31:16] != BPCHAR_NE) ||
                                op1[15:8] != T_VAR ||
                                op2[23:16] != T_CONST ||
                                op1[31:16] >= reg_natts[15:0] ||
                                ({1'b0, op3[15:0]} + {1'b0, op3[31:16]}) > 17'd1024) begin
                                status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                                error_reg <= ERR_UNSUPPORTED;
                                state <= ST_FAIL;
                            end else begin
                                str0_base <= current_page_addr + {16'd0, tuple_off + att_pos[op1[19:16]]};
                                str1_base <= reg_filter_addr + 32'd2048 + {16'd0, op3[15:0]};
                                str0_len <= att_len[op1[19:16]];
                                str1_len <= op3[31:16];
                                str_idx <= 16'd0;
                                str_equal <= 1'b1;
                                str_chunk_len <= string_chunk_len4(op3[31:16]);
                                if (att_len[op1[19:16]] != op3[31:16]) begin
                                    push_bool_result((op0[31:16] == TEXT_NE || op0[31:16] == BPCHAR_NE) ? 1'b1 : 1'b0);
                                end else if (op3[31:16] == 16'd0) begin
                                    push_bool_result((op0[31:16] == TEXT_EQ || op0[31:16] == BPCHAR_EQ) ? 1'b1 : 1'b0);
                                end else begin
                                    str0_sel <= current_page_addr + {16'd0, tuple_off + att_pos[op1[19:16]]};
                                    str1_sel <= (reg_filter_addr + 32'd2048 + {16'd0, op3[15:0]});
                                    start_read32((current_page_addr + {16'd0, tuple_off + att_pos[op1[19:16]]}) & 32'hfffffffc,
                                                  ST_STRING_READ0);
                                end
                            end
                        end else begin
                            status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                            error_reg <= ERR_UNSUPPORTED;
                            state <= ST_FAIL;
                        end
                    end else if (op0[7:0] == T_BOOLEXPR) begin
                        state <= ST_EVAL_BOOL;
                    end else begin
                        status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                        error_reg <= ERR_UNSUPPORTED;
                        state <= ST_FAIL;
                    end
                end

                ST_ARG1: begin
                    arg0 <= read_data;
                    if (op_arg1_tag == T_VAR) begin
                        if (op_arg1_index >= reg_natts[15:0]) begin
                            status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                            error_reg <= ERR_UNSUPPORTED;
                            state <= ST_FAIL;
                        end else begin
                            start_read32(current_page_addr + {16'd0, tuple_off + att_pos[op_arg1_index[3:0]]}, ST_EVAL_OP_READ);
                        end
                    end else if (op_arg1_tag == T_CONST) begin
                        if (op_arg1_index >= 16'd128) begin
                            status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                            error_reg <= ERR_UNSUPPORTED;
                            state <= ST_FAIL;
                        end else if (clause_const1_valid[clause_idx]) begin
                            arg1 <= clause_const1_lo[clause_idx];
                            state <= ST_EVAL_OP;
                        end else begin
                            start_read32(reg_filter_addr + 32'd1536 + ({16'd0, op_arg1_index} << 2), ST_EVAL_OP_READ);
                        end
                    end else begin
                        status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                        error_reg <= ERR_UNSUPPORTED;
                        state <= ST_FAIL;
                    end
                end

                ST_ARG0_FLOAT_LO: begin
                    arg0_float[31:0] <= read_data;
                    start_read32(arg0_addr + 32'd4, ST_ARG0_FLOAT_HI);
                end

                ST_ARG0_FLOAT_HI: begin
                    arg0_float[63:32] <= read_data;
                    if (op_arg1_tag == T_VAR) begin
                        if (op_arg1_index >= reg_natts[15:0]) begin
                            status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                            error_reg <= ERR_UNSUPPORTED;
                            state <= ST_FAIL;
                        end else begin
                            arg1_addr <= current_page_addr + {16'd0, tuple_off + att_pos[op_arg1_index[3:0]]};
                            start_read32(current_page_addr + {16'd0, tuple_off + att_pos[op_arg1_index[3:0]]}, ST_ARG1_FLOAT_LO);
                        end
                    end else if (op_arg1_tag == T_CONST) begin
                        if (op_arg1_index >= 16'd64) begin
                            status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                            error_reg <= ERR_UNSUPPORTED;
                            state <= ST_FAIL;
                        end else if (clause_const1_valid[clause_idx]) begin
                            arg1_float <= {clause_const1_hi[clause_idx], clause_const1_lo[clause_idx]};
                            state <= ST_EVAL_OP;
                        end else begin
                            start_read32(reg_filter_addr + 32'd1024 + ({16'd0, op_arg1_index} << 3), ST_ARG1_FLOAT_LO);
                        end
                    end else begin
                        status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                        error_reg <= ERR_UNSUPPORTED;
                        state <= ST_FAIL;
                    end
                end

                ST_ARG1_FLOAT_LO: begin
                    arg1_float[31:0] <= read_data;
                    if (op_arg1_tag == T_VAR)
                        start_read32(arg1_addr + 32'd4, ST_ARG1_FLOAT_HI);
                    else
                        start_read32(reg_filter_addr + 32'd1024 + ({16'd0, op_arg1_index} << 3) + 32'd4, ST_ARG1_FLOAT_HI);
                end

                ST_ARG1_FLOAT_HI: begin
                    arg1_float[63:32] <= read_data;
                    state <= ST_EVAL_OP;
                end

                ST_EVAL_OP_READ: begin
                    arg1 <= read_data;
                    state <= ST_EVAL_OP;
                end

                ST_STRING_READ0: begin
                    str0_word0 <= read_data;
                    if (({1'b0, str0_sel} + str_chunk_len) > 4) begin
                        start_read32(((str0_base + {16'd0, str_idx}) & 32'hfffffffc) + 32'd4,
                                     ST_STRING_READ0_NEXT);
                    end else begin
                        str0_word1 <= 32'd0;
                        start_read32((str1_base + {16'd0, str_idx}) & 32'hfffffffc,
                                     ST_STRING_READ1);
                    end
                end

                ST_STRING_READ0_NEXT: begin
                    str0_word1 <= read_data;
                    start_read32((str1_base + {16'd0, str_idx}) & 32'hfffffffc,
                                 ST_STRING_READ1);
                end

                ST_STRING_READ1: begin
                    str1_word0 <= read_data;
                    if (({1'b0, str1_sel} + str_chunk_len) > 4) begin
                        start_read32(((str1_base + {16'd0, str_idx}) & 32'hfffffffc) + 32'd4,
                                     ST_STRING_READ1_NEXT);
                    end else begin
                        str1_word1 <= 32'd0;
                        state <= ST_STRING_EVAL;
                    end
                end

                ST_STRING_READ1_NEXT: begin
                    str1_word1 <= read_data;
                    state <= ST_STRING_EVAL;
                end

                ST_STRING_EVAL: begin
                    str0_chunk = string_chunk32(str0_word0, str0_word1, str0_sel);
                    str1_chunk = string_chunk32(str1_word0, str1_word1, str1_sel);
                    str_chunk_mask = string_chunk_mask32(str_chunk_len);
                    str_chunk_equal = ((str0_chunk & str_chunk_mask) == (str1_chunk & str_chunk_mask));

                    if (!str_chunk_equal) begin
                        str_equal <= 1'b0;
                        push_bool_result((op_func_id == TEXT_NE || op_func_id == BPCHAR_NE) ? 1'b1 : 1'b0);
                    end else if ((str_idx + {13'd0, str_chunk_len}) >= str0_len) begin
                        push_bool_result((op_func_id == TEXT_EQ || op_func_id == BPCHAR_EQ) ? 1'b1 : 1'b0);
                    end else begin
                        str_chunk_len <= string_chunk_len4(str0_len - (str_idx + {13'd0, str_chunk_len}));
                        str_idx <= str_idx + {13'd0, str_chunk_len};
                        str0_sel <= str0_base[1:0] + str_idx[1:0] + str_chunk_len[1:0];
                        str1_sel <= str1_base[1:0] + str_idx[1:0] + str_chunk_len[1:0];
                        start_read32((str0_base + {16'd0, str_idx + {13'd0, str_chunk_len}}) & 32'hfffffffc,
                                     ST_STRING_READ0);
                    end
                end

                ST_EVAL_OP: begin
                    unsupported_func = 1'b0;
                    if (op_class == C_INT) begin
                        case (op0[31:16])
                            DATE_EQ, INT4_EQ: op_bool = (arg0 == arg1);
                            DATE_NE, INT4_NE: op_bool = (arg0 != arg1);
                            DATE_GE, INT4_GE: op_bool = (arg0 >= arg1);
                            DATE_LE, INT4_LE: op_bool = (arg0 <= arg1);
                            DATE_GT, INT4_GT: op_bool = (arg0 >  arg1);
                            DATE_LT, INT4_LT: op_bool = (arg0 <  arg1);
                            default: begin
                                op_bool = 1'b0;
                                unsupported_func = 1'b1;
                            end
                        endcase
                    end else if (op_class == C_FLOAT8) begin
                        case (op0[31:16])
                            FLOAT8_EQ: op_bool = float64_eq_cmp(arg0_float, arg1_float);
                            FLOAT8_NE: op_bool = !float64_eq_cmp(arg0_float, arg1_float);
                            FLOAT8_LT: op_bool = float64_lt_cmp(arg0_float, arg1_float);
                            FLOAT8_LE: op_bool = float64_lt_cmp(arg0_float, arg1_float) || float64_eq_cmp(arg0_float, arg1_float);
                            FLOAT8_GT: op_bool = float64_lt_cmp(arg1_float, arg0_float);
                            FLOAT8_GE: op_bool = float64_lt_cmp(arg1_float, arg0_float) || float64_eq_cmp(arg0_float, arg1_float);
                            default: begin
                                op_bool = 1'b0;
                                unsupported_func = 1'b1;
                            end
                        endcase
                    end else begin
                        op_bool = 1'b0;
                        unsupported_func = 1'b1;
                    end

                    if (unsupported_func) begin
                        status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                        error_reg <= ERR_UNSUPPORTED;
                        state <= ST_FAIL;
                    end else begin
                        push_bool_result(op_bool);
                    end
                end

                ST_EVAL_BOOL: begin
                    if (op_nargs == 0 || op_nargs > bool_top) begin
                        status_reg <= STATUS_DONE | STATUS_ERROR;
                        error_reg <= ERR_BOOL_STACK;
                        state <= ST_FAIL;
                    end else begin
                        unsupported_func = 1'b0;
                        bool_sum = 8'd0;
                        for (i = 0; i < 64; i = i + 1) begin
                            if (i < op_nargs)
                                bool_sum = bool_sum + bool_stack[bool_top - op_nargs + i];
                        end
                        if (op0[31:16] == OP_AND)
                            bool_stack[bool_top - op_nargs] <= (bool_sum == op_nargs);
                        else if (op0[31:16] == OP_OR)
                            bool_stack[bool_top - op_nargs] <= (bool_sum != 0);
                        else begin
                            unsupported_func = 1'b1;
                            status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                            error_reg <= ERR_UNSUPPORTED;
                            state <= ST_FAIL;
                        end
                        if (!unsupported_func) begin
                            bool_top <= bool_top - op_nargs + 7'd1;
                            clause_idx <= clause_idx + 8'd1;
                            state <= ST_READ_OP0;
                        end
                    end
                end

                ST_HIT_CHECK: begin
                    if (bool_top != 7'd1) begin
                        status_reg <= STATUS_DONE | STATUS_ERROR;
                        error_reg <= ERR_BOOL_STACK;
                        state <= ST_FAIL;
                    end else if (bool_stack[0]) begin
                        hit_count_reg <= hit_count_reg + 32'd1;
                        page_hit_count <= page_hit_count + 32'd1;
                        compute_hit_index <= page_hit_count[7:0];
                        if (reg_result_mode[0]) begin
                            state <= ST_NEXT_TUPLE;
                        end else begin
                            start_write32((reg_page_count > 1) ?
                                          (reg_hit_addr + HIT_DESC_OFFSET +
                                           ({24'd0, page_idx} * reg_hit_stride) +
                                           (page_hit_count << 2)) :
                                          (reg_hit_addr + (hit_count_reg << 2)),
                                          {tuple_len, tuple_off},
                                          reg_result_mode[1] ?
                                              ST_PROJ_ROW_BEGIN :
                                              (reg_result_mode[2] ?
                                               ST_COMP_PREP : ST_NEXT_TUPLE));
                        end
                    end else begin
                        state <= ST_NEXT_TUPLE;
                    end
                end

                ST_PROJ_ROW_BEGIN: begin
                    projection_row_size_work = 32'd8 +
                        ({24'd0, reg_projection_config[7:0]} << 3);
                    projection_attr_invalid_work = 1'b0;
                    for (i = 0; i < 16; i = i + 1) begin
                        if (i < reg_projection_config[7:0]) begin
                            projection_attr_work = projection_attr_index(i);
                            if (projection_attr_work >= reg_natts[7:0]) begin
                                projection_attr_invalid_work = 1'b1;
                            end else begin
                                projection_row_size_work = projection_row_size_work +
                                    (({16'd0, att_len[projection_attr_work]} + 32'd3) &
                                     32'hfffffffc);
                            end
                        end
                    end

                    if (projection_attr_invalid_work ||
                        projection_row_size_work >
                            {16'd0, reg_projection_config[31:16]}) begin
                        status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                        error_reg <= ERR_PROJECTION_CONFIG;
                        state <= ST_FAIL;
                    end else if (projection_page_cursor > reg_projection_stride ||
                                 projection_row_size_work >
                                     (reg_projection_stride - projection_page_cursor)) begin
                        status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                        error_reg <= ERR_PROJECTION_OVERFLOW;
                        state <= ST_FAIL;
                    end else begin
                        projection_row_base <= reg_projection_addr +
                            ({24'd0, page_idx} * reg_projection_stride) +
                            projection_page_cursor;
                        projection_row_bytes <= projection_row_size_work;
                        projection_data_cursor <= 32'd8 +
                            ({24'd0, reg_projection_config[7:0]} << 3);
                        projection_col_idx <= 8'd0;
                        start_write32(reg_projection_addr +
                                      ({24'd0, page_idx} * reg_projection_stride) +
                                      projection_page_cursor,
                                      projection_row_size_work,
                                      ST_PROJ_ROW_META);
                    end
                end

                ST_PROJ_ROW_META: begin
                    start_write32(projection_row_base + 32'd4,
                                  {16'd0, 8'd0, reg_projection_config[7:0]},
                                  ST_PROJ_DESC_OFFSET);
                end

                ST_PROJ_DESC_OFFSET: begin
                    if (projection_col_idx >= reg_projection_config[7:0]) begin
                        projection_col_idx <= 8'd0;
                        projection_data_cursor <= 32'd8 +
                            ({24'd0, reg_projection_config[7:0]} << 3);
                        state <= ST_PROJ_DATA_PREP;
                    end else begin
                        projection_attr_work =
                            projection_attr_index(projection_col_idx[3:0]);
                        start_write32(projection_row_base + 32'd8 +
                                      ({24'd0, projection_col_idx} << 3),
                                      projection_data_cursor,
                                      ST_PROJ_DESC_META);
                    end
                end

                ST_PROJ_DESC_META: begin
                    projection_attr_work =
                        projection_attr_index(projection_col_idx[3:0]);
                    start_write32(projection_row_base + 32'd12 +
                                  ({24'd0, projection_col_idx} << 3),
                                  {16'd0, att_len[projection_attr_work]},
                                  ST_PROJ_DESC_NEXT);
                end

                ST_PROJ_DESC_NEXT: begin
                    projection_attr_work =
                        projection_attr_index(projection_col_idx[3:0]);
                    projection_data_cursor <= projection_data_cursor +
                        (({16'd0, att_len[projection_attr_work]} + 32'd3) &
                         32'hfffffffc);
                    projection_col_idx <= projection_col_idx + 8'd1;
                    state <= ST_PROJ_DESC_OFFSET;
                end

                ST_PROJ_DATA_PREP: begin
                    if (projection_col_idx >= reg_projection_config[7:0]) begin
                        projection_page_cursor <= projection_page_cursor +
                                                  projection_row_bytes;
                        state <= ST_NEXT_TUPLE;
                    end else begin
                        projection_attr_work =
                            projection_attr_index(projection_col_idx[3:0]);
                        projection_selected_attr <= projection_attr_work;
                        if (att_len[projection_attr_work] == 16'd0) begin
                            projection_col_idx <= projection_col_idx + 8'd1;
                        end else begin
                            projection_src_addr <= current_page_addr +
                                {16'd0, tuple_off + att_pos[projection_attr_work]};
                            projection_dst_addr <= projection_row_base +
                                                   projection_data_cursor;
                            projection_data_remaining <= att_len[projection_attr_work];
                            projection_src_sel <=
                                (tuple_off + att_pos[projection_attr_work]) & 16'd3;
                            projection_chunk_len <=
                                string_chunk_len4(att_len[projection_attr_work]);
                            start_read32((current_page_addr +
                                          {16'd0, tuple_off +
                                           att_pos[projection_attr_work]}) &
                                         32'hfffffffc,
                                         ST_PROJ_DATA_READ0);
                        end
                    end
                end

                ST_PROJ_DATA_READ0: begin
                    projection_word0 <= read_data;
                    if (({1'b0, projection_src_sel} + projection_chunk_len) > 4) begin
                        start_read32((projection_src_addr & 32'hfffffffc) + 32'd4,
                                     ST_PROJ_DATA_READ1);
                    end else begin
                        projection_chunk_work =
                            string_chunk32(read_data, 32'd0, projection_src_sel) &
                            string_chunk_mask32(projection_chunk_len);
                        start_write32(projection_dst_addr,
                                      projection_chunk_work,
                                      ST_PROJ_DATA_ADVANCE);
                    end
                end

                ST_PROJ_DATA_READ1: begin
                    projection_chunk_work =
                        string_chunk32(projection_word0, read_data,
                                       projection_src_sel) &
                        string_chunk_mask32(projection_chunk_len);
                    start_write32(projection_dst_addr,
                                  projection_chunk_work,
                                  ST_PROJ_DATA_ADVANCE);
                end

                ST_PROJ_DATA_ADVANCE: begin
                    if (projection_data_remaining <= 16'd4) begin
                        projection_data_cursor <= projection_data_cursor +
                            (({16'd0, att_len[projection_selected_attr]} + 32'd3) &
                             32'hfffffffc);
                        projection_col_idx <= projection_col_idx + 8'd1;
                        state <= ST_PROJ_DATA_PREP;
                    end else begin
                        projection_src_addr <= projection_src_addr + 32'd4;
                        projection_dst_addr <= projection_dst_addr + 32'd4;
                        projection_data_remaining <= projection_data_remaining - 16'd4;
                        projection_chunk_len <=
                            string_chunk_len4(projection_data_remaining - 16'd4);
                        start_read32(((projection_src_addr + 32'd4) &
                                      32'hfffffffc),
                                     ST_PROJ_DATA_READ0);
                    end
                end

                ST_COMP_PREP: begin
                    compute_attr_work = reg_compute_config[15:8];
                    if (compute_attr_work >= reg_natts[7:0] ||
                        ((tuple_off + att_pos[compute_attr_work]) & 16'd3) != 16'd0 ||
                        (reg_compute_config[7:4] == 4'd1 &&
                         att_len[compute_attr_work] != 16'd4) ||
                        ((reg_compute_config[7:4] == 4'd2 ||
                          reg_compute_config[7:4] == 4'd3) &&
                         att_len[compute_attr_work] != 16'd8)) begin
                        status_reg <= STATUS_DONE | STATUS_UNSUPPORTED;
                        error_reg <= ERR_COMPUTE_CONFIG;
                        state <= ST_FAIL;
                    end else begin
                        start_read32(current_page_addr +
                                     {16'd0, tuple_off + att_pos[compute_attr_work]},
                                     ST_COMP_READ_LO);
                    end
                end

                ST_COMP_READ_LO: begin
                    if (reg_compute_config[7:4] == 4'd1) begin
                        if (reg_compute_config[3:0] == 4'd2) begin
                            compute_hash_work <= reg_compute_seed;
                            compute_hash_value <= {32'd0, read_data};
                            compute_hash_index <= 4'd0;
                            state <= ST_COMP_HASH_STEP;
                        end else begin
                            compute_signed_work = {{32{read_data[31]}}, read_data};
                            compute_nonnull_count <= compute_nonnull_count + 64'd1;
                            if (reg_compute_config[16]) begin
                                compute_sum_work = compute_sum + compute_signed_work;
                                if ((compute_sum[63] == compute_signed_work[63]) &&
                                    (compute_sum_work[63] != compute_sum[63]))
                                    compute_overflow <= 1'b1;
                                compute_sum <= compute_sum_work;
                            end
                            if (!compute_initialized) begin
                                compute_min <= compute_signed_work;
                                compute_max <= compute_signed_work;
                                compute_initialized <= 1'b1;
                            end else begin
                                if (reg_compute_config[17] &&
                                    compute_signed_work < $signed(compute_min))
                                    compute_min <= compute_signed_work;
                                if (reg_compute_config[18] &&
                                    compute_signed_work > $signed(compute_max))
                                    compute_max <= compute_signed_work;
                            end
                            state <= ST_NEXT_TUPLE;
                        end
                    end else begin
                        compute_word_lo <= read_data;
                        start_read32(current_page_addr +
                                     {16'd0, tuple_off +
                                      att_pos[reg_compute_config[15:8]]} + 32'd4,
                                     ST_COMP_READ_HI);
                    end
                end

                ST_COMP_READ_HI: begin
                    compute_value_work = {read_data, compute_word_lo};
                    if (reg_compute_config[3:0] == 4'd2) begin
                        compute_hash_work <= reg_compute_seed;
                        compute_hash_value <= compute_value_work;
                        compute_hash_index <= 4'd0;
                        state <= ST_COMP_HASH_STEP;
                    end else begin
                        compute_nonnull_count <= compute_nonnull_count + 64'd1;
                        if (!compute_initialized) begin
                            compute_min <= compute_value_work;
                            compute_max <= compute_value_work;
                            compute_initialized <= 1'b1;
                        end else if (reg_compute_config[7:4] == 4'd2) begin
                            if (reg_compute_config[17] &&
                                $signed(compute_value_work) < $signed(compute_min))
                                compute_min <= compute_value_work;
                            if (reg_compute_config[18] &&
                                $signed(compute_value_work) > $signed(compute_max))
                                compute_max <= compute_value_work;
                        end else begin
                            if (reg_compute_config[17] &&
                                ((float64_is_nan(compute_min) &&
                                  !float64_is_nan(compute_value_work)) ||
                                 float64_lt_cmp(compute_value_work, compute_min)))
                                compute_min <= compute_value_work;
                            if (reg_compute_config[18] &&
                                ((!float64_is_nan(compute_max) &&
                                  float64_is_nan(compute_value_work)) ||
                                 float64_lt_cmp(compute_max, compute_value_work)))
                                compute_max <= compute_value_work;
                        end
                        state <= ST_NEXT_TUPLE;
                    end
                end

                ST_COMP_HASH_STEP: begin
                    if ((reg_compute_config[7:4] == 4'd1 &&
                         compute_hash_index == 4'd4) ||
                        (reg_compute_config[7:4] != 4'd1 &&
                         compute_hash_index == 4'd8)) begin
                        start_write32(reg_compute_addr +
                                      ({24'd0, page_idx} * reg_compute_stride) +
                                      32'd16 + ({24'd0, compute_hit_index} << 2),
                                      compute_hash_next, ST_NEXT_TUPLE);
                    end else begin
                        compute_hash_work <= compute_hash_next;
                        compute_hash_index <= compute_hash_index + 4'd1;
                    end
                end

                ST_COMP_FLUSH_INIT: begin
                    compute_flush_index <= 5'd0;
                    state <= ST_COMP_FLUSH_WRITE;
                end

                ST_COMP_FLUSH_WRITE: begin
                    start_write32(reg_compute_addr +
                                  ({24'd0, page_idx} * reg_compute_stride) +
                                  ({27'd0, compute_flush_index} << 2),
                                  compute_partial_word(compute_flush_index),
                                  ST_COMP_FLUSH_NEXT);
                end

                ST_COMP_FLUSH_NEXT: begin
                    if ((reg_compute_config[3:0] == 4'd2 &&
                         compute_flush_index >= 5'd3) ||
                        (reg_compute_config[3:0] == 4'd1 &&
                         compute_flush_index >= 5'd15)) begin
                        state <= ST_DONE;
                    end else begin
                        compute_flush_index <= compute_flush_index + 5'd1;
                        state <= ST_COMP_FLUSH_WRITE;
                    end
                end

                ST_NEXT_TUPLE: begin
                    tuple_idx <= tuple_idx + 8'd1;
                    state <= ST_TUPLE_ITEM;
                end

                ST_DONE: begin
                    if (reg_page_count > 1) begin
                        if (reg_result_mode[1]) begin
                            start_write32(reg_hit_addr + 32'h00000100 +
                                          ({24'd0, page_idx} << 2),
                                          projection_page_cursor,
                                          ST_WRITE_PAGE_COUNT);
                        end else begin
                            start_write32(reg_hit_addr + ({24'd0, page_idx} << 2),
                                          page_hit_count,
                                          ST_NEXT_PAGE);
                        end
                    end else begin
                        page_done_reg <= 32'd1;
                        if (write_outstanding == 7'd0) begin
                            status_reg <= STATUS_DONE;
                            state <= ST_IDLE;
                        end else begin
                            state <= ST_WRITE_DRAIN;
                        end
                    end
                end

                ST_WRITE_PAGE_COUNT: begin
                    start_write32(reg_hit_addr + ({24'd0, page_idx} << 2),
                                  page_hit_count,
                                  ST_NEXT_PAGE);
                end

                ST_NEXT_PAGE: begin
                    page_done_reg <= {24'd0, page_idx} + 32'd1;
                    if (({24'd0, page_idx} + 32'd1) >= reg_page_count) begin
                        if (write_outstanding == 7'd0) begin
                            status_reg <= STATUS_DONE;
                            state <= ST_IDLE;
                        end else begin
                            state <= ST_WRITE_DRAIN;
                        end
                    end else begin
                        page_idx <= page_idx + 8'd1;
                        state <= ST_PAGE_START;
                    end
                end

                ST_WRITE_DRAIN: begin
                    if (write_outstanding == 7'd0 && !write_pending) begin
                        status_reg <= STATUS_DONE;
                        state <= ST_IDLE;
                    end
                end

                ST_FAIL: begin
                    if (write_outstanding == 7'd0 && !write_pending) begin
                        status_reg <= (status_reg & ~STATUS_BUSY) | STATUS_DONE;
                        state <= ST_IDLE;
                    end
                end

                default: begin
                    state <= ST_IDLE;
                end
            endcase
        end

        if (write_response_fire && m_axi_bresp != 2'b00) begin
            status_reg <= STATUS_BUSY | STATUS_ERROR;
            error_reg <= ERR_AXI_RESP;
            state <= ST_FAIL;
        end
    end
end

endmodule
