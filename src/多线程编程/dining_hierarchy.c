/*
 * 文件名: dining_hierarchy.c
 * 描述: 破坏循环等待
 * 逻辑: 规定所有哲学家必须先拿编号小的筷子，再拿编号大的筷子。
 * 编译: gcc dining_hierarchy.c -o dining_hierarchy -lpthread
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define N 5

pthread_mutex_t forks[N];

void *philosopher(void *arg) {
    int id = *(int *)arg;
    int left = id;
    int right = (id + 1) % N;

    /* 确定加锁顺序：min_fork 先加锁，max_fork 后加锁 */
    int first_fork, second_fork;
    
    if (left < right) {
        /* 对于哲学家 0-3：左边是低序号，右边是高序号 */
        first_fork = left;
        second_fork = right;
    } else {
        /* 对于哲学家 4：右边是 0 (低序号)，左边是 4 (高序号) */
        first_fork = right;
        second_fork = left;
    }

    while (1) {
        printf("[哲学家 %d] 思考中...\n", id);
        usleep(rand() % 1000 + 1000); // 随机思考时间

        // 1. 拿第一把锁 (编号较小的)
        pthread_mutex_lock(&forks[first_fork]);
        printf("[哲学家 %d] 拿起了第一把筷子 (%d).\n", id, first_fork);

        // 2. 拿第二把锁 (编号较大的)
        pthread_mutex_lock(&forks[second_fork]);
        printf("[哲学家 %d] 拿起了第二把筷子 (%d). 开始进餐!\n", id, second_fork);

        usleep(rand() % 1000 + 1000);

        // 3. 放下锁
        pthread_mutex_unlock(&forks[second_fork]);
        pthread_mutex_unlock(&forks[first_fork]);
        printf("[哲学家 %d] 放下筷子.\n", id);
    }
    return NULL;
}

int main(void) {
    pthread_t phils[N];
    int ids[N];

    for (int i = 0; i < N; i++) {
        pthread_mutex_init(&forks[i], NULL);
        ids[i] = i;
    }

    for (int i = 0; i < N; i++) {
        pthread_create(&phils[i], NULL, philosopher, &ids[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(phils[i], NULL);
    }

    return 0;
}