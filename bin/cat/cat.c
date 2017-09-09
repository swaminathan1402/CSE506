#include <stdio.h>
#include <stdlib.h>
#include <string.h>
size_t syscall_read(int fd, char *buffer, size_t count){
   
    long long int fd1 = (long long int) fd;
    long long int buffer1 = (long long int) buffer;
    long long int count1 = (long long int) count;
    
    long long int ret;
    __asm__ __volatile__(

        "movq $0, %%rax;"
        "movq %1, %%rbx;"
        "movq %2, %%rcx;"
        "movq %3, %%rdx;"
        "syscall;"
        "movq %%rax, %0;"
        :"=r"(ret)
        :"r"(fd1), "r"(buffer1), "r"(count1)
        :"rax", "rbx", "rcx", "rdx"
    );
    int i = 0;
    while(buffer[i] != '\n') i++;
    buffer[i] = '\0';
    return ret;
}

int syscall_write(int fd, char *buffer, size_t count){


    long long int fd1 = (long long int) fd;
    long long int buffer1 = (long long int) buffer;
    long long int count1 = (long long int) count;
    long long int ret;


    __asm__ __volatile__(

        "movq $1, %%rax;"
        "movq %1, %%rbx;"
        "movq %2, %%rcx;"
        "movq %3, %%rdx;"
        "syscall;"
        "movq %%rax, %0;"
        :"=r"(ret)
        :"r"(fd1), "r"(buffer1), "r"(count1)
        :"rax", "rbx", "rcx", "rdx"
    );
    return ret;
}

size_t syscall_open (const char *filename, int flags, int mode)
{
        long long int filename1= (long long int)filename ;
        long long int flags1=(long long int)flags;
        long long int mode1= (long long int)mode;
         long long int ret;
        __asm__(
                "movq $2, %%rax;"
                "movq %1,%%rbx;"
                "movq %2,%%rcx;"
                "movq %3,%%rdx;"
                "syscall;"
                "movq %%rax,%0"
                :"=r"(ret)
                :"r"(filename1),"r"(flags1),"r"(mode1)
                :"rax","rbx","rcx","rdx"
                );
        return ret;
}

int syscall_close(unsigned int fd)
{
    long long int ret;
    long long int fd1 = (long long int) fd;

     __asm__("movq $3,%%rax;"
	"movq %1,%%rbx;"
        "syscall;"
	"movq %%rax, %0;"
	:"=r"(ret)
	:"r"(fd1)
	:"rax", "rbx"
	);
	return ret;
}

int main (int argc, char *argv[], char *envp[])
{
	int temp=1;
	char str[10240];
	char *strin = str;
	long long int filename ;
	size_t size;
	//argc = 3;
	//char *reads[] = {"/home/nghosh/workdir/todo","/home/nghosh/workdir/README"};
	int i=0;
        int argv_c = 0;
        char parameter_list[argc][1024];
        for(i=0; i<argc; i++){
            int argv_p = 0;
            while(argv[0][argv_c] != '\0'){
            	parameter_list[i][argv_p] = argv[0][argv_c];
            	argv_c++;
            	argv_p++;
                                                                            
            }
            parameter_list[i][argv_c] = argv[0][argv_c];
            argv_c++;
        }	
	while (temp<argc)
	{
		syscall_write(1, parameter_list[temp], strlen(parameter_list[temp]));
		//filename = syscall_open(reads[temp-1],0,0);
		filename = syscall_open(parameter_list[temp], 0, 0);
		size=syscall_read(filename,strin,10240);
		syscall_write(1,strin, size);
		temp++;
		syscall_close(filename);
	}

	return 0;	
}


