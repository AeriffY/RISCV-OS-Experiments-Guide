/*
 * 文件名: thread_memory.c
 * 描述: 验证 POSIX 线程共享进程地址空间的特性
 * 编译: gcc thread_memory.c -o thread_memory -lpthread
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int global_shared_var = 0;

/* 子线程执行函数 */
void *thread_function(void *arg) {
    printf("\n[子线程] 启动执行...\n");
    
    /* 打印变量地址，验证是否与主线程一致 */
    printf("[子线程] 变量地址: %p, 修改前数值: %d\n", 
           (void*)&global_shared_var, global_shared_var);
    
    /* 修改全局变量 */
    global_shared_var = 100;
    printf("[子线程] 已将变量修改为: 100\n");
    
    return NULL;
}

int main(void) {
    pthread_t thread_id;
    int ret;

    printf("[主线程] 初始变量地址: %p, 初始数值: %d\n", 
           (void*)&global_shared_var, global_shared_var);

    /* 创建新线程 */
    ret = pthread_create(&thread_id, NULL, thread_function, NULL);
    if (ret != 0) {
        perror("线程创建失败");
        exit(1);
    }

    /* 阻塞等待子线程结束，确保修改已完成 */
    pthread_join(thread_id, NULL);

    printf("\n[主线程] 子线程已退出。\n");
    
    /* 再次读取变量，验证修改是否对主线程可见 */
    printf("[主线程] 最终变量数值: %d\n", global_shared_var);

    if (global_shared_var == 100) {
        printf("[结论] 验证成功：线程间共享地址空间。\n");
    } else {
        printf("[结论] 验证失败：数据未同步。\n");
    }

    return 0;
}