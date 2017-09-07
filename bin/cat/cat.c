#include<stdio.h>
#include<stdlib.h>
#include <syscall.h>

int main (int argc, char*argv[], char *envp[])
{
	int temp=1;
	char str[1024];
	while (temp!=argc)
	{
		//FILE *filename = fopen(argv[temp],"r");
		int fd = syscall_open(argv[temp], 0, 0); 
		//while (fgets(str,1024, fd )!=NULL)
		while (syscall_read(fd, str, 1024) != 0)
		{
			//printf("%s",str);
			syscall_write(1, str, 1024);
		}
		//fclose(filename);
		syscall_close(fd);
		temp++;
	}

}


