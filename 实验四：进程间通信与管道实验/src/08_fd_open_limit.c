#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    int fd;
    while (1)
    {
        fd = open("file.txt", O_RDONLY | O_CREAT, 0664);
        if (fd < 0)
        {
            perror("fail to open");
            exit(1);
        }

        printf("fd = %d\n", fd);
    }

    return 0;
}
