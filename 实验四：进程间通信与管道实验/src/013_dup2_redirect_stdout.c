#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int fd1;
    // 如果使用dup2，需要给第二个参数赋一个初始值
    int fd2 = 3;

    // 将标准输出（fd 1）复制一份为fd2，这样fd2也指向标准输出
    dup2(1, fd2);
    printf("fd2 = %d\n", fd2);

    // 打开文件test.txt，若不存在则创建，可读写权限
    fd1 = open("test.txt", O_CREAT | O_RDWR, 0664);

    // 输出重定向：
    // 关闭标准输出fd 1，然后将fd1复制一份为fd 1
    // 此时printf输出会写入test.txt文件
    dup2(fd1, 1);
    printf("hello world\n");

    // 恢复标准输出：
    // 关闭fd 1，然后将fd2复制一份为fd 1
    // 此时printf输出回到终端
    dup2(fd2, 1);
    printf("你好北京\n");

    return 0;
}