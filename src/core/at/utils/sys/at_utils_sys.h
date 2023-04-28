#ifndef UTILS_SYS_H__
#define UTILS_SYS_H__

int at_utils_sys__create_dir(const char* path);

int at_utils_sys__dir_exists(const char *path);

int at_utils_sys__create_dir_if_not_exists(const char *path);

#endif