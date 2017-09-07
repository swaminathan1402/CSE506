#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
// #include <sys/types.h>
#include <dirent.h>
#include <syscall.h>
#define REL '~'
#define CURRENT '.'

char shell[255] = "\nsbush>";
char *shell_text = "sbush";
char *shell_sign = ">";
char *space = " ";
char *bin_string = "/bin/";
char dollar_PATH[1024];
char HOME[1024];
int run_status = 1;
int ps1_enabled= 0;

char *_removeSpaces(char command[]){
	// Removes unncessary spaces from the command. 
	// It basically cleans the command.
	int j=0;
	int i=0;
	int spaces =0;
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
	//printf("%d difference \n", j);

	//char *formatted_command = (char *)malloc(sizeof(char) * (j));
	/*
	char formatted_command[1024];
	for(int i=0; i<j; i++){
		formatted_command[i] = command[i];
	}
	*/
	char *formatted_command = command;
	return formatted_command;
}



char *_getBaseName(char directory[]){
	// This is utility to get the basename of the directory
	// the directory name could be of these types: /home/people or /home/people/ or "/home/name\'s\\of\ people"


	// printf("%s is what we get\n", directory);
	int size = strlen(directory);
	int j=size-1;
	char name[1024];
	while(j>=0 && directory[j] == '/'){
		j--;
	}
	int last_mark = j;
	while(j >=0 && directory[j] != '/'){
		j--;
	}
	int counter = 0;
	for(int i=j+1; i<=last_mark; i++)
		name[counter++] = directory[i];
	name[counter] = '\0';
	char *basename = NULL;	
	size_t size_omg= 1024;
	// if the name contains only '..', then we need to get the basename of the directory
	if((strlen(name) == 2) && (name[0] == CURRENT) && (name[1] == CURRENT)){
		basename = getcwd(basename, size_omg); 
		basename = _getBaseName(basename);
	} else {
		basename = name;
	}
	// printf("%s: Basename\n", basename);
	return basename;
}


void modifyShellPrompt(char directory[], char *type){
	char *temp;
	// printf("trying to change shell prompt %s\n", type);
	if(ps1_enabled==1)
	{
		//printf("\n%s", shell);
		syscall_write(1, shell, strlen(shell));
		return ;
	}
	if(strcmp(type, "cd") == 0){
		temp = _getBaseName(directory);
		strcpy(shell, shell_text);
		strcat(shell, space);
		strcat(shell, temp);
		strcat(shell, shell_sign);
		syscall_write(1, shell, strlen(shell));
		//printf("\n%s", shell);
	}
	
}

void runBinary(char *command, char *arguments,int bg_process){
	int status;
	pid_t pid;
	// TODO: pass a signal for invalid command 
	// sbush: blah: command not found... [have something like this]
	pid = fork();
	if(pid == 0){
		// this is a child process
		if(strlen(arguments) == 0)
			arguments = NULL;
		char *cmd[] = {command, arguments, (char *)0};
		char final_command[1024];
		strcpy(final_command, bin_string);
		strcat(final_command, command);
		int ret = execv(final_command, cmd);
		syscall_exit(ret);
	} else if(pid > 0){
		if(bg_process ==1)
		{	
			//printf("%d", pid);
			//printf("\n%s ",shell);

			//syscall_write(1, pid, sizeof(pid));
			syscall_write(1, shell, strlen(shell));
			return ;
		}
		
		pid_t wait_status = waitpid(pid, &status);//waitpid(pid, &status, 0);
		//https://www.gnu.org/software/libc/manual/html_node/Exit-Status.html#Exit-Status
		if(WIFEXITED(status) && wait_status){
			if(WEXITSTATUS(status) == 255){
				//printf("sbush: %s: command not found...", command);
				//printf("\n%s ", shell);
				char *error_msg = "sbush: %s: command not found...";
				syscall_write(1, error_msg, strlen(error_msg));
				syscall_write(1, shell, strlen(shell));
			} else /*if(WEXITSTATUS(status) == 0)*/ {
				//printf("\n%s ", shell);
				syscall_write(1, shell, strlen(shell));
			}
		}
	}
}

void runScripts(char *arguments[]){
	//char *filename = arguments[1];
	int status;
	pid_t pid = fork();
	//char *args[] = {"/bin/sh", filename, arguments+1};
	if(pid == 0){
		//int ret = execv("/bin/sh", args);
		int ret = execv("/bin/sh", arguments);
		syscall_exit(ret);
	} else if(pid > 0){
		//if(waitpid(pid, &status, 0) > 0){
		if(waitpid(pid, &status) > 0){
			//printf("%s \n", shell);
		}
	}
}


int parseExportArguments(char *argument)
{
	char variable[100];//Shell variable which is to be exported.
	char assignment[10][1024];// The assignment for the variable
	int i=0 ;
	int j=0;
	int k=0;
	while(argument[i] !='=' )
	{
		variable[i]= argument[i];
		i++;
	}
	variable[i]='\0';
	i++;
	if(strcmp(variable ,"PATH")==0)	
	{
		while(argument[i]!='\0')
		{
			assignment[k][j] =argument[i];
			i++;
			j++;
			if(argument[i]==':')
			{
				assignment[k][j]='\0';
				if(strcmp(assignment[k],"$PATH" )!=0 )  			
				{
				 	int dir =opendir(assignment[k]);
				 	if(dir)
					{	
						strcat(dollar_PATH,":");
						strcat(dollar_PATH ,assignment[k]);
					}		
					else
					{
					//	printf("\n Environment variable set to non-existent directory");
						return 0;
					}			
				}			
			j=0;k++;
			i++;
			}
			else if(argument[i]=='\0')
			{	
				assignment[k][j]='\0';	
				int dir = opendir(assignment[k]);
				if (dir)
				{
					if(k==0)
					strcpy(dollar_PATH,assignment[0]);
					else
					{
					strcat(dollar_PATH,":");
					strcat(dollar_PATH,assignment[k]);
					}
					//setenv	("PATH",dollar_PATH,1);
				//	printf("%s",dollar_PATH);
					return 1;
				}
				else
				{
			//	printf("\nEnvironment variable set to non-existent directory");
				return 0;	
				}	
			}
		}
	}
	else if(strcmp(variable,"PS1")==0)
	{
		ps1_enabled=1;
		while(argument[i]!='\0')
		{
	      		assignment[0][j]=argument[i];
			j++;
			i++;

		}
		assignment[0][j]='\0';
		strcpy(shell,assignment[0]);
		return 1;	
	}
	return 0;			

}

void runPipes(char commands[10][100], int no_of_commands){
	// *commands[] = {ls -ltr, grep word, wc -l};
	int fd[2];
	int input = 0,t;
	int status;
	for(int i=0; i<no_of_commands; i++){
		//if(syscall_pipe(fd)){
			//printf("pipe creation failed \n");
		//	char *pipe_fail_msg = "pipe creation failed\n";
		//	syscall_write(1, pipe_fail_msg, strlen(pipe_fail_msg));
		//}
		//printf("%d", x);
		syscall_write(1, "oh man\n", 8);
		if((t==syscall_pipe(fd)) == -1){
		syscall_write(1, (char *)t, 2);	
		return;
		}
		pid_t pid;
		pid = syscall_fork();
		if(pid == 0){
			if(input != 0) syscall_dup2(input, 0); // if there is some output present from previous command
			if(i < no_of_commands-1) {// making sure only n-1 cmd outputs are written to the pipe. Last output will be written in the screen
				syscall_dup2(fd[1], 1);
			}
			char arguments[1024];
			char cmd[100];
			char *clean_command = _removeSpaces(commands[i]); //removes all unnecessary spaces
			int j=0, k=0;
			while(clean_command[j] != '\0' && clean_command[j] != ' '){
				cmd[j] = clean_command[j];
				j++;
			}
			cmd[j] = '\0';
			j+=1;
			while(clean_command[j] != '\0'){
				arguments[k] = clean_command[j];
				j++;
				k++;
			}
			arguments[k] = '\0';
			char *args = arguments;
			if(strlen(arguments) == 0)
                        	args = NULL;
                        char *cmd_arr[] = {cmd, args, (char *)0};
                        char final_command[1024];
                        strcpy(final_command, bin_string);
                        strcat(final_command, cmd);
			syscall_write(1, final_command, strlen(final_command));
                        int ret = execv(final_command, cmd_arr);
			syscall_exit(ret);
		} else if(pid > 0){
			//if(waitpid(pid, &status, 0) > 0){
			if(waitpid(pid, &status) > 0){
				syscall_write(1, "parent is done waiting\n", 24);
				syscall_close(fd[1]); // close the write end of the pipe
				input = fd[0]; // store the output for later references 
			}
		}
	}
}

void interpretCommand(char *query){
	// printf("query is %s\n", query);
	if(strlen(query) == 0){
		//printf("\n%s ", shell);
	syscall_write(1, query, strlen(query));	
	syscall_write(1, shell, strlen(shell));
		return;
	}
	char command[100];
	char arguments[1024];
	char inst_array[10][100];
	int no_of_commands=1 ;
	int j=0;
	int i=0;
	int bg_process=0; 
	while (query[j]!='\0')
	{
	
		if(query[j]=='|')
		{ 
			inst_array[no_of_commands-1][i]='\0';
			no_of_commands++;
			i=0;
		}
		else
		{
			inst_array[no_of_commands-1][i]=query[j];
			i++;
		}	
		j++;
	}
	inst_array[no_of_commands-1][i]='\0';
	j=0;
	i=0;
	if(no_of_commands>1)
	{
	//	runPipe(inst_array);Run the pipe function to handle multiple pipes.
		/*	
		for(int k=0;k<no_of_commands;k++)
		{
		printf("\n%s",inst_array[k]);
		}
		printf("\n%s",shell);  // CURSOR IMPLEMENTATION
		*/
		runPipes(inst_array, no_of_commands);
		//printf("\n%s ", shell);
		syscall_write(1, shell, strlen(shell));
	}
	else
	{
    
		while(query[j] != '\0' && query[j] != ' ')
		{
			command[j] = query[j];
			j++;
		}
		command[j] = '\0';
		j++;  // point to the arguments if any

		while(query[j] != '\0')
		{
			arguments[i] = query[j];
			j++;
			i++;
		}
		if(arguments[i-1]=='&')
		{
			bg_process=1;	
			i--;
		}
		arguments[i] = '\0';
		if (strcmp(arguments, "$PATH")==0)
		{
			strcpy (arguments, dollar_PATH);
		}
		if(strcmp(arguments, "$PS1")==0)
		{
			strcpy (arguments,shell);
		}
		// printf("the parsed command is %s\n", command);
		// printf("the arguments are %s\n", arguments);	
		size_t size = 1024;
		if(strcmp(command, "pwd") == 0) {
			char *buffer = NULL;
			buffer = getcwd(buffer, size);

			//printf("\n%s ", buffer);
			//printf("\n%s ", shell);
			syscall_write(1, buffer, strlen(buffer));
			syscall_write(1, shell, strlen(shell));

		} else if(strcmp(command, "exit") == 0) {
			run_status = 0;
		}
		else if(strcmp(command, "cd") == 0){
			char *directory = arguments;
		// If we find any wildcard entry like '~' or '.' 
			if(strlen(arguments) == 0){
				char *home = HOME;
				strcpy(directory, home);	
			}
			else if(directory[0] == REL){
				int j = 0;
				char temp[255];
			// copy the path name following the REL(~) sign
				for(int i=1; i<strlen(directory); i++){
					temp[j] = directory[i];
					j++;
				}
				temp[j] = '\0';
				if(directory[0] == REL) strcpy(directory, HOME);
			/*			
			else {
				// can ignore this 
				size_t size;
				directory = NULL;
				directory = getcwd(directory, size);
			}
			*/
				strcat(directory, temp);
			// printf("Translated directory name: %s\n", directory);
			}
			int ret = chdir(directory);
			if(ret == 0){

			// printkf("directory changed \n");
				modifyShellPrompt(directory, "cd");
			} else {
				//printf("sbush: cd: %s: No such file or directory", directory);
				//printf("\n%s ", shell);
				char *error_msg = "sbush: cd: No such file or directory";
				syscall_write(1, error_msg, strlen(error_msg));
				syscall_write(1, shell, strlen(shell));
			}
		}
		else if(strcmp(command,"export" )==0){
		 	int result=parseExportArguments(arguments);	
			if(result==0){
				//printf ("\nEnvironment variable not set");
				char *error_msg = "Environment variable not set \n";
				syscall_write(1, error_msg, strlen(error_msg));
			}
			//printf("\n%s ", shell);
			syscall_write(1, shell, strlen(shell));
		}
	
/*
	else if(strcmp(command, "ls") == 0 || strcmp(command, "cat") == 0 || strcmp(command, "grep") == 0){
		runBinary(command, arguments);
		printf("\n%s \n", shell)
		}*/else {
			runBinary(command, arguments,bg_process);
		//printf("sbush: %s: command not found...\n", command);
		//printf("%s \n", shell);
		} 

	}
}

char* commandParser(){
	char ch = '\0';
	char command[1024];
	int i=0;
	while((ch=getchar()) != '\n'){
		command[i] = ch;
		i++;
	}
	char* command_ptr = command;
	return command_ptr;
}


char *envParser(char *env){
	char output[1024];
	int i =0, j=0;
	while(env[i] != '='){
		i++;
	}
	i+=1;
	
	while(env[i] != '\0'){
		output[j] = env[i];
		i++;
		j++;
	}
	output[j] = '\0';
	char *try = output;
	return try;
}

int main(int argc, char* argv[], char *envp[]) {
	/*
      printf("sbush> %s\n", pwd);
	*/
	if(argc >= 2){
		runScripts(argv);
	}
	//dollar_PATH = //(char *)malloc(sizeof(getenv("PATH")+1024));
	else {
		//dollar_PATH = (char *)malloc(sizeof(getenv("PATH")+1024));
		//strcpy(dollar_PATH, getenv("PATH")); 
		char *pathVariable = envParser(envp[9]);
		strcpy(dollar_PATH, pathVariable); 
		char *homeVariable = envParser(envp[12]);
		strcpy(HOME, homeVariable);
		//printf("%s",dollar_PATH);
		//syscall_write(1, dollar_PATH, strlen(dollar_PATH));
		syscall_write(1, shell, strlen(shell));
		char basename[1024];
		if(getcwd(basename, sizeof(basename)) != NULL){
			modifyShellPrompt(basename, "cd");
		}
		/*
		while(run_status){
			char *command = commandParser();
			syscall_write(1, command, strlen(command));
			interpretCommand(command);
		}
		*/

	//	char* command = commandParser();
	//	interpretCommand(command);
		//runBinary("ls",NULL, 0);  --works
		char command[10][100] = {
			"ls", "grep include"
		};
		runPipes(command, 2);
	}

  return 0;
}
