#include <stdio.h>
#include <string.h>
size_t syscall_read(int fd, char *buffer, size_t count){
    
    int ret;
    __asm__(

        "movl $0, %%eax;"
        "movl %1, %%ebx;"
        "movl %2, %%ecx;"
        "movl %3, %%edx;"
        "syscall;"
        "movl %%eax, %0;"
        :"=m"(ret)
        :"m"(fd), "m"(buffer), "m"(count)
        :
    );
    int i = 0;
    while(buffer[i] != '\n') i++;
    buffer[i] = '\0';
    return ret;
}

int syscall_write(int fd, char *buffer, size_t count){
    int ret;
    __asm__(

        "movl $1, %%eax;"
        "movl %1, %%ebx;"
        "movl %2, %%ecx;"
        "movl %3, %%edx;"
        "syscall;"
        "movl %%eax, %0;"
        :"=m"(ret)
        :"m"(fd), "m"(buffer), "m"(count)
        :
    );
    return ret;
}

int syscall_pipe(int *filedes)

{

        int ret;

        __asm__ ("movl $22 , %%eax;"

        "movl %1,%%ebx;"

        "syscall;"

        "movl %%eax, %0;"

        :"=m"(ret)

        :"m"(filedes)

        :

        );

        return ret;

}

pid_t syscall_fork(){
    int ret;
    __asm__(
        "movl $57, %%eax;"
        "syscall;"
        "movl %%eax, %0;"
        :"=m"(ret)
        :
        :
    );
    return ret;

}


int syscall_dup2(unsigned int old_fd, unsigned int new_fd){

        int ret;

        __asm__(

                "movl $33, %%eax;"

                "movl %1, %%ebx;"

                "movl %2, %%ecx;"

                "syscall;"

                "movl %%eax, %0;"

                : "=m"(ret)

                : "m"(old_fd), "m"(new_fd)

                :

        );

        return ret;

}


int main(){
    /*
    while(1){
        char command[1024];
        syscall_read(0, command, 1024);
        syscall_write(1, command, strlen(command));

        //printf("%s\n", command);
    }//syscall_write(1, command, 1024);
    */
    return 0;
}
