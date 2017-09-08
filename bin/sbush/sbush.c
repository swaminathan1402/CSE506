#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>


int main(int argc, char* argv[], char* envp[]){  
  syscall_write(1,"Hello world\n",12);
 //  for (i=0;i<17;i++)
  // syscall_write(1, envp[i], 1024)1
syscall_write(1,argv[0],2);
return 0;
}
