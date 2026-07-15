#include "msg_client.h"
#include "query.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libnvme.h>
#include <fcntl.h>
#include <unistd.h>
// #include "common.h"

int connect_to_server(char *name) 
{
        int conn, ret;
        struct sockaddr_un address;

        conn = socket(PF_UNIX, SOCK_STREAM, 0);
        if (conn == -1){
                DBG_INFO("socket failed\n");
                return -1;
        }
        memset(&address, 0, sizeof(address));
        address.sun_family = AF_UNIX;
        snprintf(address.sun_path, UNIX_PATH_MAX, "%s", name);

        DBG_INFO("connecting to server %s\n", address.sun_path);

        ret = connect(conn, (struct sockaddr *)&address, sizeof(struct sockaddr_un));
        if (ret != 0)
        {
                DBG_INFO("connect failed\n");
                close(conn);
                return -1;
        }
        return conn;
}

// int connect_to_server() 
// {
//         DBG_INFO("开启nvme设备\n");

//         int fd = nvme_open(DEV_PATH);
//         if(fd < 0) {
//                 perror("open nvme device");
//                 return -1;
//         }
//         DBG_INFO("开启成功\n");
//         return fd;
// }

int close_connect(int conn)
{
	return close(conn);
}

// int close_connect(int fd)
// {
//         free(buffer);
// 	close(fd);
//         return 0;
// }

int send_query_requset(int conn, tupdesc_t *tupdesc, filter_t *filter, qual_t *qual, extent_info_t *filter_extent)
{
	struct iovec iov[4];
        struct msghdr msgh;
        int ret;

        iov[0].iov_base = tupdesc;
        iov[0].iov_len = sizeof(tupdesc_t);
        DBG_INFO("sizeof tupdesc_t = %ld", sizeof(tupdesc_t));

        iov[1].iov_base = filter;
        iov[1].iov_len = sizeof(filter_t);
        DBG_INFO("sizeof filter_t = %ld", sizeof(filter_t));


        iov[2].iov_base = qual;
        iov[2].iov_len = sizeof(qual_t);
        DBG_INFO("sizeof qual_t = %ld", sizeof(qual_t));

        iov[3].iov_base = filter_extent;
        iov[3].iov_len = sizeof(extent_info_t);
        DBG_INFO("sizeof extent_info_t = %ld\n", sizeof(extent_info_t));
	memset(&msgh, 0, sizeof(msgh));
	msgh.msg_name = NULL;
        msgh.msg_namelen = 0;
        msgh.msg_iov = iov;
        msgh.msg_iovlen = 4;
	msgh.msg_control = NULL;
	msgh.msg_controllen = 0;

        // FILE *fp;
        //  // 保存 tupdesc_t
        // fp = fopen("/home/cjj/fpga/module/tupdesc.dat", "wb");
        // if (fp == NULL) {
        //         perror("tupdesc.dat open failed");
        //         return 1;
        // }
        // fwrite(tupdesc, sizeof(tupdesc_t), 1, fp);
        // fclose(fp);
        // fp = fopen("/home/cjj/fpga/module/tupdesc.dat", "rb");
        // tupdesc_t tupdesc_read;
        // fread(&tupdesc_read, sizeof(tupdesc_t), 1, fp);
        // fclose(fp);
        // for(int i = 0; i < MAX_COL; i++) {
        //     DBG_INFO("Read back tupdesc.desc_len_in[%d]=%d, desc_align_in[%d]=%d\n",
        //              i, tupdesc_read.desc_len_in[i],
        //              i, tupdesc_read.desc_align_in[i]);
        // }

        // // 保存 filter_t
        // fp = fopen("/home/cjj/fpga/module/filter.dat", "wb");
        // if (fp == NULL) {
        //         perror("filter.dat open failed");
        //         return 1;
        // }
        // fwrite(filter, sizeof(filter_t), 1, fp);
        // fclose(fp);

        // // 保存 qual_t
        // fp = fopen("/home/cjj/fpga/module/qual.dat", "wb");
        // if (fp == NULL) {
        //         perror("qual.dat open failed");
        //         return 1;
        // }
        // fwrite(qual, sizeof(qual_t), 1, fp);
        // fclose(fp);

        // printf("All data saved successfully.\n");



        // print_msghdr_iov(iov);

        ret = sendmsg(conn, &msgh, 0);
        if (ret < 0){
		DBG_INFO("sendmsg failed errno=%d(%s)", errno, strerror(errno));
		return -1;
	}

	return 0;
}


int recv_query_result(int conn,page_t *pages)
{
        int ret;
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

        ret = recvmsg(conn, &msgh, MSG_WAITALL);
        if (ret < 0){
		DBG_INFO("recvmmsg failed errno=%d(%s)", errno, strerror(errno));
                return ret;
        }
       
	return 0;
}


int send_q6_query_requset(int conn, char *filepath, int file_cnt, int buf_cnt)
{
	struct iovec iov[3];
        struct msghdr msgh;
        int ret;
	int request_cnt;

	tupdesc_t tupdesc;
	filter_t filter;
	qual_t qual;

	fill_tupdesc(&tupdesc);	
	fill_filter_q6(&filter);
	fill_qual(&qual, filepath, file_cnt);

	request_cnt = get_query_request_cnt(&qual, buf_cnt);

        iov[0].iov_base = &tupdesc;
        iov[0].iov_len = sizeof(tupdesc_t);

        iov[1].iov_base = &filter;
        iov[1].iov_len = sizeof(filter_t);

        iov[2].iov_base = &qual;
        iov[2].iov_len = sizeof(qual_t);

	memset(&msgh, 0, sizeof(msgh));
	msgh.msg_name = NULL;
        msgh.msg_namelen = 0;
        msgh.msg_iov = iov;
        msgh.msg_iovlen = 3;
	msgh.msg_control = NULL;
	msgh.msg_controllen = 0;

        ret = sendmsg(conn, &msgh, 0);
        if (ret < 0){
		DBG_INFO("sendmsg failed errno=%d(%s)", errno, strerror(errno));
		return -1;
	}

	return request_cnt;
}

void print_msghdr_iov(struct iovec *iov) {
    for (size_t i = 0; i < 3; i++) {
        void *base = iov[i].iov_base;
        size_t len = iov[i].iov_len;
        DBG_INFO("iov[%zu] at %p, length=%zu\n", i, base, len);

        if (i == 0) { // tupdesc_t
            tupdesc_t *td = (tupdesc_t *)base;
            for (int j = 0; j < MAX_COL; j++) {
                DBG_INFO("  tupdesc.desc_len_in[%d]=%d, desc_align_in[%d]=%d\n",
                         j, td->desc_len_in[j], j, td->desc_align_in[j]);
            }
        } else if (i == 1) { // filter_t
            filter_t *f = (filter_t *)base;
            DBG_INFO("  filter: clause[64], const_float8[64], const_int[128], const_char[1024]\n");

            for (int j = 0; j < 64; j++) {
                pg_clause_op_t *c = &f->clause[j];
                DBG_INFO("    clause[%d]: op_type=%u, op_class=%u, func_id=%u, nargs=%u, "
                         "arg0_tag=%u,arg0_index=%u,arg0_len=%u,arg1_tag=%u,arg1_index=%u,arg1_len=%u\n",
                         j, c->op_type, c->op_class, c->func_id, c->nargs,
                         c->arg0_tag, c->arg0_index, c->arg0_len,
                         c->arg1_tag, c->arg1_index, c->arg1_len);
            }

            for (int j = 0; j < 64; j++) {
                DBG_INFO("    const_float8[%d]=%f\n", j, f->const_float8[j]);
            }

            for (int j = 0; j < 128; j++) {
                DBG_INFO("    const_int[%d]=%d\n", j, f->const_int[j]);
            }

            // const_char 可能很大，可按每行16字节打印部分
            for (int j = 0; j < 1024; j += 16) {
                char buf[17] = {0};
                int k;
                for (k = 0; k < 16 && j+k < 1024; k++) buf[k] = f->const_char[j+k];
                DBG_INFO("    const_char[%d..%d]=%s\n", j, j+k-1, buf);
            }

        } else if (i == 2) { // qual_t
            qual_t *q = (qual_t *)base;
            DBG_INFO("  qual: natts=%d, filter_cnt=%d, file_cnt=%d, filepath=%s\n",
                     q->natts, q->filter_cnt, q->file_cnt, q->filepath);
        }
    }
}
