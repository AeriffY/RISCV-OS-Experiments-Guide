#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

#define BUF_SIZE 128

int main()
{
    int p2c[2]; // 父进程 → 子进程 p2c[0]:子进程读 p2c[1]：父进程写
    int c2p[2]; // 子 → 父 c2p[0]：父进程读  c2p[1]：子进程写

    // 建立两个无名管道
    if (pipe(p2c) < 0 || pipe(c2p) < 0) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
       
        // 子进程：接收数字 → 计算 → 返回结果
        close(p2c[1]); // 关闭父写端
        close(c2p[0]); // 关闭父读端

        int num;
        ssize_t r;

        while ((r = read(p2c[0], &num, sizeof(int))) > 0) {
            int result = num * 10;  // 计算
            if (write(c2p[1], &result, sizeof(int)) < 0) {
                perror("child write");
                exit(1);
            }
        }

        close(p2c[0]);// 关闭子读端
        close(c2p[1]); // 关闭子写端
        exit(0);
    }

    // 父进程：读取文件 → 把数字发给子进程 → 输出结果
    close(p2c[0]); // 关闭子读端
    close(c2p[1]); // 关闭子写端

    int fd = open("data.txt", O_RDONLY);
    if (fd < 0) {
        perror("open data.txt");
        exit(1);
    }

    char buf[BUF_SIZE + 1];
    int num;

    while (1) {
        ssize_t n = read(fd, buf, BUF_SIZE);
        if (n < 0) {
            perror("file read");
            exit(1);
        }
        if (n == 0) break; // EOF

        buf[n] = '\0';

        // 按空格分割数字
        char *token = strtok(buf, " \n");
        while (token != NULL) {

            num = atoi(token); // 字符串转换为数字

            // 发送给子进程
            if (write(p2c[1], &num, sizeof(int)) < 0) {
                perror("parent write");
                exit(1);
            }

            // 接收子进程的计算结果
            int result;
            if (read(c2p[0], &result, sizeof(int)) < 0) {
                perror("parent read");
                exit(1);
            }

            printf("父进程：接收到 %d * 10 的计算结果 = %d\n", num, result);

            token = strtok(NULL, " \n");
        }
    }
    // 关闭文件和管道
    close(fd);
    close(p2c[1]); // 父进程关闭写端
    close(c2p[0]); // 父进程关闭读端

    // 回收子进程
    wait(NULL);

    return 0;
}
