#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <syscall.h>
#define O_DIRECTORY 00200000
#define O_RDONLY 0x0000


int readDir(int fd)
{
        char s[1024];
        char *buffer = s;
         long long int ret;
        struct dirent *dirp;
        while (1)
        {
		ret = getdents(fd, buffer);	
            //printf("%d",ret);
            if(ret ==-1){
		//printf("\nError");
             	break;
            }
            if(ret==0){
		//printf( "\nEmpty directory");
            	break;
            }

           for (int bpos=0 ; bpos<ret;){
                dirp = ( struct dirent *)(buffer +bpos);
                //printf("\t%s",dirp->d_name);
		char *toPrint = dirp->d_name;
		char *space = "\t";
		strcat(toPrint, space);
		int size_toPrint = strlen(toPrint);
		syscall_write(1, toPrint, size_toPrint);
                bpos += dirp->d_reclen;
           }
        }
	return 1;

}


int main (int argc , char *argv[], char *envp[])
{

        long long int fd;
        const char *filename =".";
	char parameter_list[argc][1024];
        if(argc == 1) {
                fd = openDir(filename,0);// O_RDONLY | O_DIRECTORY);
		if(fd == -1) return 0;
	       readDir(fd);
	       closeDir(fd);
        } else {
		int i=0;                                                            	
                int argv_c = 0;
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



		fd = openDir(parameter_list[1],0);// O_RDONLY | O_DIRECTORY);
		if(fd == -1) return 0;
	        readDir(fd);
	        closeDir(fd);
        }
	
	syscall_write(0, "awesome\n",9);
        return 0;
}

