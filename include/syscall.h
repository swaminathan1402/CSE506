#ifndef _syscall_H
#define _syscall_H
#include <sys/defs.h>
#include <dirent.h>


int syscall_write(int fd, char *msg, size_t size);
int syscall_open(const char *filename, int flags, int mode);
int syscall_close(unsigned int fd);
char* syscall_read(int fd, char *msg, size_t size);
int syscall_unlink(const char *pathname);
int syscall_chdir(const char *filename);
char *syscall_getcwd(char *buf, size_t size);
pid_t syscall_fork();
int syscall_execvpe(const char *filename, char *const argv[], char *const envp[]);
pid_t syscall_waitpid(pid_t pid, int *status, int options);
pid_t syscall_getpid();
pid_t syscall_getppid();
int syscall_lseek(unsigned int fd, int offset, unsigned int origin);
int syscall_mkdir(const char *pathname, int mode);
int syscall_pipe(int *filedes);
int syscall_exit(int status);
int syscall_dup2(unsigned int old_fd, unsigned int new_fd);
void *syscall_malloc(size_t size);
void syscall_free(void *ptr);
void *syscall_mmap(unsigned long addr, unsigned long len, unsigned long prot, unsigned long flags, unsigned long fd, unsigned long offset);
int syscall_munmap(unsigned long addr, size_t len);
int syscall_getdents(int fd, char *buffer, unsigned int count);

#endif
