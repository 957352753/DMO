#include "nvme_filter.h"
int filter_cnt = 0;
double total_time = 0.0;

int nvme_vsc_send(void* buffer,int fd)
{
    
    
    if(fd < 0) 
    {
        perror("open nvme device\n");
        return -1;
    }
    
    int result = 0;
    int err = nvme_io_passthru(fd, OPCODE,
                                0,               // flags
                                0,               // reserved
                                NSID,            // namespace id
                                0, 0,            // cdw2, cdw3
                                0, 0, PAGE_NUM, 0, 0, 0, // cdw10 to cdw15 (命令参数，可修改)
                                BUFFER_LEN,      // data_len
                                buffer,          // data 主机内存地址
                                0,               // metadata_len
                                NULL,            // metadata
                                1000,            // timeout_ms
                                result);         // result

    if(err < 0) {
        DBG_INFO("nvme_io_passthru failed");
        close(fd);
        return -1;
    }

    //close(fd);
    return 0;
}
