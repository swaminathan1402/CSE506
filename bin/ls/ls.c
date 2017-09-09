#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#define O_DIRECTORY 00200000
#define O_RDONLY 0x0000
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

int openDir (const char* file, int flags)
{
	long long int file1 =(long long int) file;
	long long int flags1 = (long long int)flags;
	long long int ret;
	
	__asm__ __volatile__(
		"movq $2, %%rax;"
		"movq %1 ,%%rbx;"
		"movq %2, %%rcx;"
		"movq $0 ,%%rdx;"
		"syscall;"
		"movq %%rax, %0;"
		:"=r"(ret)
		:"r" (file1), "r"(flags1)
		:"rax","rbx","rcx","rdx"
	);
	return ret;

}

int getdents(int fd, char *buffer){

        long long int fd1= (long long int) fd;
	long long int buf = (long long int) buffer;
	long long int ret;
	__asm__ __volatile__(
                "movq $78, %%rax;"
                "movq %1, %%rbx;"
                "movq %2, %%rcx;"
                "movq $1024,%%rdx;"
                "syscall;"
                "movq %%rax , %0"
                :"=r"(ret)
                :"r"(fd1),"m"(buf)
                :"rax","rbx","rcx","rdx"
        );
	return ret;
}

int readDir(int fd)
{
        char s[1024];
        char *buffer = s;
         long long int ret;
        struct dirent *dirp;
        while (1)
        {
		ret = getdents(fd, buffer);	
            //printf("%d",ret);
            if(ret ==-1){
		//printf("\nError");
             	break;
            }
            if(ret==0){
		//printf( "\nEmpty directory");
            	break;
            }

           for (int bpos=0 ; bpos<ret;){
                dirp = ( struct dirent *)(buffer +bpos);
                //printf("\t%s",dirp->d_name);
		char *toPrint = dirp->d_name;
		char *space = "\t";
		strcat(toPrint, space);
		int size_toPrint = strlen(toPrint);
		syscall_write(1, toPrint, size_toPrint);
                bpos += dirp->d_reclen;
           }
        }
	return 1;

}


int closeDir(int fd)
{
	long long int fd1= (long long int)fd;
	long long int ret;
	__asm__(
	   "movq  $3, %%rax;"
	   "movq %1, %%rbx;"
	   "syscall;"
	   "movq %%rax, %0;"
	   :"=r"(ret)
	   :"r" (fd1)
	   :"rax","rbx"
	);
	return ret;
}

int main (int argc , char *argv[], char *envp[])
{
        long long int fd;
        const char *filename =".";
        if(argc == 1) {
                fd = openDir(filename,0);// O_RDONLY | O_DIRECTORY);
        } else {
		syscall_write(1,"ls works\n", 9);
		syscall_write(1, argv[1], strlen(argv[1]));
                fd = openDir(argv[1],0);// O_RDONLY | O_DIRECTORY);
        }
	if(fd == -1){
		return 0;
	}
       readDir(fd);
       closeDir(fd);
        return 0;
}
