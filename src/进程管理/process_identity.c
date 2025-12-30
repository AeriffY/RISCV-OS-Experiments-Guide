/*
 * 文件名: process_identity.c
 * 描述: 打印进程的各类标识符 (PID, PPID, UID, EUID, GID, EGID)
 * 编译: gcc process_identity.c -o process_identity
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    /* * 获取并打印进程标识符 
     * getpid(): 获取当前进程ID
     * getppid(): 获取父进程ID 
     */
    printf("Process Status:\n");
    printf("  PID  (Process ID)        : %d\n", getpid());
    printf("  PPID (Parent Process ID) : %d\n", getppid());
    
    /* * 获取并打印用户与组标识符
     * getuid():  真实用户ID (Real UID) - 谁启动了进程
     * geteuid(): 有效用户ID (Effective UID) - 进程拥有的权限身份 
     * getgid():  真实组ID
     * getegid(): 有效组ID
     */
    printf("User Identity:\n");
    printf("  UID  (Real User ID)      : %d\n", getuid());
    printf("  EUID (Effective User ID) : %d\n", geteuid());
    
    printf("Group Identity:\n");
    printf("  GID  (Real Group ID)     : %d\n", getgid());
    printf("  EGID (Effective Group ID): %d\n", getegid());

    return 0;
}