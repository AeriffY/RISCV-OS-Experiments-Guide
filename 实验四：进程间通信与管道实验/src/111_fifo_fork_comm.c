#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
    char buffer[80];       // 缓冲区，用于存放从 FIFO 中读取的数据
    int fd;                // 文件描述符
    char *FIFO = "myfifo"; // FIFO 文件名（必须给指针赋值）

    unlink(FIFO);          // 删除同名 FIFO（如果已存在），避免 mkfifo 失败
    mkfifo(FIFO, 0666);    // 创建命名管道 FIFO，权限为 0666（可读可写）

    // 创建子进程
    if (fork() > 0) {      // 父进程执行写操作

        char s[] = "hello!\n"; // 要写入 FIFO 的数据（注意这里是英文引号）

        fd = open(FIFO, O_WRONLY);     // 以只写方式打开 FIFO
        write(fd, s, sizeof(s));       // 将字符串写入 FIFO
        close(fd);                     // 关闭写端，表示写完

    } else {               // 子进程执行读操作

        fd = open(FIFO, O_RDONLY);     // 以只读方式打开 FIFO
        read(fd, buffer, 80);          // 从 FIFO 读取数据放入 buffer
        printf("%s", buffer);          // 将读取到的数据打印到屏幕
        close(fd);                     // 关闭读端
    }

    return 0; // 程序结束
}