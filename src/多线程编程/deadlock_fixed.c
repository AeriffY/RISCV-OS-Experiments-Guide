/*
 * 文件名: deadlock_fixed.c
 * 描述: 通过所有线程均按 A->B 顺序加锁以消除死锁
 * 编译: gcc deadlock_fixed.c -o deadlock_fixed -lpthread
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex_A;
pthread_mutex_t mutex_B;

void *thread_1_func(void *arg) {
    /* 保持原有的加锁顺序: A -> B */
    pthread_mutex_lock(&mutex_A);
    printf("[Thread 1] 已持有 锁A\n");

    printf("[Thread 1] 休眠 1 秒...\n");
    sleep(1);

    printf("[Thread 1] 试图获取 锁B...\n");
    pthread_mutex_lock(&mutex_B);
    
    printf("[Thread 1] 成功获取双锁！任务完成。\n");

    pthread_mutex_unlock(&mutex_B);
    pthread_mutex_unlock(&mutex_A);
    
    return NULL;
}

void *thread_2_func(void *arg) {
    /* * 
     * 修改加锁顺序，使其与 Thread 1 一致 (A -> B)。
     * 即使这里也有 sleep，也不会导致死锁。
     */
    printf("[Thread 2] 试图获取 锁A...\n");
    
    /* * 如果 Thread 1 已经拿了 A 并在睡觉，Thread 2 会在这里阻塞。
     * 因为没有持有 B，所以不会阻碍 Thread 1 醒来后去拿 B。
     */
    pthread_mutex_lock(&mutex_A);
    printf("[Thread 2] 已持有 锁A\n");

    printf("[Thread 2] 休眠 1 秒...\n");
    sleep(1);

    printf("[Thread 2] 试图获取 锁B...\n");
    pthread_mutex_lock(&mutex_B);

    printf("[Thread 2] 成功获取双锁。\n");

    pthread_mutex_unlock(&mutex_B);
    pthread_mutex_unlock(&mutex_A);

    return NULL;
}

int main(void) {
    pthread_t t1, t2;

    pthread_mutex_init(&mutex_A, NULL);
    pthread_mutex_init(&mutex_B, NULL);

    pthread_create(&t1, NULL, thread_1_func, NULL);
    pthread_create(&t2, NULL, thread_2_func, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("所有线程正常退出，未发生死锁\n");

    pthread_mutex_destroy(&mutex_A);
    pthread_mutex_destroy(&mutex_B);

    return 0;
}