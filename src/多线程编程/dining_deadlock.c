/*
 * 文件名: dining_deadlock.c
 * 描述: 哲学家就餐问题，复现死锁
 * 逻辑: 所有哲学家同时拿起左手边的筷子，休眠，然后试图拿右手边的筷子。
 * 编译: gcc dining_deadlock.c -o dining_deadlock -lpthread
 * 运行: ./dining_deadlock
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define N 5 //哲学家数量

/* 五把锁代表五根筷子 */
pthread_mutex_t forks[N];

void *philosopher(void *arg) {
    int id = *(int *)arg;
    int left = id;
    int right = (id + 1) % N;

    while (1) {
        printf("[哲学家 %d] 思考中...\n", id);
        usleep(1000); //思考一小会儿

        /* 动作 1: 拿起左边的筷子 */
        printf("[哲学家 %d] 试图拿起左边的筷子 (%d)...\n", id, left);
        pthread_mutex_lock(&forks[left]);
        printf("[哲学家 %d] 已持有左筷子 (%d).\n", id, left);

        /* * 强制休眠 1 秒
         * 这会让所有哲学家都完成"动作1"，全部持有了左筷子。
         * 此时，桌上没有剩余的筷子了。
         */
        sleep(1); 

        /* 动作 2: 试图拿起右边的筷子 */
        printf("[哲学家 %d] 试图拿起右边的筷子 (%d)...\n", id, right);
        /* * 死锁发生点:
         * 哲学家 0 等待筷子 1 (被哲学家 1 持有)
         * ...
         * 哲学家 4 等待筷子 0 (被哲学家 0 持有)
         * 形成闭环，所有线程阻塞在此处。
         */
        pthread_mutex_lock(&forks[right]);
        
        printf("[哲学家 %d] 成功拿到双筷，开始进餐！\n", id);
        sleep(1);

        /* 放下筷子 */
        pthread_mutex_unlock(&forks[right]);
        pthread_mutex_unlock(&forks[left]);
        printf("[哲学家 %d] 放下筷子，继续思考。\n", id);
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

    /* 创建 5 个哲学家线程 */
    for (int i = 0; i < N; i++) {
        pthread_create(&phils[i], NULL, philosopher, &ids[i]);
    }

    /* 等待线程结束 (实际上永远不会结束) */
    for (int i = 0; i < N; i++) {
        pthread_join(phils[i], NULL);
    }

    return 0;
}