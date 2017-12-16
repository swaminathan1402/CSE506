#ifndef _DIRENT_H
#define _DIRENT_H
#include <sys/defs.h>
#define NAME_MAX 255

struct dirent {
 uint8_t  d_reclen;
 char d_name[NAME_MAX+1];
}__attribute__((packed));

typedef struct DIR DIR;

//DIR *opendir(const char *name);
int opendir(const char *name);
//struct dirent *readdir(DIR *dirp);
void readdir(int fd);
//int closedir(DIR *dirp);
int closedir(int fd);

#endif
