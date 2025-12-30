/*
 * 文件名: deadlock_demo.c
 * 描述: 演示 ABBA 顺序导致的双线程死锁
 * 编译: gcc deadlock_demo.c -o deadlock_demo -lpthread
 * 运行: ./deadlock_demo 程序将陷入永久停滞，需按 Ctrl+C 终止
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* 定义两个互斥锁 */
pthread_mutex_t mutex_A;
pthread_mutex_t mutex_B;

void *thread_1_func(void *arg) {
    /* 1. 获取锁 A */
    pthread_mutex_lock(&mutex_A);
    printf("[Thread 1] 已持有 锁A\n");

    /* 2. 强制休眠，放弃 CPU，让 Thread 2 有机会运行并获取 锁B */
    printf("[Thread 1] 休眠 1 秒...\n");
    sleep(1);

    printf("[Thread 1] 试图获取 锁B...\n");
    /* 3. 尝试获取锁 B (此时锁 B 被 Thread 2 持有，将阻塞) */
    pthread_mutex_lock(&mutex_B);
    
    printf("[Thread 1] 成功获取双锁！\n");

    /* 4. 释放锁 */
    pthread_mutex_unlock(&mutex_B);
    pthread_mutex_unlock(&mutex_A);
    
    return NULL;
}

void *thread_2_func(void *arg) {
    /* 1. 获取锁 B (与 Thread 1 顺序相反) */
    pthread_mutex_lock(&mutex_B);
    printf("[Thread 2] 已持有 锁B\n");

    /* 2. 强制休眠，确保 Thread 1 已持有 锁A */
    printf("[Thread 2] 休眠 1 秒...\n");
    sleep(1);

    printf("[Thread 2] 试图获取 锁A...\n");
    /* 3. 尝试获取锁 A (此时锁 A 被 Thread 1 持有，将阻塞) */
    pthread_mutex_lock(&mutex_A);

    printf("[Thread 2] 成功获取双锁！\n");

    /* 4. 释放锁 */
    pthread_mutex_unlock(&mutex_A);
    pthread_mutex_unlock(&mutex_B);

    return NULL;
}

int main(void) {
    pthread_t t1, t2;

    /* 初始化互斥锁 */
    pthread_mutex_init(&mutex_A, NULL);
    pthread_mutex_init(&mutex_B, NULL);

    printf("=== 死锁演示程序启动 ===\n");

    /* 创建线程 */
    pthread_create(&t1, NULL, thread_1_func, NULL);
    pthread_create(&t2, NULL, thread_2_func, NULL);

    /* 等待线程结束 (实际上永远不会等到) */
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    /* 清理资源 */
    pthread_mutex_destroy(&mutex_A);
    pthread_mutex_destroy(&mutex_B);

    return 0;
}