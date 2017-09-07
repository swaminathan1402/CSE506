#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <syscall.h>

char getchar(){
	char buffer[1];
	read(0, buffer, 1);
	return buffer[0];
		
}

char *gets(char *s){
	read(0, s, 1024);
	char *string = s;
	return string;	
}

	/*
	char ch='\0';
	char command[1024];
	int i=0;
	while((ch = getchar()) != '\n'){
		command[i] = ch;
		i++;
	}
	//command[i] = '\0';
	printf("%s", command);
	return 0;
	*/
