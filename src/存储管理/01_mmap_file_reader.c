 /* 利用mmap()来读取/etc/passwd 文件内容*/
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/mman.h>
#include <stdio.h>   // 必须包含，否则 printf 会报错
#include <stdlib.h>  // exit 需要
int main()
{
    int fd;
    void *start;
    struct stat sb;
    fd=open("/etc/passwd",O_RDONLY); /*打开/etc/passwd*/
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    fstat(fd,&sb); /*取得文件大小*/
    /* 利用man fstat 可以看到struct stat 的定义*/
    start=mmap(NULL,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    // if(start == MAP_FAILED) /*判断是否映射成功*/
    // return;
    if (start == MAP_FAILED) {  /*判断是否映射成功*/ 
        perror("mmap");
        close(fd);
        exit(1);
    }
    // printf("%s",start);
    printf("%s\n", (char *)start);  // void* 必须强制转换为 char*
    munmap(start,sb.st_size); /*解除映射*/
    close (fd);

    return 0;
}
