#include <stdio.h>
#include <syscall.h>

int main(){
    printf("Going to sleep now");
    syscall_sleep(5);
    printf("Done sleeping");
//    while(1);
}
