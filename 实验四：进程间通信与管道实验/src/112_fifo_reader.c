#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FIFO_NAME "myfifo"

int main()
{
    // 先删除可能已经存在的 FIFO 文件
    unlink(FIFO_NAME);

    // 创建新的 FIFO（具名管道）
    if (mkfifo(FIFO_NAME, 0666) < 0) {
        perror("mkfifo");
        exit(1);
    }

    // 以只读方式打开 FIFO（会阻塞，直到有写端打开）
    int fd = open(FIFO_NAME, O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    printf("读取程序启动：等待数据……\n");

    // 循环读取
    while (1) {
        char buf[128];
        ssize_t n = read(fd, buf, sizeof(buf)-1);
        if (n > 0) {
            buf[n] = '\0';
            printf("读程序：收到 %s", buf);
        }
        else if (n == 0) {
            // 所有写端关闭，FIFO 到达 EOF
            printf("读程序：所有写端已关闭，退出。\n");
            break;
        }
        else {
            perror("read");
            break;
        }
    }

    close(fd);
    unlink(FIFO_NAME); // 删除 FIFO 文件
    return 0;
}