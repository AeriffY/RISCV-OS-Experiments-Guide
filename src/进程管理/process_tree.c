/*
 * 文件名: process_tree.c
 * 描述: 递归调用 fork 生成二叉树结构的进程组
 * 编译: gcc process_tree.c -o process_tree
 * 运行: ./process_tree [深度]
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* * 递归创建进程树的函数 
 * current_depth: 当前深度
 * max_depth: 目标最大深度
 */
void create_tree(int current_depth, int max_depth) {
    /* 递归终止条件：达到最大深度 */
    if (current_depth >= max_depth) {
        printf("  [叶节点] PID: %d, 深度: %d. 任务完成，进入休眠...\n", getpid(), current_depth);
        sleep(60); /* 保持存活以便观测 */
        exit(0);
    }

    printf("[中间节点] PID: %d, 深度: %d. 正在生成子节点...\n", getpid(), current_depth);

    /* 生成左子节点*/
    pid_t left_pid = fork();
    
    if (left_pid < 0) {
        perror("左节点 fork 失败");
        exit(1);
    } else if (left_pid == 0) {
        /* 子进程逻辑：递归进入下一层 */
        create_tree(current_depth + 1, max_depth);
        /* create_tree 内部会 exit，所以子进程不会执行到这里 */
    }

    /*生成右子节点*/
    pid_t right_pid = fork();

    if (right_pid < 0) {
        perror("右节点 fork 失败");
        exit(1);
    } else if (right_pid == 0) {
        /* 子进程逻辑：递归进入下一层 */
        create_tree(current_depth + 1, max_depth);
    }

    /* * 父进程逻辑：
     * 此时已生成左右两个子节点。
     * 为了维持进程树结构不崩塌（防止子进程变孤儿），父进程必须等待或休眠。
     */
    printf("[中间节点] PID: %d 生成完毕 (L:%d, R:%d). 进入休眠...\n", getpid(), left_pid, right_pid);
    
    /* 休眠 60 秒，给学生足够的时间去另一个终端运行 pstree */
    sleep(60);
    
    /* 等待子进程退出，回收僵尸进程 (可选，程序结束时由 init 回收亦可) */
    wait(NULL);
    wait(NULL);
    
    exit(0);
}

int main(int argc, char *argv[]) {
    int max_depth = 3; // 默认深度

    if (argc > 1) {
        max_depth = atoi(argv[1]);
    }

    printf("根进程 PID: %d\n", getpid());
    printf("目标深度: %d\n", max_depth);
    printf("请在另一个终端执行: pstree -p %d\n", getpid());

    /* 从深度 0 开始递归 */
    create_tree(0, max_depth);

    return 0;
}