#ifndef _DIRENT_H
#define _DIRENT_H
#include <sys/defs.h>
#define NAME_MAX 255

struct dirent {
 long d_ino;
 off_t d_off;
 unsigned short d_reclen;
 char d_name[NAME_MAX+1];
};

typedef struct dirent DIR;

//DIR *opendir(const char *name);
int opendir(const char *name);
//struct dirent *readdir(DIR *dirp);
void readdir(int fd);
//int closedir(DIR *dirp);
int closedir(int fd);

#endif
