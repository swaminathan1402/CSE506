#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define REL '~'
#define CURRENT '.'

char shell[255] = "sbush>";
char *shell_text = "sbush";
char *shell_sign = ">";
char *space = " ";
int run_status = 1;


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
	if(strcmp(type, "cd") == 0){
		temp = _getBaseName(directory);
		strcpy(shell, shell_text);
		strcat(shell, space);
		strcat(shell, temp);
		strcat(shell, shell_sign);
		printf("%s\n", shell);
	}
}

void interpretCommand(char *query){
	// printf("query is %s\n", query);
	char command[100];
	char arguments[1024];
	int j=0;
	int i=0;
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
	arguments[i] = '\0';
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
	char basename[1024];
	if(getcwd(basename, sizeof(basename)) != NULL){
		modifyShellPrompt(basename, "cd");
	}
	while(run_status){
		char *command = commandParser();
		interpretCommand(command);
	}
  return 0;
}
