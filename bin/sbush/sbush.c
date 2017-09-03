#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#define REL '~'
#define CURRENT '.'

char shell[255] = "sbush>";
char *shell_text = "sbush";
char *shell_sign = ">";
char *space = " ";
char *bin_string = "/bin/";
char dollar_PATH[1024];
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

	char *formatted_command = (char *)malloc(sizeof(char) * (j));
	for(int i=0; i<j; i++){
		formatted_command[i] = command[i];
		//printf("%s\n", &command[i]);
	}
	//char *formatted_command = command;
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
	char *basename;	
	// if the name contains only '..', then we need to get the basename of the directory
	if((strlen(name) == 2) && (name[0] == CURRENT) && (name[1] == CURRENT)){
		size_t size;
		basename = getcwd(basename, size); 
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
		printf("%s\n", shell);
		return ;
	}
	if(strcmp(type, "cd") == 0){
		temp = _getBaseName(directory);
		strcpy(shell, shell_text);
		strcat(shell, space);
		strcat(shell, temp);
		strcat(shell, shell_sign);
		printf("%s\n", shell);
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
		exit(ret);
	} else if(pid > 0){
		
		if(bg_process ==1)
		{	
			printf("%d \n", pid);
			printf("%s \n",shell);
			return ;
		}
		
		pid_t wait_status = waitpid(pid, &status, 0);
		//https://www.gnu.org/software/libc/manual/html_node/Exit-Status.html#Exit-Status
		if(WIFEXITED(status) && wait_status){
			if(WEXITSTATUS(status) == 255){
				printf("sbush: %s: command not found...\n", command);
				printf("%s \n", shell);
			} else /*if(WEXITSTATUS(status) == 0)*/ {
				printf("\n%s \n", shell);
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
		exit(ret);
	} else if(pid > 0){
		if(waitpid(pid, &status, 0) > 0){
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
				 	DIR* dir =opendir(assignment[k]);
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
				DIR* dir = opendir(assignment[k]);
				if (dir)
				{
					if(k==0)
					strcpy(dollar_PATH,assignment[0]);
					else
					{
					strcat(dollar_PATH,":");
					strcat(dollar_PATH,assignment[k]);
					}
					setenv	("PATH",dollar_PATH,1);
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

void interpretCommand(char *query){
	// printf("query is %s\n", query);
	char command[100];
	char arguments[1024];
	int j=0;
	int i=0;
	int bg_process=0; 
	while(query[j] != '\0' && query[j] != ' '){
		command[j] = query[j];
		j++;
	}
	command[j] = '\0';
	j++;  // point to the arguments if any

	while(query[j] != '\0'){
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
	if(strcmp(command, "pwd") == 0) {
		char *buffer = NULL;
		size_t size;
		buffer = getcwd(buffer, size);
		printf("%s %s \n", shell, buffer);
	} else if(strcmp(command, "exit") == 0) {
		run_status = 0;
	}
	else if(strcmp(command, "cd") == 0){
		char *directory = arguments;
		// If we find any wildcard entry like '~' or '.' 
		if(directory[0] == REL /*|| 
			(directory[0] == CURRENT && directory[1] == '\0') ||
				(directory[0] == CURRENT && directory[1] != '\0' && directory[1] != CURRENT) */){
			int j = 0;
			char temp[255];
			// copy the path name following the REL(~) sign
			for(int i=1; i<strlen(directory); i++){
				temp[j] = directory[i];
				j++;
			}
			temp[j] = '\0';
			if(directory[0] == REL) strcpy(directory, getenv("HOME"));
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
			// printf("directory changed \n");
			modifyShellPrompt(directory, "cd");
		} else {
			printf("sbush: cd: %s: No such file or directory\n", directory);
			printf("%s \n", shell);
		}
	}
	else if(strcmp(command,"export" )==0){
	 int result=parseExportArguments(arguments);	
		if(result==0)
		printf ("\nmEnvironment variable not set");
		printf("%s \n", shell);
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

char* commandParser(){
	char ch;
	int i=0;
	char command[1024];
	ch = '\0';
	while(ch != '\n'){
		ch = getchar();
		command[i] = ch;
		i++;
	}

	command[i] = '\0';
	command[i-1] = '\0';
	char *cmd = _removeSpaces(command);
	return cmd;
}

int main(int argc, char* argv[]) {
	/*
      printf("sbush> %s\n", pwd);
	*/
	if(argc >= 2){
		runScripts(argv);
	}
	//dollar_PATH = //(char *)malloc(sizeof(getenv("PATH")+1024));
	else {
		strcpy(dollar_PATH, getenv("PATH")); 
		char *envold =strdup(getenv("PATH"));//save old environment
		printf("%s \n",dollar_PATH);
		char basename[1024];
		if(getcwd(basename, sizeof(basename)) != NULL){
			modifyShellPrompt(basename, "cd");
		}
		while(run_status){
			char *command = commandParser();
			interpretCommand(command);
		}
		setenv("PATH",envold,1);
	}

  return 0;
}
