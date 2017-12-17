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
	//printf("Run binary");
	//printf("Args: %s", args);
	//int pid = syscall_fork();
	//printf("Pid: %d\n", pid);
	/*
	if(pid == 0){
	    printf("Child");
	}
	else{
	    printf("Parent");
	}
	*/
	int pid = 0;
	if(pid == 0){
		char *arguments = args;
		if(strlen(args) == 0) {
			arguments = NULL;
		}
		
		char *cmd_arr[] = {command, arguments, NULL};
		char final_command[1024] = "\0";
		if(strcmp(command, "ls") == 1 || strcmp(command, "cat")) {
			strcpy(final_command, ROOTFS_BIN_PATH);
		} else {
			strcpy(final_command, bin_string);
		}
		strcat(final_command, command);
		printf("Final command: %s\n", final_command);
		//int ret = syscall_execvpe(final_command, cmd_arr, NULL);
		//syscall_exit(ret);
	} else if(pid > 0){
		if(bgprocess == 1){
		        printf("Background");
			return;
		}
		if(syscall_waitpid(pid, &status, 0) > 0){
			if(WIFEXITED(status) && !WEXITSTATUS(status)){
			} else{
			}
		}
	}
	
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
