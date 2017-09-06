#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<unistd.h>
int main (int argc , char *argv[])
{
struct dirent *dir;
char buffer[1024];
getcwd(buffer,1024);

DIR *current =opendir(buffer);
while((dir = readdir(current))!=NULL)
{
printf("%s\t", dir->d_name);
}
closedir(current);
}

