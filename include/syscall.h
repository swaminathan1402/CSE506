#ifndef _syscall_H
#define _syscall_H
#include <sys/defs.h>

int syscall_write(int fd, const void *msg, size_t size);
int syscall_open(const char *filename, int flags, int mode);
int syscall_close(unsigned int fd);
size_t syscall_read(int fd, void *msg, size_t size);
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

#endif
