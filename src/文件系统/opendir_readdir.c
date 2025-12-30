#include <dirent.h>

DIR *dir = opendir("my_dir");
struct dirent *entry;

while ((entry = readdir(dir)) != NULL) {
    printf("文件名: %s | 类型: %d\n", entry->d_name, entry->d_type);
}
closedir(dir);