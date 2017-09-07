#include <stdio.h>
#include <unistd.h>

char getchar(){
	char buffer[1];
	read(0, buffer, 1);
	//write(1, buffer, 1);
	return buffer[0];
}
