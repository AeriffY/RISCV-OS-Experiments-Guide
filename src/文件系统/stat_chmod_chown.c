#include <sys/stat.h>

struct stat st;
stat("source_file", &st); // 获取源文件属性

// 应用属性到目标文件
chmod("target_file", st.st_mode);          // 克隆权限模式
chown("target_file", st.st_uid, st.st_gid); // 克隆所有者与所属组