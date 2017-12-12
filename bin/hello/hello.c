#include<stdio.h>

int openDir (const char* file, int flags)
{
	long long int file1 =(long long int) file;
	long long int flags1 = (long long int)flags;
	long long int ret;
	
	__asm__ __volatile__(
		"movq $2, %%rax;"
		"movq %1 ,%%rdi;"
		"movq %2, %%rsi;"
		"movq $0 ,%%rdx;"
		"int $0x80;"
		"movq %%rax, %0;"
		:"=r"(ret)
		:"r" (file1), "r"(flags1)
		:"rax","rdi","rsi","rdx"
	);
	return ret;

}

int syscall_write(int fd, char *buffer, int count){


    long long int fd1 = (long long int) fd;
    long long int buffer1 = (long long int) buffer;
    long long int count1 = (long long int) count;
    long long int ret;


    __asm__ __volatile__(
        "movq $1, %%rax;"
        "movq %1, %%rdi;"
        "movq %2, %%rsi;"
        "movq %3, %%rdx;"
       	"int $0x80;"
        "movq %%rax, %0;"
        :"=r"(ret)
        :"r"(fd1), "r"(buffer1), "r"(count1)
        :"rax", "rsi", "rdi", "rdx"
    );
    return ret;
}
int syscall_read(int fd, char *buffer, int count){
   
    long long int fd1 = (long long int) fd;
    long long int buffer1 = (long long int) buffer;
    long long int count1 = (long long int) count;
    
    long long int ret;
    __asm__(

        "movq $0, %%rax;"
        "movq %1, %%rdi;"
        "movq %2, %%rsi;"
        "movq %3, %%rdx;"
        "int $0x80;"
        "movq %%rax, %0;"
        :"=r"(ret)
        :"r"(fd1), "r"(buffer1), "r"(count1)
        :"rax", "rdi", "rsi", "rdx"
    );
    if(fd == 0){
	    int i = 0;
	    while(buffer[i] != '\n') i++;
	    buffer[i] = '\0';
    }
    return ret;
}
int syscall_fork(){
    long long int ret;
    __asm__ __volatile__(
        "movq $57, %%rax;"
        "int $0x80;"
        "movq %%rax, %0;"
        :"=r"(ret)
        :
        :"rax"
    );
    return ret;

}
int syscall_execvpe(const char *filename , char *const argv[], char *const envp[] ){
	long long int filename1= (long long int)filename; // random_binary 
	long long int argv1 = (long long int) argv;
	long long int envp1= (long long int) envp;	
	long long int ret;
	__asm__ __volatile__(
		"movq $59, %%rax;"
		"movq %1,%%rdi;"
		"movq %2,%%rsi;"
		"movq %3,%%rdx;"
		"int $0x80;"
		"movq %%rax, %0;"
		:"=r" (ret)
		:"r" (filename1), "r"(argv1), "r" (envp1)
		:"rax", "rdi", "rsi", "rdx"
	);
	return ret;
	
}


int syscall_exit(int status) {
	long long int status1 = (long long int) status;
	long long int ret;
	__asm__(
	"movq $60, %%rax;"
	"movq %1, %%rbx;"
	"syscall;"
	"movq %%rax, %0;"
	: "=r"(ret)
	: "r"(status1)
	:"rax", "rbx"
	);
	return ret;

}
int syscall_waitpid(int pid, int *status, int options){
	long long int pid1 = (long long int) pid;
	long long int options1 = (long long int) options;
	long long int ret;

	__asm__ __volatile__(
		"movq $61,%%rax;"
		"movq %1,%%rdi;"
		"movq %2,%%rsi;"
		"movq %3, %%rdx;"
		"int $0x80;"
		"movq %%rax, %0;"
		: "=r" (ret)
		: "r"(pid1), "r"(status), "r"(options1)
		: "rax", "rdi", "rsi", "rdx"
	);
	return ret;

}

int main(int argc, char *argv[], char *envp[]){
  //syscall_write(0, "hello world\n", 11); 
  //syscall_write(0, "hello world\n", 11); 
  int pid = syscall_fork();
  if (pid == 0){
    syscall_write(0, "child process\n", 14);
    syscall_write(0, "child process\n", 14);
    syscall_write(0, "child process\n", 14);
    syscall_write(0, "child process\n", 14);
    syscall_write(0, "child process\n", 14);
    syscall_write(0, "child process\n", 14);
    syscall_write(0, "child process\n", 14);
    //char *command_args[] = {"bin/echo", "hello mister karey ka sister", (char *)0};
    //syscall_execvpe("bin/echo", command_args, (char *)0); // TODO

  } else {
    syscall_write(0, "parentprocess1\n", 15);
    int status;
    int wait_status = syscall_waitpid(pid, &status, (char *)0);
    if(wait_status > 0)
    	syscall_write(0, "parentprocess2\n", 15);
  }
/*
char buffer[1024];
syscall_read(1, buffer, 1024);
syscall_write(0, buffer, 1024);

int fd = openDir("bin/sbush",0);
char fdchar[5] ="Fd:";
fdchar[3]= fd+48;               
fdchar[4]= '\n';                
syscall_write(0 , fdchar, 4);
*/
return ;
}
