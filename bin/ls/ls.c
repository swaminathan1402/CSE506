#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>

int main (int argc , char *argv[], char *envp[])
{
	int fd;
	const char *filename = ".";
	if(argc < 1) {
		fd = opendir(filename);
	} else {
		fd = opendir(argv[1]);
	}
	readdir(fd);
	closedir(fd);
	return 0;
	/*
	struct dirent *dir;
	char buffer[1024];
	getcwd(buffer,1024);

	//DIR *current = opendir(buffer);
	int fd = open(buffer, 1);
	
	while((dir = readdir(current))!=NULL)
	{
		printf("%s\t", dir->d_name);
	}
	closedir(current);
	return 0;
	*/
}

