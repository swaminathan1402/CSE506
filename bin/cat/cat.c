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
        "movq %1, %%rdi;"
        "movq %2, %%rsi;"
        "movq %3, %%rdx;"
        "int $0x80;"
        "movq %%rax, %0;"
        :"=r"(ret)
        :"r"(fd1), "r"(buffer1), "r"(count1)
        :"rax", "rdi", "rsi", "rdx"
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
        "movq %1, %%rdi;"
        "movq %2, %%rsi;"
        "movq %3, %%rdx;"
        "int $0x80;"
        "movq %%rax, %0;"
        :"=r"(ret)
        :"r"(fd1), "r"(buffer1), "r"(count1)
        :"rax", "rdi", "rsi", "rdx"
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
                "movq %1,%%rdi;"
                "movq %2,%%rsi;"
                "movq %3,%%rdx;"
                "int $0x80;"
                "movq %%rax,%0"
                :"=r"(ret)
                :"r"(filename1),"r"(flags1),"r"(mode1)
                :"rax","rdi","rsi","rdx"
                );
        return ret;
}

int syscall_close(unsigned int fd)
{
    long long int ret;
    long long int fd1 = (long long int) fd;

     __asm__("movq $3,%%rax;"
	"movq %1,%%rdi;"
        "int $0x80;"
	"movq %%rax, %0;"
	:"=r"(ret)
	:"r"(fd1)
	:"rax", "rdi"
	);
	return ret;
}

int main (int argc, char *argv[], char *envp[])
{
     
   	//char* file2 = argv[1] ;
//	printf  
	int temp=1;
	char str[100];
	char *strin = str;
	char* filename="text_files/text2.txt";
	size_t size;
	int fd;
	//argc = 3;
	//char *reads[] = {"/home/nghosh/workdir/todo","/home/nghosh/workdir/README"};
	/*int i=0;
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
        }*/	
//	while (temp<argc)
//	{
	//	syscall_write(1, parameter_list[temp], strlen(parameter_list[temp]));
		//filename = syscall_open(reads[temp-1],0,0);
		fd = syscall_open(filename, 0, 0);
		size=syscall_read(fd,strin,100);
		syscall_write(1,strin, size);
		while(1);
		temp++;
		syscall_close(fd);
//	}
	while(1);
	return 0;	
}


