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
	char *temp_filename= filename;
	char* temp_parentname ;
	char* parentname;
	filedir* temp = (filedir* )fileDescriptor+index; 
	if(filesz==0)
	{
		temp->type=0;
		temp->parent =root;
		temp->root=root;
		//temp->start_point_addr		
		root->children[root->child_count++]=temp;
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
		*temp_parentname = *temp_filename++; 
		temp_parent = findParent(parentname , index);	
		temp->parent= temp_parent;
		temp_parent->children[temp_parent->child_count++]= temp;
		temp->type=1;
		temp-> root= root;			
	}
		
}


void print_File_Descriptor()
{
	int i=0;
	while(fileDescriptor[i]->filename!=NULL)
	{
		kprintf("\nFilename :%s , Type: %d", fileDescriptor[i]->filename, fileDescriptor[i]->type);
		i++;
	}
}
