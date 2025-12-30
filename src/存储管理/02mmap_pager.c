#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define LINES_PER_PAGE 20   // 每页 20 行

int main(int argc, char *argv[])
{
    // 参数检查
    if (argc < 2) {
        printf("Usage: %s filename\n", argv[0]);
        return 1;
    }

    // 打开文件
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    // 获取文件大小
    struct stat sb;
    fstat(fd, &sb);

    // mmap 映射文件内容
    char *start = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (start == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    close(fd); // 映射后即可关闭文件描述符

    // 当前读取位置（指向 mmap 内存区）
    size_t pos = 0;
    // 保存每一页的起始位置，用于“回读上一页”
    size_t page_starts[10000]; 
    int current_page = 0; // 当前页编号

    page_starts[0] = 0; // 第 0 页从 pos=0 开始

    while (pos < sb.st_size) {

        int line_count = 0;     // 当前页已读取的行数
        size_t page_begin = pos; // 记录当前页起始位置

        //输出一页（每页 20 行）
        while (pos < sb.st_size && line_count < LINES_PER_PAGE) {

            putchar(start[pos]);

            // 遇到换行符 → 行数加 1
            if (start[pos] == '\n') {
                line_count++;
            }

            pos++;
        }

        //  当前页结束 
        printf("\n--- 回车继续，b 返回上一页，q 退出 --- ");

        int c = getchar();
        getchar();  // 读取掉回车本身

        if (c == 'q') break;

        if (c == 'b') {
            // 如果是第一页，就不再后退
            if (current_page == 0) {
                pos = page_starts[0];
            } else {
                current_page--;
                pos = page_starts[current_page]; // 重新回到上一页开头
            }
            continue;
        }

        // 用户按回车 → 进入下一页
        current_page++;
        page_starts[current_page] = pos; // 记录下一页的起始位置
    }

    munmap(start, sb.st_size);
    return 0;
}
