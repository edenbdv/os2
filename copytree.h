// copytree.h
#ifndef COPYTREE_H
#define COPYTREE_H


#include <sys/stat.h> 

#ifdef __cplusplus
extern "C" {
#endif

void copy_file(const char *src, const char *dest, int copy_symlinks, int copy_permissions);
void copy_directory(const char *src, const char *dest, int copy_symlinks, int copy_permissions);
int mkdir_p(const char *path, mode_t mode);

#ifdef __cplusplus
}
#endif

#endif // COPYTREE_H