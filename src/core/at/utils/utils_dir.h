#ifndef AT__UTILS_DIR_H__
#define AT__UTILS_DIR_H__

int at_utils__create_dir(const char* path);

int at_utils__dir_exists(const char *path);

int at_utils__create_dir_if_not_exists(const char *path);

#endif