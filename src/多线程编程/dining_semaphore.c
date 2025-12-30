/*
 * 文件名: dining_semaphore.c
 * 描述: 限制并发数
 * 逻辑: 最多允许 4 个哲学家同时竞争筷子，保证至少有一人能凑齐一对。
 * 编译: gcc dining_semaphore.c -o dining_semaphore -lpthread
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5

pthread_mutex_t forks[N];
/* 定义信号量，初始值为 4 (允许的最大并发进餐人数) */
sem_t limit;

void *philosopher(void *arg) {
    int id = *(int *)arg;
    int left = id;
    int right = (id + 1) % N;

    while (1) {
        printf("[哲学家 %d] 思考中...\n", id);
        usleep(rand() % 2000);

        /* * 申请进餐许可
         * 如果当前已有 4 人在尝试进餐，第 5 个人会被阻塞在这里。
         * 他甚至没有机会去拿第一把锁，从而避免了死锁。
         */
        sem_wait(&limit); 

        pthread_mutex_lock(&forks[left]);
        printf("[哲学家 %d] 拿起左筷 (%d).\n", id, left);
        
        /* * 即使在这里发生切换，因为最多只有 4 个人占了左筷子，
         * 必然有一根筷子是空闲的，从而有人能拿到右筷子。
         */
        
        pthread_mutex_lock(&forks[right]);
        printf("[哲学家 %d] 拿起右筷 (%d). 进餐中...\n", id, right);

        usleep(rand() % 2000); // 进餐

        pthread_mutex_unlock(&forks[right]);
        pthread_mutex_unlock(&forks[left]);

        /* 归还进餐许可 */
        sem_post(&limit);
        
        printf("[哲学家 %d] 完成进餐.\n", id);
    }
    return NULL;
}

int main(void) {
    pthread_t phils[N];
    int ids[N];

    /* 初始化互斥锁 */
    for (int i = 0; i < N; i++) {
        pthread_mutex_init(&forks[i], NULL);
        ids[i] = i;
    }

    sem_init(&limit, 0, 4);

    for (int i = 0; i < N; i++) {
        pthread_create(&phils[i], NULL, philosopher, &ids[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(phils[i], NULL);
    }

    /* 清理资源 */
    sem_destroy(&limit);
    for (int i = 0; i < N; i++) {
        pthread_mutex_destroy(&forks[i]);
    }

    return 0;
}