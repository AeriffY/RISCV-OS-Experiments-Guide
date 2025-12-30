#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int filedes[2];   // filedes[0]：读端，filedes[1]：写端
    char buffer[80];

    // 创建无名管道
    if (pipe(filedes) < 0)
    {
        printf("pipe error");
        return -1;
    }

    // 创建子进程
    if (fork() > 0)
    {
        // 父进程
        char s[] = "hello!\n";

        close(filedes[0]);                // 关闭读端
        write(filedes[1], s, sizeof(s));  // 写入管道
        close(filedes[1]);                // 关闭写端
    }
    else
    {
        // 子进程
        close(filedes[1]);                // 关闭写端
        read(filedes[0], buffer, 80);     // 从管道读取数据
        printf("%s", buffer);
        close(filedes[0]);                // 关闭读端
    }

    return 0;
}
