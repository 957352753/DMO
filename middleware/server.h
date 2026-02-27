#ifndef SERVER_H
#define SERVER_H

#include "common.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stdbool.h>
#include "filter_common.h"
#define UNIX_PATH_MAX 108
#define REGION_SIZE  4096
typedef struct {
    int conn;
    int fd;
} client_ctx_t;
typedef struct {
    int server_sock;
    struct sockaddr_un address;
    char sock_name[256];
    bool running;
    void *data;
    int conn;
    int fd;
} pushdown_server_t;

// 函数接口
int open_server(pushdown_server_t *server, const char *name, void *data);
int start_server(pushdown_server_t *server);
int close_server(pushdown_server_t *server);
int msg_func(int conn,int fd);
int send_result_msg(int conn, page_t *pages);
int load_data_to_buffer(
    char *buffer,
    const tupdesc_t *tupdesc,
    const filter_t *filter,
    const qual_t *qual,
    const extent_info_t *extent_info);
void* client_thread(void* arg);
#endif
