#include "common.h"
#include "server.h"
#define SERVER_PATH "/home/cjj/module/seqscan_server" // 与client端保持一致

int main()
{   
    hlog_init();
    pushdown_server_t server;
    int ret;
    ret = open_server(&server, SERVER_PATH, NULL);
    DBG_INFO("open_server ret=%d\n", ret);
    start_server(&server);
    return 0;
}