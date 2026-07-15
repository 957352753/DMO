`timescale 1ns / 1ps

module filter_accel_projection_tb;
    localparam PAGE_ADDR = 32'h00001000;
    localparam FILTER_ADDR = 32'h00003000;
    localparam TUPDESC_ADDR = 32'h00004000;
    localparam HIT_ADDR = 32'h00005000;
    localparam PROJECTION_ADDR = 32'h00006000;
    localparam PAGE2_ADDR = 32'h00009000;
    localparam BATCH_PROJECTION_ADDR = 32'h0000c000;
    localparam AGGREGATE_ADDR = 32'h0000f000;
    localparam HASH_ADDR = 32'h0000f400;
    localparam MATCH_PAGE_ADDR = 32'h00007000;
    localparam TUPLE_OFF = 16'd8000;
    localparam TUPLE_LEN = 16'd40;

    logic clk = 1'b0;
    logic resetn = 1'b0;

    logic [7:0] s_awaddr;
    logic [2:0] s_awprot = 3'd0;
    logic s_awvalid;
    wire s_awready;
    logic [31:0] s_wdata;
    logic [3:0] s_wstrb = 4'hf;
    logic s_wvalid;
    wire s_wready;
    wire [1:0] s_bresp;
    wire s_bvalid;
    logic s_bready;
    logic [7:0] s_araddr;
    logic [2:0] s_arprot = 3'd0;
    logic s_arvalid;
    wire s_arready;
    wire [31:0] s_rdata;
    wire [1:0] s_rresp;
    wire s_rvalid;
    logic s_rready;

    wire [31:0] m_awaddr;
    wire [7:0] m_awlen;
    wire [2:0] m_awsize;
    wire [1:0] m_awburst;
    wire [3:0] m_awcache;
    wire [2:0] m_awprot;
    wire m_awvalid;
    logic m_awready = 1'b1;
    wire [31:0] m_wdata;
    wire [3:0] m_wstrb;
    wire m_wlast;
    wire m_wvalid;
    logic m_wready = 1'b1;
    logic [1:0] m_bresp = 2'b00;
    logic m_bvalid;
    wire m_bready;
    wire [31:0] m_araddr;
    wire [7:0] m_arlen;
    wire [2:0] m_arsize;
    wire [1:0] m_arburst;
    wire [3:0] m_arcache;
    wire [2:0] m_arprot;
    wire m_arvalid;
    logic m_arready = 1'b1;
    logic [31:0] m_rdata;
    logic [1:0] m_rresp = 2'b00;
    logic m_rlast = 1'b1;
    logic m_rvalid;
    wire m_rready;

    byte memory [0:16'hffff];
    logic [31:0] write_address;
    logic [31:0] write_value;
    logic write_address_valid;
    logic write_value_valid;
    logic [31:0] read_address;
    logic [8:0] read_beats_remaining;

    filter_accel dut (
        .aclk(clk), .aresetn(resetn),
        .s_axi_awaddr(s_awaddr), .s_axi_awprot(s_awprot),
        .s_axi_awvalid(s_awvalid), .s_axi_awready(s_awready),
        .s_axi_wdata(s_wdata), .s_axi_wstrb(s_wstrb),
        .s_axi_wvalid(s_wvalid), .s_axi_wready(s_wready),
        .s_axi_bresp(s_bresp), .s_axi_bvalid(s_bvalid),
        .s_axi_bready(s_bready), .s_axi_araddr(s_araddr),
        .s_axi_arprot(s_arprot), .s_axi_arvalid(s_arvalid),
        .s_axi_arready(s_arready), .s_axi_rdata(s_rdata),
        .s_axi_rresp(s_rresp), .s_axi_rvalid(s_rvalid),
        .s_axi_rready(s_rready),
        .m_axi_awaddr(m_awaddr), .m_axi_awlen(m_awlen),
        .m_axi_awsize(m_awsize), .m_axi_awburst(m_awburst),
        .m_axi_awcache(m_awcache), .m_axi_awprot(m_awprot),
        .m_axi_awvalid(m_awvalid), .m_axi_awready(m_awready),
        .m_axi_wdata(m_wdata), .m_axi_wstrb(m_wstrb),
        .m_axi_wlast(m_wlast), .m_axi_wvalid(m_wvalid),
        .m_axi_wready(m_wready), .m_axi_bresp(m_bresp),
        .m_axi_bvalid(m_bvalid), .m_axi_bready(m_bready),
        .m_axi_araddr(m_araddr), .m_axi_arlen(m_arlen),
        .m_axi_arsize(m_arsize), .m_axi_arburst(m_arburst),
        .m_axi_arcache(m_arcache), .m_axi_arprot(m_arprot),
        .m_axi_arvalid(m_arvalid), .m_axi_arready(m_arready),
        .m_axi_rdata(m_rdata), .m_axi_rresp(m_rresp),
        .m_axi_rlast(m_rlast), .m_axi_rvalid(m_rvalid),
        .m_axi_rready(m_rready)
    );

    always #5 clk = ~clk;

    function automatic [31:0] load_word(input [31:0] address);
        load_word = {memory[address + 3], memory[address + 2],
                     memory[address + 1], memory[address]};
    endfunction

    task automatic store_word(input [31:0] address, input [31:0] value);
        begin
            memory[address] = value[7:0];
            memory[address + 1] = value[15:8];
            memory[address + 2] = value[23:16];
            memory[address + 3] = value[31:24];
        end
    endtask

    always_ff @(posedge clk) begin
        if (!resetn) begin
            m_rvalid <= 1'b0;
            m_rdata <= 32'd0;
            m_rlast <= 1'b0;
            m_arready <= 1'b1;
            read_address <= 32'd0;
            read_beats_remaining <= 9'd0;
        end else begin
            if (m_rvalid && m_rready) begin
                if (read_beats_remaining > 9'd1) begin
                    read_address <= read_address + 32'd4;
                    read_beats_remaining <= read_beats_remaining - 9'd1;
                    m_rdata <= load_word(read_address + 32'd4);
                    m_rlast <= (read_beats_remaining == 9'd2);
                end else begin
                    m_rvalid <= 1'b0;
                    m_rlast <= 1'b0;
                    read_beats_remaining <= 9'd0;
                    m_arready <= 1'b1;
                end
            end
            if (m_arvalid && m_arready) begin
                read_address <= m_araddr;
                read_beats_remaining <= {1'b0, m_arlen} + 9'd1;
                m_rdata <= load_word(m_araddr);
                m_rlast <= (m_arlen == 8'd0);
                m_rvalid <= 1'b1;
                m_arready <= 1'b0;
            end
        end
    end

    always_ff @(posedge clk) begin
        logic have_address;
        logic have_value;
        logic [31:0] selected_address;
        logic [31:0] selected_value;

        if (!resetn) begin
            write_address <= 32'd0;
            write_value <= 32'd0;
            write_address_valid <= 1'b0;
            write_value_valid <= 1'b0;
            m_bvalid <= 1'b0;
        end else begin
            have_address = write_address_valid || (m_awvalid && m_awready);
            have_value = write_value_valid || (m_wvalid && m_wready);
            selected_address = write_address_valid ? write_address : m_awaddr;
            selected_value = write_value_valid ? write_value : m_wdata;

            if (m_awvalid && m_awready) begin
                write_address <= m_awaddr;
                write_address_valid <= 1'b1;
            end
            if (m_wvalid && m_wready) begin
                write_value <= m_wdata;
                write_value_valid <= 1'b1;
            end
            if (!m_bvalid && have_address && have_value) begin
                store_word(selected_address, selected_value);
                write_address_valid <= 1'b0;
                write_value_valid <= 1'b0;
                m_bvalid <= 1'b1;
            end
            if (m_bvalid && m_bready)
                m_bvalid <= 1'b0;
        end
    end

    task automatic axi_write(input [7:0] address, input [31:0] value);
        begin
            @(posedge clk);
            s_awaddr <= address;
            s_wdata <= value;
            s_awvalid <= 1'b1;
            s_wvalid <= 1'b1;
            s_bready <= 1'b1;
            wait (s_bvalid);
            @(posedge clk);
            s_awvalid <= 1'b0;
            s_wvalid <= 1'b0;
            s_bready <= 1'b0;
        end
    endtask

    task automatic axi_read(input [7:0] address, output [31:0] value);
        begin
            @(posedge clk);
            s_araddr <= address;
            s_arvalid <= 1'b1;
            s_rready <= 1'b1;
            wait (s_rvalid);
            value = s_rdata;
            @(posedge clk);
            s_arvalid <= 1'b0;
            s_rready <= 1'b0;
        end
    endtask

    task automatic expect_word(input [31:0] address,
                               input [31:0] expected,
                               input string label_text);
        logic [31:0] actual;
        begin
            actual = load_word(address);
            if (actual !== expected)
                $fatal(1, "%s: expected %08x, got %08x", label_text,
                       expected, actual);
        end
    endtask

    initial begin
        logic [31:0] status;
        integer cycles;

        s_awaddr = 8'd0;
        s_awvalid = 1'b0;
        s_wdata = 32'd0;
        s_wvalid = 1'b0;
        s_bready = 1'b0;
        s_araddr = 8'd0;
        s_arvalid = 1'b0;
        s_rready = 1'b0;
        for (integer i = 0; i < 65536; i++)
            memory[i] = 8'd0;

        /* One normal PostgreSQL tuple: int4 at +24 and float8 at +32. */
        store_word(PAGE_ADDR + 12, 32'd28);
        store_word(PAGE_ADDR + 24,
                   ({16'd0, TUPLE_LEN} << 17) | (32'd1 << 15) | TUPLE_OFF);
        memory[PAGE_ADDR + TUPLE_OFF + 22] = 8'd24;
        store_word(PAGE_ADDR + TUPLE_OFF + 24, 32'h11223344);
        store_word(PAGE_ADDR + TUPLE_OFF + 32, 32'h55667788);
        store_word(PAGE_ADDR + TUPLE_OFF + 36, 32'h99aabbcc);

        /* tupdesc lengths [4,8], align masks [3,7]. */
        store_word(TUPDESC_ADDR, 32'h00080004);
        store_word(TUPDESC_ADDR + 32, 32'h00070003);

        /* Four line pointers exercise the no-predicate match-all scanner.
         * Three are normal; the unused entry must not become a hit. */
        store_word(MATCH_PAGE_ADDR + 12, 32'd40);
        store_word(MATCH_PAGE_ADDR + 24,
                   ({16'd0, TUPLE_LEN} << 17) | (32'd1 << 15) | 32'd8000);
        store_word(MATCH_PAGE_ADDR + 28,
                   ({16'd0, TUPLE_LEN} << 17) | (32'd1 << 15) | 32'd7952);
        store_word(MATCH_PAGE_ADDR + 32, 32'd0);
        store_word(MATCH_PAGE_ADDR + 36,
                   ({16'd0, TUPLE_LEN} << 17) | (32'd1 << 15) | 32'd7904);

        /* int4 attribute 0 equals constant 0. */
        store_word(FILTER_ADDR, 32'h00400071);
        store_word(FILTER_ADDR + 4, 32'h00006802);
        store_word(FILTER_ADDR + 8, 32'h00690004);
        store_word(FILTER_ADDR + 12, 32'h00040000);
        store_word(FILTER_ADDR + 1536, 32'h11223344);

        repeat (5) @(posedge clk);
        resetn = 1'b1;
        repeat (3) @(posedge clk);

        axi_write(8'h00, 32'h00000002);
        axi_write(8'h08, PAGE_ADDR);
        axi_write(8'h0c, TUPDESC_ADDR);
        axi_write(8'h10, FILTER_ADDR);
        axi_write(8'h14, 32'd0);
        axi_write(8'h18, HIT_ADDR);
        axi_write(8'h20, 32'd2);
        axi_write(8'h24, 32'd1);
        axi_write(8'h28, 32'd1);
        axi_write(8'h3c, 32'd1);
        axi_write(8'h50, 32'h00000002);
        axi_write(8'h54, PROJECTION_ADDR);
        axi_write(8'h58, 32'd8192);
        axi_write(8'h5c, (32'd64 << 16) | (32'd2 << 8) | 32'd2);
        axi_write(8'h60, 32'h00000001); /* output attrs [1,0] */
        axi_write(8'h64, 32'd0);
        axi_write(8'h68, 32'd0);
        axi_write(8'h6c, 32'd0);
        axi_write(8'h00, 32'h00000001);

        status = 32'd0;
        for (cycles = 0; cycles < 20000 && !status[0]; cycles++) begin
            axi_read(8'h04, status);
        end
        if (!status[0] || status[3:2] != 2'b00)
            $fatal(1, "accelerator status %08x after %0d cycles", status, cycles);

        axi_read(8'h2c, status);
        if (status != 32'd1)
            $fatal(1, "hit count expected 1, got %0d", status);

        expect_word(HIT_ADDR, {TUPLE_LEN, TUPLE_OFF}, "hit descriptor");
        expect_word(PROJECTION_ADDR, 32'd36, "row bytes");
        expect_word(PROJECTION_ADDR + 4, 32'd2, "column count");
        expect_word(PROJECTION_ADDR + 8, 32'd24, "column 0 offset");
        expect_word(PROJECTION_ADDR + 12, 32'd8, "column 0 length");
        expect_word(PROJECTION_ADDR + 16, 32'd32, "column 1 offset");
        expect_word(PROJECTION_ADDR + 20, 32'd4, "column 1 length");
        expect_word(PROJECTION_ADDR + 24, 32'h55667788, "float8 low");
        expect_word(PROJECTION_ADDR + 28, 32'h99aabbcc, "float8 high");
        expect_word(PROJECTION_ADDR + 32, 32'h11223344, "int4 value");

        /* No predicate is SQL match-all, including the FPGA projection path. */
        axi_write(8'h00, 32'h00000002);
        axi_write(8'h24, 32'd0);
        axi_write(8'h54, PROJECTION_ADDR + 32'h100);
        axi_write(8'h00, 32'h00000001);
        status = 32'd0;
        for (cycles = 0; cycles < 20000 && !status[0]; cycles++) begin
            axi_read(8'h04, status);
        end
        if (!status[0] || status[3:2] != 2'b00)
            $fatal(1, "zero-predicate accelerator status %08x", status);
        axi_read(8'h2c, status);
        if (status != 32'd1)
            $fatal(1, "zero-predicate hit count expected 1, got %0d", status);
        expect_word(PROJECTION_ADDR + 32'h100, 32'd36,
                    "zero-predicate projected row");
        axi_write(8'h24, 32'd1);

        /* Two-page SG run also verifies that consumed address-table entries
         * are safely reused for actual projection byte counts. */
        for (integer i = 0; i < 8192; i++)
            memory[PAGE2_ADDR + i] = memory[PAGE_ADDR + i];
        store_word(HIT_ADDR, 32'd1);
        store_word(HIT_ADDR + 4, 32'd1);
        store_word(HIT_ADDR + 32'h100, PAGE_ADDR);
        store_word(HIT_ADDR + 32'h104, PAGE2_ADDR);

        axi_write(8'h00, 32'h00000002);
        axi_write(8'h18, HIT_ADDR);
        axi_write(8'h38, 32'd0);
        axi_write(8'h3c, 32'd2);
        axi_write(8'h40, 32'h00000400);
        axi_write(8'h44, HIT_ADDR);
        axi_write(8'h4c, HIT_ADDR + 32'h100);
        axi_write(8'h54, BATCH_PROJECTION_ADDR);
        axi_write(8'h00, 32'h00000001);

        status = 32'd0;
        for (cycles = 0; cycles < 40000 && !status[0]; cycles++) begin
            axi_read(8'h04, status);
        end
        if (!status[0] || status[3:2] != 2'b00)
            $fatal(1, "batch accelerator status %08x after %0d cycles",
                   status, cycles);

        expect_word(HIT_ADDR, 32'd1, "batch page 0 hit count");
        expect_word(HIT_ADDR + 4, 32'd1, "batch page 1 hit count");
        expect_word(HIT_ADDR + 32'h100, 32'd36,
                    "batch page 0 projection bytes");
        expect_word(HIT_ADDR + 32'h104, 32'd36,
                    "batch page 1 projection bytes");
        expect_word(HIT_ADDR + 32'h200, {TUPLE_LEN, TUPLE_OFF},
                    "batch page 0 descriptor");
        expect_word(HIT_ADDR + 32'h600, {TUPLE_LEN, TUPLE_OFF},
                    "batch page 1 descriptor");
        expect_word(BATCH_PROJECTION_ADDR, 32'd36,
                    "batch page 0 projected row");
        expect_word(BATCH_PROJECTION_ADDR + 32'h2000, 32'd36,
                    "batch page 1 projected row");

        /* Match-all count bypasses tuple and attribute parsing. */
        axi_write(8'h00, 32'h00000002);
        axi_write(8'h08, MATCH_PAGE_ADDR);
        axi_write(8'h24, 32'd0);
        axi_write(8'h28, 32'd4);
        axi_write(8'h38, 32'd8192);
        axi_write(8'h3c, 32'd1);
        axi_write(8'h44, 32'd0);
        axi_write(8'h4c, 32'd0);
        axi_write(8'h50, 32'h00000001);
        axi_write(8'h00, 32'h00000001);

        status = 32'd0;
        for (cycles = 0; cycles < 20000 && !status[0]; cycles++) begin
            axi_read(8'h04, status);
        end
        if (!status[0] || status[3:2] != 2'b00)
            $fatal(1, "match-all accelerator status %08x", status);
        axi_read(8'h2c, status);
        if (status != 32'd3)
            $fatal(1, "match-all hit count expected 3, got %0d", status);

        /* Descriptor mode preserves PostgreSQL line-pointer order. */
        axi_write(8'h00, 32'h00000002);
        axi_write(8'h50, 32'h00000000);
        axi_write(8'h00, 32'h00000001);
        status = 32'd0;
        for (cycles = 0; cycles < 20000 && !status[0]; cycles++) begin
            axi_read(8'h04, status);
        end
        if (!status[0] || status[3:2] != 2'b00)
            $fatal(1, "match-all descriptor status %08x", status);
        expect_word(HIT_ADDR, ({16'd0, TUPLE_LEN} << 17) |
                    (32'd1 << 15) | 32'd8000, "match-all descriptor 0");
        expect_word(HIT_ADDR + 4, ({16'd0, TUPLE_LEN} << 17) |
                    (32'd1 << 15) | 32'd7952, "match-all descriptor 1");
        expect_word(HIT_ADDR + 8, ({16'd0, TUPLE_LEN} << 17) |
                    (32'd1 << 15) | 32'd7904, "match-all descriptor 2");

        /* Streaming int4 SUM/MIN/MAX writes one fixed 64-byte page partial. */
        axi_write(8'h00, 32'h00000002);
        axi_write(8'h08, PAGE_ADDR);
        axi_write(8'h18, HIT_ADDR);
        axi_write(8'h28, 32'd1);
        axi_write(8'h38, 32'd8192);
        axi_write(8'h3c, 32'd1);
        axi_write(8'h24, 32'd0);
        axi_write(8'h44, 32'd0);
        axi_write(8'h4c, 32'd0);
        axi_write(8'h50, 32'h00000004);
        axi_write(8'h70, AGGREGATE_ADDR);
        axi_write(8'h74, 32'd1024);
        axi_write(8'h78, 32'h00070011); /* SUM|MIN|MAX, attr 0, int4. */
        axi_write(8'h7c, 32'd0);
        axi_write(8'h00, 32'h00000001);

        status = 32'd0;
        for (cycles = 0; cycles < 20000 && !status[0]; cycles++) begin
            axi_read(8'h04, status);
        end
        if (!status[0] || status[3:2] != 2'b00)
            $fatal(1, "aggregate accelerator status %08x", status);
        expect_word(AGGREGATE_ADDR, 32'h31414746, "aggregate magic");
        expect_word(AGGREGATE_ADDR + 4, 32'h00400001, "aggregate version");
        expect_word(AGGREGATE_ADDR + 12, 32'h00070001, "aggregate config");
        expect_word(AGGREGATE_ADDR + 16, 32'd1, "aggregate count");
        expect_word(AGGREGATE_ADDR + 32, 32'h11223344, "aggregate sum low");
        expect_word(AGGREGATE_ADDR + 40, 32'h11223344, "aggregate min low");
        expect_word(AGGREGATE_ADDR + 48, 32'h11223344, "aggregate max low");

        /* Join assist hashes the filtered fixed-width key beside the hits. */
        axi_write(8'h00, 32'h00000002);
        axi_write(8'h24, 32'd1);
        axi_write(8'h70, HASH_ADDR);
        axi_write(8'h78, 32'h00000012); /* hash, attr 0, int4. */
        axi_write(8'h7c, 32'h811c9dc5);
        axi_write(8'h00, 32'h00000001);

        status = 32'd0;
        for (cycles = 0; cycles < 20000 && !status[0]; cycles++) begin
            axi_read(8'h04, status);
        end
        if (!status[0] || status[3:2] != 2'b00)
            $fatal(1, "hash accelerator status %08x", status);
        expect_word(HASH_ADDR, 32'h31485346, "hash magic");
        expect_word(HASH_ADDR + 4, 32'h00100001, "hash version");
        expect_word(HASH_ADDR + 12, 32'd1, "hash count");
        expect_word(HASH_ADDR + 16, 32'hdb9d5e28, "hash value");

        axi_read(8'h34, status);
        if (status != 32'h0001000a)
            $fatal(1, "accelerator version expected 0001000a, got %08x", status);

        $display("FILTER_ACCEL_PROJECTION_COMPUTE_TB_PASS");
        $finish;
    end
endmodule
