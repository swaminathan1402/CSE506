#include<sys/defs.h>
#include<sys/filedirectory.h>
#include<sys/string.h>
#include<sys/kprintf.h>

filedir* findParent(char* dir,int index)
{
	for(int i=0;i<index;i++) 
	{
		filedir* temp= (filedir*)fileDescriptor+i;
		 if(strcmp(temp->filename , dir)==0)
	 	{
			return temp;
		}
	}
	return NULL;
}


void create_File_Descriptor_Entry(char* filename, int index ,int filesz, filedir* root)
{
        filedir* temp_parent ;
	char* temp_filename= filename;
	char* temp_parentname ;
	char* parentname;
	int i=0;
	filedir* temp = (filedir *)root + index; 
	if(filesz==0)
	{
		temp->type=1;
		temp->parent = (uint64_t)root;
		temp->root=(uint64_t)root;
		//temp->start_point_addr		
		while(*filename!= '\0')
		{
		temp->filename[i]= *filename;
		filename++;
		i++;
		}
		temp->filename[i]='\0';
		root->children[root->child_count++]=(uint64_t*)temp;
		
	}
	else
	{
		parentname =temp_parentname;
		while(*temp_filename != '/')	
		{
		 *temp_parentname = *temp_filename ;
		 temp_filename++;
		 temp_parentname++;	
		}
		*temp_parentname = *temp_filename; 
		temp_filename++;
		temp_parent = findParent(parentname , index);	
		
		temp->parent=(uint64_t) temp_parent;
		temp_parent->children[temp_parent->child_count++]= (uint64_t*)temp;
		temp->type=2;
		temp-> root=(uint64_t) root;
		while (*temp_filename!='\0')
		{
			temp->filename[i] = *temp_filename;
			i++;
			temp_filename++;
		}			
			temp->filename[i]='\0';
	}
	return;	
}


void print_File_Descriptor()
{
	int i=0;
	while(i<=10)
	{
		kprintf("\nFilename :%s , Type: %d",(fileDescriptor+i)->filename,(fileDescriptor+i)->type);
		i++;
	}
}
