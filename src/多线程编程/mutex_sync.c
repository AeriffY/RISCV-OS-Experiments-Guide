/*
 * 文件名: mutex_sync.c
 * 描述: 使用互斥锁解决竞态条件
 * 编译: gcc mutex_sync.c -o mutex_sync -lpthread
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define ITERATIONS 1000000

long long counter = 0;

/* 定义互斥锁变量 */
pthread_mutex_t mutex;

void *increment_task_safe(void *arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        /* * 进入临界区前加锁
         * 若锁已被占用，当前线程将被挂起
         */
        pthread_mutex_lock(&mutex);
        
        /* 临界区: 此时只有当前线程能执行 */
        counter++;
        
        /* 退出临界区立即解锁，唤醒等待队列中的其他线程 */
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(void) {
    pthread_t t1, t2;
    clock_t start, end;
    double cpu_time_used;

    /* 初始化互斥锁，使用默认属性 */
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("互斥锁初始化失败");
        exit(1);
    }

    
    start = clock(); /* 开始计时 */

    pthread_create(&t1, NULL, increment_task_safe, NULL);
    pthread_create(&t2, NULL, increment_task_safe, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    end = clock(); /* 结束计时 */

    /* 销毁互斥锁 */
    pthread_mutex_destroy(&mutex);

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("实际结果: %lld (预期: %d)\n", counter, ITERATIONS * 2);
    printf("运行耗时: %f 秒\n", cpu_time_used);

    if (counter == ITERATIONS * 2) {
        printf("数据一致性已恢复。\n");
    }

    return 0;
}