#include "server.h"
#include "nvme_filter.h"
static __thread double nvme_total_time = 0.0;
static inline double get_time_sec()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts); // 纳秒级计时，不受系统时间改变影响
    return ts.tv_sec + ts.tv_nsec / 1e9;
}
int open_server(pushdown_server_t *server, const char *name, void *data)
{
    DBG_INFO("open_server start\n");
    int ret;
    server->server_sock = socket(PF_UNIX, SOCK_STREAM, 0);
    if (server->server_sock == -1) 
    {
        return -1;
    }

    server->address.sun_family = AF_UNIX;
    snprintf(server->address.sun_path, UNIX_PATH_MAX, "%s", name);

    ret = unlink(name);
    if (ret != 0 && errno != ENOENT && errno != EPERM) 
    {
        return -2;
    }

    ret = bind(server->server_sock, (struct sockaddr *) &server->address, sizeof(server->address));
    if (ret != 0)
    {
        return -3;
    }

    strncpy(server->sock_name, name, strlen(name)+1);
    server->data = data;
    server->running = false;
    server->conn = -1;
    server->fd = nvme_open(DEV_PATH);
    return 0;
}
void* client_thread(void* arg)
{
    // pthread_t ptid = pthread_self();
    // pid_t tid = syscall(SYS_gettid);

    // DBG_INFO("client thread start: pthread_id=%lu, tid=%d\n",
    //          (unsigned long)ptid, tid);
    
    //return NULL;
    client_ctx_t *ctx = (client_ctx_t*)arg;
    int conn = ctx->conn;
    int fd   = ctx->fd;
    free(ctx);

    while (1) {
        if (msg_func(conn, fd) != 0) {
            DBG_INFO("client disconnected\n");
            break;
        }
    }
    DBG_INFO("total_time=%.6f s\n",nvme_total_time);
    close(conn);
    return NULL;
}
int start_server(pushdown_server_t *server)
{

    int ret;
    struct sockaddr_un client_addr;
    socklen_t addrlen;

    ret = listen(server->server_sock, 128);
    if (ret != 0) 
    {
        DBG_INFO("listen failed errno=%d\n", ret);
        return -1;
    }
    chmod(server->sock_name, 0777);
    DBG_INFO("server start running\n");
    server->running = true;

    while (server->running) 
    {
        addrlen = sizeof(client_addr);
        filter_cnt = 0;

        int conn = accept(server->server_sock, (struct sockaddr*)&client_addr, &addrlen);
        if (conn < 0) {
            DBG_INFO("accept failed\n");
            continue;
        }
        DBG_INFO("accepted a client connection: %d\n",conn);

        // 构造线程参数
        client_ctx_t *ctx = malloc(sizeof(client_ctx_t));
        ctx->conn = conn;
        ctx->fd = server->fd;

         // 创建线程
        pthread_t tid;
        pthread_create(&tid, NULL, client_thread, ctx);
        pthread_detach(tid);   // 不阻塞主线程

        // if (server->conn == -1) 
        // {
        //     DBG_INFO("accept failed errno=%d(%s)\n", errno, strerror(errno));
        //     continue;
        //     //return -2;
        // }

        // while(1)
        // {
        //     ret = msg_func(server->conn,server->fd);
        //     if (ret != 0) 
        //     {
        //         DBG_INFO("client disconnected\n");
        //         break;
        //     }
        // }
        // close(server->conn);// ← 关闭已断开的连接
        // server->conn = -1;// ← 重置
        DBG_INFO("wait for next client...\n");
    }

    return 0;
}

int close_server(pushdown_server_t *server) 
{
    if (server->server_sock >= 0) 
    {
        close(server->server_sock);
        server->server_sock = -1;
    }
    return 0;
}

int send_result_msg(int conn, page_t *pages)
{
    struct msghdr msgh;
	struct iovec iov[1];
    iov[0].iov_base = pages;
    iov[0].iov_len = sizeof(page_t) * 16;

    memset(&msgh, 0, sizeof(msgh));
    msgh.msg_name = NULL;
	msgh.msg_namelen = 0;
	msgh.msg_iov = iov;
	msgh.msg_iovlen= 1;
	msgh.msg_control= NULL;
	msgh.msg_controllen= 0;
    int ret;
    DBG_INFO("send_result_msg start\n");
    ret = sendmsg(conn, &msgh, 0);
    if(ret < 0) 
    {
        DBG_INFO("sendmsg failed errno=%d\n", ret);
        return -1;
    }

    return 0;
}

int msg_func(int conn,int fd) 
{
    //需要用四个过滤信息  填充这个结构体 
    struct msghdr msgh;
	struct iovec iov[4];
	tupdesc_t tupdesc;
	filter_t filter;
	qual_t qual;
    extent_info_t extent_info;
	iov[0].iov_base = &tupdesc;
	iov[0].iov_len = sizeof(tupdesc_t);

	iov[1].iov_base = &filter;
	iov[1].iov_len = sizeof(filter_t);

	iov[2].iov_base = &qual;
	iov[2].iov_len = sizeof(qual_t);

    iov[3].iov_base = &extent_info;
    iov[3].iov_len = sizeof(extent_info_t);

	memset(&msgh, 0, sizeof(msgh));
	msgh.msg_name = NULL;
	msgh.msg_namelen = 0;
	msgh.msg_iov = iov;
	msgh.msg_iovlen= 4;
	msgh.msg_control= NULL;
	msgh.msg_controllen= 0;

    int ret;
    DBG_INFO("msg_func start\n");
    ret = recvmsg(conn, &msgh, 0); 
    DBG_INFO("recvmsg ret=%d\n", ret);
    if (ret <= 0) {
        DBG_INFO("recvmsg failed errno=%d(%s)\n", errno, strerror(errno));
        return -1;
    }

    // DBG_INFO("recvmsg success\n");
    // // 打印接收到的消息
    // DBG_INFO("extent_info: start_block=%lu, block_count=%lu\n", extent_info.start_block, extent_info.block_count);
    // DBG_INFO("qual.natts=%d, qual.filter_cnt=%d, qual.file_cnt=%d, qual.filepath=%s\n", 
    //     qual.natts, qual.filter_cnt, qual.file_cnt, qual.filepath);
    //把数据发送给CSD 进行过滤

    
    // 4KB对齐
    page_t *pages = aligned_alloc(8192, 8192 * 16);
    memset(pages, 0, sizeof(page_t) * 16);
    

    //加载发送的信息
    ret = load_data_to_buffer(
        pages[0].data,
        &tupdesc,
        &filter,
        &qual,
        &extent_info);
    if(ret < 0)
    {
        DBG_INFO("load_data_to_buffer failed errno=%d\n", ret);
        return ret;
    }
    //发送过滤指令
    DBG_INFO("nvme_vsc_send start\n");
    double start_ts = get_time_sec();
    ret = nvme_vsc_send(pages[0].data,fd);
    double end_ts = get_time_sec();
    double duration = end_ts - start_ts;

    nvme_total_time += duration;

    DBG_INFO("total_time=%.6f s\n",nvme_total_time);
    // // double start_ts = get_time_sec();
    // // ret = nvme_vsc_send(pages[0].data,fd);
    // // double end_ts = get_time_sec();
    // // double duration = end_ts - start_ts;
    // // total_time += duration;
    
    // DBG_INFO("nvme_vsc_send ret=%d, time=%.6f s, total_time=%.6f s, filter_cnt=%d\n", ret, duration, total_time, filter_cnt);

    DBG_INFO("filter_result page num =%d\n",*((int*)pages[0].data));
    
    DBG_INFO("send_result_msg start\n");
    ret = send_result_msg(conn, pages);
    if(ret < 0) 
    {
        DBG_INFO("send_result_msg failed errno=%d\n", ret);
        return ret;
    }
    free(pages);
    return 0;
}

int get_tuple_cnt(const char *data) {
    const uint8_t *base = (const uint8_t *)data;   // 指向第一页数据
    uint16_t pd_lower = ((uint16_t)base[13] << 8) | base[12]; // 解析 pd_lower（2 字节）

    int tuple_cnt = (pd_lower - 24) >> 2;
    if (tuple_cnt < 0) tuple_cnt = 0;

    return tuple_cnt;
}

int load_data_to_buffer(
        char *buffer,
        const tupdesc_t *tupdesc,
        const filter_t *filter,
        const qual_t *qual,
        const extent_info_t *extent_info)
{
    if (!buffer || !tupdesc || !filter || !qual || !extent_info) {
        return -1;
    }

    // 检查每个结构体大小是否超过 4KB 区域
    if (sizeof(tupdesc_t) > REGION_SIZE ||
        sizeof(filter_t) > REGION_SIZE ||
        sizeof(qual_t) > REGION_SIZE ||
        sizeof(extent_info_t) > REGION_SIZE)
    {
        fprintf(stderr, "Error: one of the structs exceeds 4KB region size!\n");
        return -2;
    }

    // 计算每项在 buffer 中的起始偏移
    char *tupdesc_pos      = buffer + REGION_SIZE * 0; // 0 KB
    char *filter_pos       = buffer + REGION_SIZE * 1; // 4 KB
    char *qual_pos         = buffer + REGION_SIZE * 2; // 8 KB
    char *extent_info_pos  = buffer + REGION_SIZE * 3; // 12 KB

    // 全部清零（可选）
    //memset(buffer, 0, REGION_SIZE * 4);

    // 写入 4 个区域
    memcpy(tupdesc_pos,     tupdesc,     sizeof(tupdesc_t));
    memcpy(filter_pos,      filter,      sizeof(filter_t));
    memcpy(qual_pos,        qual,        sizeof(qual_t));
    memcpy(extent_info_pos, extent_info, sizeof(extent_info_t));

    return 0;
}