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

char *removeSpaces(char command[]){
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
	char *formatted_command = command;
	return formatted_command;
}

char *commandParser(){
	char ch;
	int i=0;
	char command[1024];
	ch = '\0';
	while(ch != '\n'){
		ch = getchar();
		command[i++] = ch;
	}
	command[i-1] = '\0';
	printf("@commandParser Before removing spaces: %s\n", command);
	char *cmd = removeSpaces(command);
	printf("@commandParser After removing spaces: %s\n", cmd);
	return cmd;
}

char *_getLastName(char directory[]){
	// the directory name could be of these types: /home/people or /home/people/ or "/home/name\'s\\of\ people"
	// return: people
	printf("%s is what we get\n", directory);
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
	char *last_name = name;
	printf("%s: Last name\n", last_name);
	return last_name;
}


void modifyShellPrompt(char directory[], char *type){
	char *temp;
	printf("trying to change shell prompt %s\n", type);
	if(strcmp(type, "cd") == 0){
		temp = _getLastName(directory);
		strcpy(shell, shell_text);
		strcat(shell, space);
		strcat(shell, temp);
		strcat(shell, shell_sign);
		printf("%s\n", shell);
	}
}

void interpretCommand(char query[]){
	char command[1024];
	char arguments[1024];
	int j=0;
	int i=0;
	while(query[j] != '\0' && query[j] != ' '){
		command[j] = query[j];
		j++;
	}
	command[j] = '\0';
	j++;  // point to the arguments if any
	printf("command is %s\n", command);	
	while(query[j] != '\0'){
		arguments[i] = query[j];
		j++;
		i++;
	}
	arguments[i] = '\0';
	printf("argument is %s\n", arguments);	
	if(strcmp(command, "pwd") == 0) {
		char *buffer = NULL;
		size_t size;
		buffer = getcwd(buffer, size);
		printf("%s %s \n", shell, buffer);
	}
	else if(strcmp(command, "cd") == 0){
		printf("it is a cd command\n");
		char *directory = arguments;
		if(directory[0] == REL || directory[0] == CURRENT){
			int j = 0;
			char temp[255];
			// copy the path name following the REL(~) sign
			for(int i=1; i<strlen(directory); i++){
				temp[j] = directory[i];
				j++;
			}
			temp[j] = '\0';
			if(directory[0] == REL) strcpy(directory, getenv("HOME"));
			else {
				size_t size;
				directory = NULL;
				directory = getcwd(directory, size);
				printf("IDIOT %s\n", directory);
			}
			strcat(directory, temp);
			printf("Translated directory name: %s\n", directory);
		}
		int ret = chdir(directory);
		if(ret == 0){
			printf("directory changed \n");
			modifyShellPrompt(directory, "cd");
		} else {
			printf("sbush: cd: %s: No such file or directory\n", directory);
		}
	}
}

int main(int argc, char* argv[]) {
  //char *directory = "/home/nghosh/";
  //int ret = chdir(directory);
	/*
  if(ret == 0){
    char pwd[1024];
    if(getcwd(pwd, sizeof(pwd)) != NULL)
      printf("sbush> %s\n", pwd);
  }
	*/
	while(1){
		char *command = commandParser();
		interpretCommand(command);
	}
  return 0;
}
