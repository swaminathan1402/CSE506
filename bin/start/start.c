#include<stdio.h>
#include <syscall.h>


int main(){
	int status;
	int pid = syscall_fork();
	if(pid == 0){
	//    int ret  
	syscall_execvpe("bin/sbush", NULL, NULL);
	} else {
		 syscall_waitpid(pid, &status, 0);
	} 
	return 0;
}
