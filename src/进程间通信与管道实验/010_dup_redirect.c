#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    // 如果需要实现输出重定向的功能
    // printf函数操作的是文件描述符1，默认对应终端
    // 只要改变1对应的文件，就可以实现输出重定向
    // 实现方法：创建文件得到文件描述符后，关闭1，然后用dup复制新的文件描述符到1
    // 这样printf输出就写入到文件中

    int fd_file;
    fd_file = open("test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0664);
    if (fd_file == -1)
    {
        perror("fail to open");
        exit(1);
    }

    // 关闭标准输出文件描述符
    close(1);

    // 将fd_file复制到文件描述符1
    int fd = dup(fd_file);

    printf("hello world\n");
    printf("fd = %d\n", fd);

    return 0;
}