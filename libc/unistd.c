#include <unistd.h>  // C wrapper to make syscalls 
#include <stdio.h>
#include <syscall.h>  // sycall_functions are defined here 

int open(const char *pathname, int flags){
	/*
	int status = syscall(2, pathname, flags, 0);  // Might Erro
	return status;
	*/
	int status = syscall_open(pathname, flags, 0);	
	return status;
}

int close(int fd){
	/*
	int status = syscall(3, fd);
	return status;
	*/
	int status = syscall_close(fd);
	return status;
}

ssize_t read(int fd, void *buf, size_t count){
	/*
	int status = syscall(0, fd, buf, count);
	return status;
	*/
	int status = syscall_read(fd, buf, count);
	return status;
}

ssize_t write(int fd, char *buf, size_t count){
	/*
	int status = syscall(1, fd, buf, count);
	return status;
	*/
	int status = syscall_write(fd, buf, count);
	return status;
}

int unlink(const char *pathname) {
	/*
	int status = syscall(87, pathname);
	return status;
	*/
	int status = syscall_unlink(pathname);
	return status;
}

int chdir(const char *path){
	/*
	int status = syscall(80, path);
	return status;
	*/
	int status = syscall_chdir(path);
	return status;
}

char *getcwd(char *buff, size_t size){
	/*
	syscall(79, buff, size);
	return buff;
	*/
	syscall_getcwd(buff, size);
	return buff;	
	
}

pid_t fork() {
	/*
	int pid = syscall(57);
	return pid;
	*/
	pid_t pid = syscall_fork();
	return pid;
}

int execvpe(const char *file, char *const argv[], char *const envp[]) {
	/*
	int status = syscall(59, file, argv, envp);	
	return status;
	*/
	int status = syscall_execvpe(file, argv, envp);
	return status;
}

int execv(const char *file, char *const argv[]){
	/*
	int status = execvpe(file, argv, NULL);
	return status;
	*/
	return execvpe(file, argv, NULL);
}

/*
pid_t wait(int *status) {
	int status_wait = syscall(61, -1, status, 0, NULL);
	eturn status_wait;
}
*/

int waitpid(int pid, int *status) {
	/*
	int status_wait = syscall(61, pid, status, 0, NULL);
	return status_wait;
	*/
	int status_wait = syscall_waitpid(pid, status, 0);
	return status_wait;
}

unsigned int sleep(unsigned int seconds) {
	// this function+arguments applies to grad students=> :D 
	return 0;
}

pid_t getpid(void) {
	/*
	int pid = syscall(39);
	return pid;
	*/
	pid_t pid = syscall_getpid();
	return pid;
}

pid_t getppid(void) {
	/*
	int ppid = syscall(110);
	return ppid;
	*/
	pid_t pid = syscall_getppid();
	return pid;
}

// OPTIONAL: implement for ``on-disk r/w file system (+10 pts)''
off_t lseek(int fd, off_t offset, int whence) {
	/*
	off_t status = syscall(8, fd, offset, whence);	
	return status;
	*/
	off_t status = syscall_lseek(fd, offset, whence);
	return status;
}

int mkdir(const char *pathname, mode_t mode) {
	/*
	return syscall(83, pathname, mode);
	*/
	int status = syscall_mkdir(pathname, mode);
	return status;
}
// OPTIONAL: implement for ``signals and pipes (+10 pts)''
int pipe(int *pipefd) {
	/*
	int status;
	status = syscall(22, pipefd);
	return status;
	*/
	int status = syscall_pipe(pipefd);
	return status;
}
/*
int main(){
	const char buff[] = "hello world\n";
	int ret = write(1, buff, 10);
	char buffer[1024];
	getcwd(buffer, 1024);
	printf("%s \n", buffer);

	char *ls[] = {"ls", "-ltr", NULL};
	char *cmd = "/bin/ls";
	execv(cmd, ls);
	return 0;
}
*/
