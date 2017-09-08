#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>


int main(int argc, char* argv[], char* envp[]){
    syscall_write(1, argv[argc+1], 17);
    return 0;
}
