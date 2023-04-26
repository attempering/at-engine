#ifndef AT__UTILS_DIR__SRC_H__
#define AT__UTILS_DIR__SRC_H__

#include "utils_dir.h"

#include <sys/stat.h>

int at_utils__create_dir(const char* path)
{
  return mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IWOTH);
}

int at_utils__dir_exists(const char *path)
{
  struct stat info;
  if (stat(path, &info) != 0) {
    return 0;
  }
  return S_ISDIR(info.st_mode);
}


int at_utils__create_dir_if_not_exists(const char *path)
{
  if (!at_utils__dir_exists(path)) {
    return at_utils__create_dir(path);
  }

  return 0;
}

#endif
