#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define FIFO_NAME "myfifo"

int main()
{
    // 打开 FIFO 写端（若无读端则阻塞）
    int fd = open(FIFO_NAME, O_WRONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    pid_t pid = getpid();

    // 每隔 1 秒写一次 PID
    while (1) {
        char msg[128];
        sprintf(msg, "进程 %d 正在写入\n", pid);

        write(fd, msg, strlen(msg));
        sleep(1);
    }

    close(fd);
    return 0;
}