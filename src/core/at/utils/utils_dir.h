#ifndef AT__UTILS_DIR_H__
#define AT__UTILS_DIR_H__

int create_dir(const char* path);

int dir_exists(const char *path);

int create_dir_if_not_exists(const char *path);

#endif