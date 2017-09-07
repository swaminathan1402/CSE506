#include <dirent.h>
#include <syscall.h>

int opendir(const char *name){
	int fd = syscall_open(name, 0, 0);
	return fd;
}

int closedir(int fd) {
	int status = syscall_close(fd);
	return status;
}

void readdir(int fd){
	char buffer[1024];
	int total_bytes, i;
	DIR* dirp;
	for(;;) {
		total_bytes = syscall_getdents(fd, buffer, 1024);
		if(total_bytes <= 0) return;
		for(i=0; i<total_bytes;){
			dirp = (struct dirent *)(buffer + i);
			//printf("%s \n", dirp->d_name);
			syscall_write(1, dirp->d_name, 1024);
			i += dirp->d_reclen;
		}
		
	}
	return;
}
/*
int main(){
	const char *name = "/home";
	int fd = openDir(name);
	readDir(fd);
	closeDir(fd);
	
}
*/
