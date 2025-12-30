/*
 * 文件名: race_condition.c
 * 描述: 演示无锁多线程环境下的竞态条件
 * 编译: gcc race_condition.c -o race_condition -lpthread
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* 定义循环次数一百万次 */
#define ITERATIONS 1000000

/* 全局共享计数器 */
long long counter = 0;

void *increment_task(void *arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        /* * counter++ 在汇编层面对应三条指令:
         * 1. LOAD  reg, [counter]
         * 2. ADD   reg, 1
         * 3. STORE [counter], reg
         * 线程切换若发生在 1 和 3 之间，就会导致数据丢失 (Lost Update)。
         */
        counter++; 
    }
    return NULL;
}

int main(void) {
    pthread_t t1, t2;

    printf("=== 竞态条件测试 ===\n");
    printf("预期结果: %d\n", ITERATIONS * 2);

    /* 创建两个并发线程 */
    pthread_create(&t1, NULL, increment_task, NULL);
    pthread_create(&t2, NULL, increment_task, NULL);

    /* 等待线程结束 */
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    /* 打印结果 */
    printf("实际结果: %lld\n", counter);
    
    if (counter != ITERATIONS * 2) {
        printf("[分析] 发生竞态条件！丢失了 %lld 次更新。\n", 
               (ITERATIONS * 2) - counter);
    } else {
        printf("[分析] 运气极好，未发生冲突 (概率极低)。\n");
    }

    return 0;
}