# 编译器
CC = gcc

# 目标文件
TARGET = server_app

# 源文件
SRCS = main.c server.c hlog.c nvme_filter.c filter_common.c page.c

# 生成的 .o 文件
OBJS = $(SRCS:.c=.o)

# 头文件路径（如果有单独目录可修改）
INCLUDES = -I.

# 编译参数
CFLAGS = -Wall -g $(INCLUDES)

# 需要链接的库
LIBS = -lnvme

# 默认规则
all: $(TARGET)

# 链接
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)

# 编译 .c 为 .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
