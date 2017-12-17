#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syscall.h>



int main (int argc, char *argv[], char *envp[]){

	
	char str[100];
	char *strin = str;
	char* filename="text_files/text2.txt";
	size_t size;
	int fd = syscall_open(filename, 0, 0);
	size=syscall_read(fd,strin,100);
	syscall_write(1,strin, size);
	syscall_close(fd);

}


