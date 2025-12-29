/*
 * 文件名: cpu_time_analysis.c
 * 描述: 利用 times() 统计 Shell 命令与计算密集型任务的 CPU 时间消耗
 * 编译: gcc cpu_time_analysis.c -o cpu_time_analysis
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>

/* 用于打印时间统计的辅助函数 */
void print_times(const char *label, struct tms *start, struct tms *end, clock_t start_tick, clock_t end_tick) {
    /* 获取每秒的时钟滴答数，用于将 tick 转换为秒  */
    long clk_tck = sysconf(_SC_CLK_TCK);

    double real_time = (double)(end_tick - start_tick) / clk_tck;
    
    /* * 注意：system() 调用会创建子进程。
     * 此时应查看子进程消耗的时间 (cutime, cstime)。
     * 也就是 tms_cutime (Children User Time) 和 tms_cstime (Children System Time) 
     */
    double child_user_time = (double)(end->tms_cutime - start->tms_cutime) / clk_tck;
    double child_sys_time  = (double)(end->tms_cstime - start->tms_cstime) / clk_tck;

    /* * 如果是当前进程自身的计算任务，则查看 tms_utime 和 tms_stime
     */
    double self_user_time = (double)(end->tms_utime - start->tms_utime) / clk_tck;
    double self_sys_time  = (double)(end->tms_stime - start->tms_stime) / clk_tck;

    printf("任务: %s\n", label);
    printf("  [墙上时钟时间] Real Time : %.2f s\n", real_time);
    
    if (child_user_time > 0 || child_sys_time > 0) {
        printf("  [子进程消耗]   User Time : %.2f s | Sys Time : %.2f s\n", child_user_time, child_sys_time);
    }
    if (self_user_time > 0 || self_sys_time > 0) {
        printf("  [本进程消耗]   User Time : %.2f s | Sys Time : %.2f s\n", self_user_time, self_sys_time);
    }
    printf("-------------------------------------------------\n");
}

int main(void) {
    struct tms start_tms, end_tms;
    clock_t start_tick, end_tick;

    /* * 任务 1: 执行 Shell 命令 (模拟 I/O 密集或系统调用密集)
     * 使用 find 命令遍历 /etc 目录，这会产生较多的系统调用 (System Time)
     * grep 执行速度通常极快，难以捕捉到显著的时间消耗，故此处使用 find 替代演示
     */
    printf("正在执行 Shell 命令 (find /etc ...)...\n");
    start_tick = times(&start_tms);
    
    /* system() 会调用 fork() 产生子进程执行 /bin/sh [cite: 80] */
    system("find /etc -name '*.conf' > /dev/null 2>&1");
    
    end_tick = times(&end_tms);
    print_times("Shell 命令 (System/IO Bound)", &start_tms, &end_tms, start_tick, end_tick);

    /* * 任务 2: 计算密集型任务 (CPU Bound)
     * 进行大量的整数运算，这主要消耗用户态 CPU 时间 (User Time)
     */
    printf("正在执行计算密集型任务 (矩阵运算模拟)...\n");
    start_tick = times(&start_tms);

    volatile long i; //使用 volatile 防止编译器优化掉空循环
    for (i = 0; i < 500000000; i++) {
    }

    end_tick = times(&end_tms);
    print_times("计算循环 (CPU Bound)", &start_tms, &end_tms, start_tick, end_tick);

    return 0;
}