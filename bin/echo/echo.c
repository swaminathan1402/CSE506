#include<stdio.h>
#include<string.h>

int main(int argc, char *argv[], char *envp[]) {

  char *message = argv[1];
  syscall_write(0, message, strlen(message));
  return 0;
}
