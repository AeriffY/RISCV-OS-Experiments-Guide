#include <fcntl.h>
#include <unistd.h>

// 示例：将 src.txt 的内容复制到 dest.txt
int fd_in = open("src.txt", O_RDONLY);
int fd_out = open("dest.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

char buf[1024];
ssize_t bytes;
while ((bytes = read(fd_in, buf, sizeof(buf))) > 0) {
    write(fd_out, buf, bytes);
}
close(fd_in);
close(fd_out);