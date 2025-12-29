/*
 * 文件名: pid_isolation.c
 * 描述: 使用 clone 系统调用和 CLONE_NEWPID 标志创建独立的 PID 命名空间
 * 编译: gcc pid_isolation.c -o pid_isolation
 * 运行: sudo ./pid_isolation
 */

#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>

/* 定义子进程的栈大小 */
#define STACK_SIZE (1024 * 1024)

/* 子进程执行的函数 */
static int child_fn(void *arg) {
    /* * 在子进程的视角中：
     * 由于使用了 CLONE_NEWPID，该进程在新的命名空间中是第一个进程。
     * 因此，getpid() 应该返回 1。
     */
    printf("\n[子进程] 正在运行...\n");
    printf("[子进程] 我的 PID (getpid返回) : %d\n", getpid());
    printf("[子进程] 我的父进程 PID (getppid返回): %d\n", getppid());
    
    printf("[子进程] 我现在拥有类似 init 进程的特权 (PID=1)，处于隔离环境中。\n");
    printf("[子进程] 退出中...\n");
    
    return 0;
}

int main(int argc, char *argv[]) {
    /* 为子进程分配独立的栈空间 */
    /* 注意：在大多数架构上，栈是向下增长的，但我们需要分配堆内存 */
    char *stack = malloc(STACK_SIZE);
    if (!stack) {
        perror("malloc 失败");
        exit(1);
    }

    /* * 计算栈顶指针。
     * 假设栈向下增长，我们需要将指针指向分配区域的末尾。
     */
    char *stack_top = stack + STACK_SIZE;

    printf("[父进程] 准备调用 clone 创建子进程...\n");

    /* * 调用 clone 系统调用：
     * child_fn: 子进程入口函数
     * stack_top: 子进程栈指针
     * CLONE_NEWPID: 创建新的 PID 命名空间
     * SIGCHLD: 子进程结束时向父进程发送信号，以便 waitpid 工作
     */
    pid_t pid = clone(child_fn, stack_top, CLONE_NEWPID | SIGCHLD, NULL);

    if (pid == -1) {
        perror("clone 调用失败 (是否使用了 sudo?)");
        exit(1);
    }

    /* * 在父进程的视角中：
     * 子进程只是宿主机系统中的一个普通进程，拥有一个常规的 PID（例如 12345）。
     */
    printf("[父进程] 子进程已创建。\n");
    printf("[父进程] 在我看来，子进程的 PID 是: %d\n", pid);

    /* 等待子进程结束 */
    waitpid(pid, NULL, 0);
    
    printf("[父进程] 子进程已退出，实验结束。\n");

    free(stack);
    return 0;
}