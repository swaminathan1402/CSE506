#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syscall.h>


void cat(char *filename){

    char str[400];
    char *strin= str;
    int file_fd= syscall_open(filename, 0,0);

    if(file_fd == -1)
            return;

    int size= syscall_read(file_fd , strin , 400);
    syscall_write(1, strin,size);
}

int main (int argc, char *argv[], char *envp[])
{
	int temp=1;
	char str[10240];
	char *strin = str;
	long long int filename ;
	size_t size;
	//argc = 3;
	//char *reads[] = {"/home/nghosh/workdir/todo","/home/nghosh/workdir/README"};
	int i=0;
        int argv_c = 0;
        char parameter_list[argc][1024];
        for(i=0; i<argc; i++){
            int argv_p = 0;
            while(argv[0][argv_c] != '\0'){
            	parameter_list[i][argv_p] = argv[0][argv_c];
            	argv_c++;
            	argv_p++;
                                                                            
            }
            parameter_list[i][argv_c] = argv[0][argv_c];
            argv_c++;
        }	
	while (temp<argc)
	{
		syscall_write(1, parameter_list[temp], strlen(parameter_list[temp]));
		//filename = syscall_open(reads[temp-1],0,0);
		filename = syscall_open(parameter_list[temp], 0, 0);
		size=syscall_read(filename,strin,10240);
		syscall_write(1,strin, size);
		temp++;
		syscall_close(filename);
	}

	return 0;	
}


