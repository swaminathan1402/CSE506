#include<stdio.h>
#include<stdlib.h>


int main (int argc, char*argv[])
{
int temp=1;
char str[1024];
while (temp!=argc)
{
	FILE	*filename = fopen(argv[temp],"r");
	while (fgets(str,1024,filename )!=NULL)
	{
	printf("%s",str);
	}
	fclose(filename);
        temp++;
}

}


