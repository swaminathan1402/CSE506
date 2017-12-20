#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syscall.h>
#define DOT '.'
#define TEST '1'
#define	_W_INT(i)	(i)
#define	_WSTATUS(x)	(_W_INT(x) & 0177)
#define WIFEXITED(x)	(_WSTATUS(x) == 0)
#define WEXITSTATUS(x)	(_W_INT(x) >> 8)
#define PROT_READ 0x04
#define PROT_WRITE 0x02
#define MAP_PRIVATE 0x0000

char shell[255] = "sbush>";
char *shell_text = "sbush";
char *shell_sign = ">";
char *space = " ";
char *bin_string = "/bin/";
char dollar_PATH[1024];
char HOME[1024];
char ROOTFS_BIN_PATH[50];
int run_status = 1;
int ps1_enabled= 0;
int go_background = 0;
char *sanitize(char *);
char *getCommand(char *);
char *getArguments(char *);
void runScripts(char arguments[10][1024]);
void runScript(char *);


char *commandParser(){
	char buffer[200] = "\0";
	scanf("%s", buffer);
	char *cmd = sanitize(buffer);
	return cmd;
}

void runBinary(char *command, char *args, int bgprocess){
	int status;
	go_background = bgprocess;  //1
	if(strcmp(command, "ls")==1){
            printf("Command: %s\n", command);
            if(strlen(args) == 0) {
                args = NULL;
            }
            int pid = syscall_fork();
            if(pid == 0){
                //if(arguments == NULL){
		if(args == NULL){
			char hack[60];
			char buffer[60];
			int cnt = 0;
			char *dir = syscall_getcwd(buffer, 60);
			for(int i=0; i<strlen(dir); i++){
				if(dir[i] != '/'){
					hack[cnt] = dir[i];
					cnt++;
				}
				else {
					break;
				}
			}
			hack[cnt] = '/';
			hack[cnt+1] = '\0';
			args = hack;
			printf("%s\n", args);

		}
		    char* command_args[] = {"bin/ls" , args ,NULL };
		    int ret = syscall_execvpe("bin/ls", command_args, NULL);
                //}
            }
            else if(pid > 0){
		if(go_background == 1){
			yield();
		    printf("Background");
		    return;
		}
		int something = syscall_waitpid(pid, &status, 0);
		printf("Waited on child");
		
            
            }

	}
	
	else if(strcmp(command, "echo") == 1){
            if(strlen(args) == 0) {
                args = NULL;
            }
            int pid = syscall_fork();
            if(pid == 0){
		    char* command_args[] = {"bin/echo" , args ,NULL };
		    int ret = syscall_execvpe("bin/echo", command_args, NULL);
            }
            else if(pid > 0){
		if(go_background == 1){
			yield();
		    printf("Background");
		    return;
		}
		int something = syscall_waitpid(pid, &status, 0);
		printf("Waited on child");
            
            }
	
	}

	else if(strcmp(command, "cat") == 1){
            if(strlen(args) == 0) {
                args = NULL;
            }
            int pid = syscall_fork();
            if(pid == 0){
		    //char* command_args[] = {"bin/cat" , "text_files/text.txt" , "text_files/text2.txt", NULL };
		    printf("%s the cat argument \n", args);
		    char* command_args[] = {"bin/cat" , args,  NULL };
		    int ret = syscall_execvpe("bin/cat", command_args, NULL);
            }
            else if(pid > 0){
		if(go_background == 1){
			yield();
		    printf("Background");
		    return;
		}
		int something = syscall_waitpid(pid, &status, 0);
		printf("Waited on child");
            
            }
	    
	}
	else if(strcmp(command, "kill") == 1){
            if(strlen(args) == 0) {
                args = NULL;
            }
            int pid = syscall_fork();
            if(pid == 0){
		    char* command_args[] = {"bin/kill" , args, NULL };
		    int ret = syscall_execvpe("bin/kill", command_args, NULL);
            }
            else if(pid > 0){
		if(go_background == 1){
		    printf("Background");
		    return;
		}
		int something = syscall_waitpid(pid, &status, 0);
		printf("Waited on child");
            
            }
	
	}

	else if(strcmp(command, "ps") == 1){
            int pid = syscall_fork();
            if(pid == 0){
		    char* command_args[] = {"bin/ps" , NULL, NULL };
		    int ret = syscall_execvpe("bin/ps", command_args, NULL);
            }
            else if(pid > 0){
		if(go_background == 1){
			yield();
		    printf("Background");
		    return;
		}
		int something = syscall_waitpid(pid, &status, 0);
		printf("Waited on child");
            
            }
	
	}
	else if(strcmp(command, "sleep") == 1){
            if(strlen(args) == 0) {
                args = NULL;
            }
            int pid = syscall_fork();
            if(pid == 0){
		    char* command_args[] = {"bin/sleep" , args, NULL };
		    int ret = syscall_execvpe("bin/sleep", command_args, NULL);
            }
            else if(pid > 0){
		if(go_background == 1){
			yield();
		    printf("Background");
		    return;
		}
		int something = syscall_waitpid(pid, &status, 0);
		printf("Waited on child");
            
            }
	
	}










	/*
	int status;
	go_background = bgprocess;  //1
	int pid = syscall_fork();
	if(pid == 0){
		char *arguments = args;

		//if(strlen(args) == 0) {
		//	arguments = NULL;
		//}
		
		//char *cmd_arr[] = {command, arguments, NULL};
		//char final_command[1024] = "\0";
		//if(strcmp(command, "ls") == 1 || strcmp(command, "cat")) {
		//	strcpy(final_command, ROOTFS_BIN_PATH);
		//} else {
		//	strcpy(final_command, bin_string);
		//}
		//strcat(final_command, command);
		//printf("Final command: %s\n", final_command);
		//int ret = syscall_execvpe(final_command, cmd_arr, NULL);


		//printf("Command: %s\n", command);
		//char* command_args[] = {"bin/ps" , NULL, NULL };
		//int ret = syscall_execvpe("bin/ls", command_args, NULL);
		//int ret = syscall_execvpe("bin/ps", command_args, NULL);
		//syscall_exit(ret);
	} else if(pid > 0){
		if(go_background == 1){
			yield();
		        printf("Background");
			return;
		}
		else {
			int something = syscall_waitpid(pid, &status, 0);
			printf("Waited on child");
		}
	}
	*/
}

void interpretCommand(char *command){
	char tempCommand[40] = "\0";
	char function[40] = "\0";
	char arguments[40] = "\0";
	strcpy(tempCommand, command);
	if(strlen(command) == 0){
		return;
	}
	else if(strcmp(tempCommand, "exit") == 1){
		run_status = 0;
		printf("Goodbye!");
		return;
	}
	int runAsBackgroundProcess = 0;	
	int i=0, j=0;
	while(command[i] != '\0' && command[i] != ' '){
		function[i] = command[i];
		i++;
	}
	function[i] = '\0';
	i+=1;
	while(command[i] != '\0'){
		arguments[j] = command[i];
		i++;
		j++;
	}
	arguments[j] = '\0';
	if(j && arguments[j-1] == '&'){
		runAsBackgroundProcess = 1;
		arguments[j-1] = '\0';
		j--;
		printf("Runing as background\n");
	}
	while(arguments[j-1] == ' '){
		arguments[j-1] = '\0';
		j--;
	}
	if(strcmp(function, "cd") == 1) {
		printf("Path: %s", arguments);
		int ret = syscall_chdir(arguments);  // TODO: implement syscall_chdir
		if(ret != 0) {
			char *error_msg = "sbush: cd: No such file or directory\n";
			printf("%s", error_msg);
		}
		return;
	}
	if(strcmp(function, "pwd") == 1){
		char buffer[1024];
		char *dir = syscall_getcwd(buffer, 1024);
		printf("%s", dir);
		return;
	} 
	if(function[0] == '.' && function[1] == '/'){
	   printf("Run script\n"); 
	   runScript(function);
	}

	else {
		runBinary(function, arguments, runAsBackgroundProcess);
		return;
	}
	return;
}


int main(int argc, char* argv[], char* envp[]){

	printf("%s", shell);
	while(run_status){
		char *command = commandParser();
		interpretCommand(command);
		printf("%s", shell);
	}
	return 0;
}


char *sanitize(char *command){
 	int j=0;
 	int i=0;
 	int spaces = 0;
 	while(command[i] == ' ' && command[i] != '\0'){
 		i++;
 	}
 	while(command[i] != '\0'){
 		if(command[i] != ' '){
 			command[j] = command[i];
 			j++;
 			i++;
 			spaces = 0;
 		} else {
 			if(!spaces){
 				spaces = 1;
 				command[j] = command[i];
 				i++;
 				j++;
 			} else {
 				i++;
 			}
 		}
 	}
 	if (j !=0 ) command[j] = '\0';
 	if(spaces) {
 		command[j-1] = '\0';
 	}
 	char *formatted_command = command;
 	return formatted_command;	
 }

 char *getCommand(char *command){
	char b[1024];
	int i = 0;
	while(command[i] != '\0' && command[i] != ' '){
		b[i] = command[i];
		i++;
	}
	b[i] = '\0';
	char *cmd = b;
	return cmd;
 }

 char *getArguments(char *command){
	char buffer[1024];
	int i = 0, j=0;
	while(command[i] != '\0' && command[i] != ' '){
		i++;
	}
	i+=1;
	while(command[i] != '\0'){
		buffer[j] = command[i];
		i++;
		j++;
	}
	buffer[j] = '\0';
	char *args = buffer;
	return args;
 }

/*
void runScript(char *script_path){
	char *filename = script_path;
	int fd = syscall_open(filename, 0, 0);
	char bigBuffer[10240];
	int i = 0;
	int line_number = 0;
	int size = syscall_read(fd, bigBuffer, 1024); // read 1KB each time 
	char temp[1024] = "\0";
	int j = 0;
	while(size > 0){
            while(bigBuffer[i]!='\0' && bigBuffer[i] != '\n'){
                    if(line_number > 0){
                            temp[j] = bigBuffer[i];
                            j++;
                    }
                    i++;
            }
            if(bigBuffer[i] == '\0'){
                    size = syscall_read(fd, bigBuffer, 1024);
                    i=0;
            } 
            if(bigBuffer[i] == '\n'){
                    if(line_number > 0) {
                            temp[j] = '\0';
                            char *generated_command = sanitize(temp);
                            printf("Generated Command: %s\n", generated_command);
                            printf("\n"); 
                    }
                    j = 0;
                    line_number++;
                    i++;
            }
    }
}
*/
void runScript(char *script_path){
	char filename[64];
	int index = 2;
	while(script_path[index] != '\0'){
	    if(script_path[index] == '\n'){
	        break;
	    }
	    filename[index-2] = script_path[index];
	    index++;
	}
	printf("Filename: ", filename);
	int fd = syscall_open(filename, 0, 0);
	char bigBuffer[1024];
	int i = 0;
	int line_number = 0;
	int size = syscall_read(fd, bigBuffer, 1024); // read 1KB each time 
	char temp[1024] = "\0";
	int j = 0;
        char *generated_command;
        if(fd>0){
	
	while(size > 0){
            while(bigBuffer[i]!='\0' && bigBuffer[i] != '\n'){
                    if(line_number > 0){
                            temp[j] = bigBuffer[i];
                            j++;
                    }
                    i++;
                    size--;
            }
            if(bigBuffer[i] == '\0'){
                    size = syscall_read(fd, bigBuffer, 1024);
                    i=0;
            }
            if(bigBuffer[i] == '\n'){
                    if(line_number > 0) {
                            temp[j] = '\0';
                            generated_command = sanitize(temp);
                            //printf("Generated Command: %s\n", generated_command);
                            //printf("\n");
                            //interpretCommand(generated_command);
                    }
                    j = 0;
                    line_number++;
                    i++;
            }
            size--;
    }
    printf("Generated Command: %s\n", generated_command);
        char* command = generated_command;
	char tempCommand[40] = "\0";
	char function[40] = "\0";
	char arguments[40] = "\0";
	strcpy(tempCommand, command);
	if(strlen(command) == 0){
		return;
	}
	else if(strcmp(tempCommand, "exit") == 1){
		run_status = 0;
		printf("Goodbye!");
		return;
	}
	i=0;
	j=0;
	while(command[i] != '\0' && command[i] != ' '){
		function[i] = command[i];
		i++;
	}
	function[i] = '\0';
	i+=1;
	while(command[i] != '\0'){
		arguments[j] = command[i];
		i++;
		j++;
	}
	arguments[j] = '\0';
	while(arguments[j-1] == ' '){
		arguments[j-1] = '\0';
		j--;
	}
	//printf("function: %s", function); 
        runBinary(function, arguments, 0);
    }
}






void runScripts(char arguments[10][1024]){
	char *filename = arguments[1];
	int fd = syscall_open(filename, 0, 0);
	char bigBuffer[10240];
	int i = 0;
	int line_number = 0;
	int size = syscall_read(fd, bigBuffer, 1024); // read 1KB each time 
	char temp[1024] = "\0";
	int j = 0;
	while(size > 0){
		while(bigBuffer[i]!='\0' && bigBuffer[i] != '\n'){
			if(line_number > 0){
				temp[j] = bigBuffer[i];
				j++;
			}
			i++;
		}
		if(bigBuffer[i] == '\0'){
			size = syscall_read(fd, bigBuffer, 1024);
			i=0;
		} 
		if(bigBuffer[i] == '\n'){
			if(line_number > 0) {
				temp[j] = '\0';
				char *generated_command = sanitize(temp);
				//interpretCommand(generated_command);
				printf("Generated Command: %s\n", generated_command);
				printf("\n"); 
			}
			j = 0;
			line_number++;
			i++;
		}
	}

}
