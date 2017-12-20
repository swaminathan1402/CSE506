#include <stdio.h>
#include <syscall.h>


int main(int argc, char *argv[], char *envp[]){
    printf("\nGoing to sleep now");
    syscall_sleep(atoi(argv[1]));
    printf("Done sleeping");
//    while(1);
}
