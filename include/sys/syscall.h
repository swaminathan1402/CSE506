#ifndef _SYSCALL_H
#define _SYSCALL_H
#include <sys/defs.h>

int SYSCALL_write(int fd, const void *msg, size_t size);
int SYSCALL_open(const char *filename, int flags, int mode);
int SYSCALL_close(unsigned int fd);
size_t SYSCALL_read(fd, void *msg, size_t size);
int SYSCALL_unlink(const char *pathname);
int SYSCALL_chdir(const char *filename);
char *SYSCALL_getcwd(char *buf, size_t size);
pid_t SYSCALL_fork();
int SYSCALL_execvpe(const char *filename, char *const argv[], char *const envp[]);
pid_t SYSCALL_waitpid(pid_t pid, int *status, int options);
pid_t SYSCALL_getpid();
pid_t SYSCALL_getppid();
int SYSCALL_lseek(unsigned int fd, int offset, unsigned int origin);
int SYSCALL_mkdir(const char *pathname, int mode);
int SYSCALL_pipe(int *filedes);
int SYSCALL_exit(int status);
int SYSCALL_dup2(unsigned int old_fd, unsigned int new_fd);

#endif
