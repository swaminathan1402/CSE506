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
int isPiped(char *);
void modifyShellPrompt(char *, char *);
char *getBaseName(char *);
void runScripts(char arguments[10][1024]);
void runScript(char *);
void parseExportArguments(char *);


char *commandParser(){
	char buffer[200] = "\0";
	scanf("%s", buffer);
	char *cmd = sanitize(buffer);
	return cmd;
}

void handle_pipes(char *command, int numberOfCommands){
	int fd[2];
	int input = 0;
	int status;
	
	int cmd_index = 0;
	char cmd_arr[10][100];
	int i=0, j=0;
	// aggregating the commands
	while(command[j] != '\0'){
		if(command[j] == '|'){
			cmd_index++;
			cmd_arr[cmd_index-1][i] = '\0';
			i = 0;
		} else {
			cmd_arr[cmd_index][i] = command[j];
			i++;
		}

		j++;
	}
	cmd_arr[cmd_index][i] = '\0';
	
	for(int i=0; i<numberOfCommands; i++){
		syscall_pipe(fd); // TODO
		pid_t pid = syscall_fork(); // TODO
		if(pid == 0){
			if(input != 0) {
				syscall_dup2(input, 0); // TODO
			}
			if(i < numberOfCommands - 1){
				syscall_dup2(fd[1], 1);
			}
			char *temp = cmd_arr[i];
			char *clean_command = sanitize(temp);
			char function[1024];
			char arguments[1024];
			int p=0,q=0;
			while(clean_command[p] != '\0' && clean_command[p] != ' '){
				function[p] = clean_command[p];
				p++;
			}
			function[p] = '\0';
			p++;
			while(clean_command[p] != '\0'){
				arguments[q] = clean_command[p];
				p++;
				q++;
			}
			arguments[q] = '\0';
			char *args = arguments;
			if(strlen(arguments) == 0)
				args = NULL;
			char *command_args[] = {function, args, (char *)0};
			char final_command[1024];
			printf("function before strcmp: %s\n", function);
			if(strcmp(function, "ls") == 1 || strcmp(function, "cat") == 1) {
				strcpy(final_command, ROOTFS_BIN_PATH);
				printf("ls final command: %s", final_command);
			} else {
				strcpy(final_command, bin_string);
			}
			strcat(final_command, function);
			int ret = syscall_execvpe(final_command, command_args, NULL); // TODO
			syscall_exit(ret);  // TODO
		} else if(pid > 0){
			if(syscall_waitpid(pid, &status, 0) > 0){ // TODO
				if(WIFEXITED(status) && !WEXITSTATUS(status)){ //TODO
					syscall_close(fd[1]); //TODO
					input = fd[0];
				} else {
					return;
				}
			} else {
				return;
			}
		}
	}
}

void runBinary(char *command, char *args, int bgprocess){
	int status;
	printf("Run binary");
	printf("Args: %s", args);
	//int pid = syscall_fork();
	int pid = 0;
	printf("About to call fork: %d", pid);
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
	//char *tempCommand = command;
	//strcpy(tempCommand, command);
	char tempCommand[40] = "\0";
	char function[40] = "\0";
	char arguments[40] = "\0";
	strcpy(tempCommand, command);
	if(strlen(command) == 0){
		return;
	}
	else if(strcmp(tempCommand, "exit") == 1){
		run_status = 0;
		return;
	}
	int isCommandsPiped = isPiped(tempCommand);
	if(isCommandsPiped > 1){
		// pipe commands
		handle_pipes(command, isCommandsPiped);
		return;
	}
	// actual command interpretation starts here
	//int size = strlen(command);
	//char *function = (char *)malloc(sizeof(char) * 1024); 
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
	if(strcmp(arguments,"$PATH")==1)
	{
		strcpy(arguments, dollar_PATH);
	}
	if(strcmp(arguments,"$PS1")==1)
	{
		strcpy(arguments, shell);
	}
	if(strcmp(function, "cd") == 1) {
		//handle_cd(arguments); TODO
		printf("Path: %s", arguments);
		int ret = syscall_chdir(arguments);  // TODO: implement syscall_chdir
		/*
		if(ret == 0){
			//modifyShellPrompt(arguments, "cd"); // modify shell prompt 
		}*/
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
	if(strcmp (function, "export")==1)
	{		
		parseExportArguments(arguments);
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
void aggregateEnvVariables(char *home_env){
	/*
	char temp_path[1024] = "\0";
	int i=0, j=0;
	while(path_env[i] != '='){
		i++;
	}
	i+=1;
	while(path_env[i] != '\0'){
		temp_path[j] = path_env[i];
		i++;
		j++;
	}
	temp_path[j] = '\0';
	*/
	char temp_home[1024] = "\0";
	int i=0;
	int j=0;
	while(home_env[i] != '='){
		i++;
	}
	i++;
	while(home_env[i] != '\0'){
		temp_home[j] = home_env[i];
		i++;
		j++;
	}
	temp_home[j] = '\0';

	//strcpy(dollar_PATH, temp_path);
	strcpy(HOME, temp_home);
	strcpy(ROOTFS_BIN_PATH, HOME);
	char *bpath = "/temp/CSE506/rootfs/bin/";
	strcat(ROOTFS_BIN_PATH, bpath);
	//printf("PATH: %s",ROOTFS_BIN_PATH);
}
int main(int argc, char* argv[], char* envp[]){
        //printf("S");
	int envp_c = 0;
        char env_parameter_list[15][1024];
        for(int i=0; i<15; i++){
        	int envp_p = 0;
        	while(envp[0][envp_c] != '\0'){
        	    env_parameter_list[i][envp_p] = envp[0][envp_c];
        	    envp_c++;
        	    envp_p++;
                                                                                          
        	}  
        	env_parameter_list[i][envp_c] = envp[0][envp_c];
        	envp_c++;
        }
	char *h = env_parameter_list[13];
        aggregateEnvVariables(h);

	if(argc > 1){
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
		printf("%s", parameter_list[1]);
		runScripts(parameter_list);
		return 0;
	}
	
	//strcpy(HOME, parameter_list[13]);
	//strcpy(dollar_PATH, parameter_list[9]);
	//char *b = "/workdir/rootfs/bin/";
	//strcpy(ROOTFS_BIN_PATH, HOME);
	//strcat(ROOTFS_BIN_PATH, b);


	printf("%s", shell);
//	strcpy(dollar_PATH ,envp[9]);  //PATH is in envp[9]
	while(run_status){
		//char buffer[1024];
		//int size = syscall_read(0, buffer, 1024);
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

 void modifyShellPrompt(char *to, char *type){
	if(ps1_enabled==1)	
	{
		printf("%s", shell);
		return;
	}
		
	if(strcmp(type, "cd") == 1){
		//TODO: fill this up
		char *temp = getBaseName(to);
		strcpy(shell, shell_text);
		strcat(shell, space);
		strcat(shell, temp);
		strcat(shell, shell_sign);
		
	}
 }

 int isPiped(char *command){
	int numberOfPipes = 1;
	int i =0;
	while(command[i] != '\0'){
		if(command[i] == '|') {
			numberOfPipes++;
		}
		i++;
	}
	return numberOfPipes;
 }

char *getBaseName(char *directory){
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
	char *basename = (void *)0;	
	// if the name contains only '..', then we need to get the basename of the directory
	if((strlen(name) == 2) && (name[0] == DOT) && (name[1] == DOT)){
		basename = syscall_getcwd(basename, 1024); 
		basename = getBaseName(basename);
	} else {
		basename = name;
	}
	return basename;	
}

void parseExportArguments ( char * arguments) {
	int i=0;
	int j=0;
	int k=0;
	char envivariable[40] = "\0";
	char attribute[10][100];	
	while (arguments[i]!='=')
	{
		envivariable[i]= arguments[i];
		i++;
	}	
	envivariable[i]='\0';
	i++;
	if(strcmp(envivariable , "PATH")==1)	
	{
		while (arguments[i]!='\0')
		{
			attribute[k][j]=arguments[i];  // separate by colon
			i++;j++;
			if(arguments[i]==':')
			{
				attribute[k][j]='\0';
				if(strcmp(attribute[k],"$PATH")!=1) 
				{
					strcat(dollar_PATH,":");
					strcat(dollar_PATH, attribute[k]);
				}
				j=0;k++;
				i++;
			}
		else if(arguments[i]=='\0')
			{	
			
			attribute[k][j]='\0';
			if(k==0)
			strcpy(dollar_PATH , attribute[k]);
			else
			{
			strcat(dollar_PATH, ":");
			strcat(dollar_PATH,attribute[k]);
			}


			}

		}	
	}
	if(strcmp (envivariable, "PS1")==1)
	{
		ps1_enabled =1;
		while (arguments[i]!='\0')
		{

			attribute[0][j]=arguments[i];
			j++;i++;
		}
	   attribute[0][j]='\0';
	   strcpy(shell, attribute[0]);
	}
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

	/*
	int status;
	pid_t pid = syscall_fork();
	if(pid == 0){
		int ret = syscall_execvpe("/bin/sh", arguments, NULL);
		syscall_exit(ret);
	} else if(pid > 0){
		if(syscall_waitpid(pid, &status, 0) > 0){
		}
	}
	*/
}
