#include<stdio.h>
#include<dirent.h>


int syscall_chdir(const char *filename){
	long long int filename1 = (long long int) filename;
	long long int ret;
	__asm__ (
	"movq $80, %%rax;"
	"movq %1,%%rdi;"
	"int $0x80;"
	"movq %%rax, %0;"
	:"=r"(ret)
	:"r"(filename1)
	:"rax", "rdi"
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


int getdents(int fd, char *buffer){

        long long int fd1= (long long int) fd;
	long long int buf = (long long int) buffer;
	long long int ret;
	__asm__ __volatile__(
                "movq $78, %%rax;"
                "movq %1, %%rdi;"
                "movq %2, %%rsi;"
                "movq $1024,%%rdx;"
                "int $0x80;"
                "movq %%rax , %0"
                :"=m"(ret)
                :"r"(fd1),"m"(buf)
                :"rax","rdi","rsi","rdx"
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

char* syscall_getcwd (char *buf , size_t size) {

	long long int buf1 = (long long int) buf;
	long long int size1 = (long long int) size;
	__asm__ __volatile__(

		"movq $79, %%rax;"
		"movq %1,%%rdi;"
		"movq %2,%%rsi;"
		"int $0x80;"
		"movq %%rax, %0;"
		:"=m"(buf1)
		:"r"(buf1), "r"(size1)
		:"rax", "rdi", "rsi"
	);
	//syscall_write(1, buf, strlen(buf));
	return buf;
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

int readDir(int fd)
{
        char s[1024];
        char *buffer = s;
         long long int ret;
        struct dirent *dirp;
	ret = getdents(fd, buffer);	
            //printf("%d",ret);
            if(ret ==-1){
	//	kprintf("\nError");
			syscall_write(1, "shit", 4);
             
            }
            if(ret==0){
	//	/printf( "\nEmpty directory");
            //	break;
			syscall_write(1, "crap", 4);
            }
	   // syscall_write( 1, buffer, );	
           for (int bpos=0 ; bpos<ret;){
                dirp = (struct dirent *)(buffer +bpos);
                //printf("\t%s",dirp->d_name);
		char *toPrint = dirp->d_name;
	 	*(toPrint+ dirp->d_reclen-1 ) ='\t';
		int size_toPrint = dirp->d_reclen-1;
		syscall_write(1, toPrint, size_toPrint);
                bpos += dirp->d_reclen;
           }
	return 1;

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


void cat(char *filename)
{

char str[400];
char *strin= str;
//syscall_write(1, filename , strlen(filename));
syscall_write(1,"Hello Cat!!",11);
int file_fd= syscall_open(filename, 0,0);

syscall_write(1,"Hello Cat!!",11);
if(file_fd == -1)
	return;
int size= syscall_read(file_fd , strin , 400);
//syscall_write(1,"Hello Cat!!",11);
syscall_write(1, strin,size);
//syscall_close(filename);
}

uint64_t syscall_mmap(int bytes){

    
    long long int ret;
    long long int bytes1 = (long long int) bytes;
     __asm__(
	"movq $9,%%rax;"
	"movq %1,%%rdi;"
        "int $0x80;"
	"movq %%rax, %0;"
	:"=r"(ret)
	:"r"(bytes1)
	:"rax", "rdi"
	);

    return ret;
}

uint64_t syscall_munmap(uint64_t addr){

    long long int ret;
    long long int addr1 = (long long int) addr;
     __asm__(
	"movq $11, %%rax;"
	"movq %1, %%rdi;"
        "int $0x80;"
	"movq %%rax, %0;"
	:"=r"(ret)
	:"r"(addr1)
	:"rax", "rdi"
	);

    return ret;
}

void *malloc(int bytes){
	uint64_t addr = syscall_mmap(bytes);
	return (void*)addr;	
}

void free(uint64_t addr){

	syscall_munmap(addr);
}

int main(int argc, char *argv[], char *envp[]){
for(int i=0; i<2; i++){
  int pid = syscall_fork();
  if (pid == 0){
    syscall_write(0, "child process\n", 14);
    /*
	Segmentation faul testing 
    int *x = (int *)0xffffffffffffffff;
    *x = *x + 20;
    */
    /*
	Stack overflow testing	
    char buffer[10000000];
    int i=0;
    while(1){
	buffer[i] = 'a';
	i++;
	syscall_write(0, buffer, 10000000);
    }
    */
	/*
    syscall_write(0, "child process2\n", 14);
    int c = (int)malloc(4);
    int d = (int)malloc(123);
    char *str = (char *)malloc(1024);
    str = "nirvik ghosh is the best";
    syscall_write(0, str, strlen(str));
    free(str);
	*/
    //free(c);
    //free(d);
    /*
		Multiple forks
    int pid2 = syscall_fork();
    if(pid2 == 0){

	    syscall_write(0, "hello1\n", 5);
    } else {

	    syscall_write(0, "hello2\n", 5);
    }
    */
    //char *command_args[] = {"bin/echo", "hello mister karey ka sister", (char *)0};
    //syscall_execvpe("bin/echo", command_args, (char *)0); // TODO
    //syscall_write(0 , "childprocess\n" , 13);

    //int fd = openDir("bin/",0);
/*	char fdchar[5] ="Fd:";
	fdchar[3]= fd+48;               
	fdchar[4]= '\n';                
	syscall_write(0 , fdchar, 4);
*/
	
	//readDir(fd);
 	//cat("text_files/text2.txt");


	/*
 	char buffer[20];	
        syscall_getcwd(buffer,20);
        syscall_write(1, buffer ,strlen(buffer));
	char buffer2[10];
	int ret=syscall_chdir("usr/");
	syscall_getcwd(buffer2,10);
	if(ret==1)
	{
	syscall_write(1,buffer2, strlen(buffer2));
	}
	else
	{
	char buffer2[15]= "Wrong setting";
	syscall_write(1 ,buffer2, strlen(buffer2));
	}
	ret=syscall_chdir("..");
	syscall_getcwd(buffer2,10 );
	syscall_write(1,buffer2,10);
	ret=syscall_chdir("bin/");
        syscall_getcwd(buffer2,10 );
        syscall_write(1,buffer2,10);
	ret=syscall_chdir("/");
        syscall_getcwd(buffer2,10 );
        syscall_write(1,buffer2,10);
	*/
	
} else {
	int status; 
	int ret = syscall_waitpid(pid, &status, NULL);
	syscall_write(0, "good process\n", 12);
  }
}
return ;
}
