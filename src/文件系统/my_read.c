#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    printf("Real UID: %d\n", getuid());       // 运行该程序的真实用户 ID
    printf("Effective UID: %d\n", geteuid()); // 内核检查权限时使用的有效 ID

    // 尝试读取机密文件
    system("cat /root/secret.txt");
    return 0;
}