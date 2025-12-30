#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

#define FIFO_NAME "myfifo"

int main()
{
    int fd;

    // 创建 FIFO，如果已存在忽略
    if (mkfifo(FIFO_NAME, 0664) == -1)
    {
        if (errno != EEXIST)
        {
            perror("fail to mkfifo");
            exit(1);
        }
    }

    // 非阻塞方式打开 FIFO，只写
    // 如果没有读端，open 会失败
    fd = open(FIFO_NAME, O_WRONLY | O_NONBLOCK);
    if (fd < 0)
    {
        perror("open fifo (write)");
        exit(1);
    }

    char msg[128];
    int count = 1;
    while (1)
    {
        sprintf(msg, "Message %d from writer\n", count++);
        ssize_t n = write(fd, msg, strlen(msg));
        if (n == -1)
        {
            if (errno == EAGAIN)
            {
                printf("no reader, write failed\n");
            }
            else
            {
                perror("write error");
                break;
            }
        }
        else
        {
            printf("written: %s", msg);
        }
        sleep(1);
    }

    close(fd);
    return 0;
}
