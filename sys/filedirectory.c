#include<sys/defs.h>
#include<sys/filedirectory.h>
#include<sys/string.h>
#include<sys/kprintf.h>

filedir* findParent(char* dir,int index, filedir *root)
{
	for(int i=0;i<index;i++) 
	{
		filedir* temp= (filedir*)root+i;
		 if(strcmp(temp->filename , dir)==1)  // if it matches 
	 	{
			//kprintf("The parent of %s is %s\n", dir, temp->filename);
			return temp;
		}
	}
	return NULL;
}

filedir* create_file_entry(char *filename, int index, int filesz){
	filedir* temp = (filedir*)fileDescriptor+index;
	temp->filename = filename;
	temp->parent = NULL;
	temp->root = NULL;
	temp->child_count = 0;
	if(filesz == 0)
		temp->type = 1; // directory
	else
		temp->type = 2; // file 
	return temp;

}

char *extractParentFromFilename(char *filename){
    int len = strlen(filename);
    while(filename[len]!='/' && len >= 0){
        len--;
    }
    len++;
    char new_parent[len];
    for(int j=0; j<len; j++)
        new_parent[j] = filename[j];
    char *parent = new_parent;
    return parent;
}

void create_File_Descriptor_Entry(char* filename, int index ,int filesz, filedir* root)
{
	/*
        filedir* temp_parent ;
	char* temp_filename= filename;
	char* temp_parentname ;
	char* parentname;
	filedir* temp = (filedir *)root + index; 
	*/
	filedir *new_file = create_file_entry(filename, index, filesz);
	int i=0;
	if(filesz==0)
	{
		new_file->parent = root;
		new_file->root = root;
		root->children[root->child_count] = new_file;
		root->child_count++;
		/*
		temp->type=1;
		temp->parent = (uint64_t)root;
		temp->root=(uint64_t)root;
		while(*filename!= '\0')
		{
		temp->filename[i]= *filename;
		filename++;
		i++;
		}
		temp->filename[i]='\0';
		root->children[root->child_count++]=(uint64_t*)temp;
		*/
	}
	else
	{
		/*
		parentname =temp_parentname;
		while(*temp_filename != '/')	
		{
		 *temp_parentname = *temp_filename ;
		 temp_filename++;
		 temp_parentname++;	
		}
		*temp_parentname = *temp_filename; 
		temp_filename++;
		*/
		char *parentName = extractParentFromFilename(filename);
		filedir *temp_parent = findParent(parentName , index, root);	
		
		new_file->parent= temp_parent;
		temp_parent->children[temp_parent->child_count++]= new_file;
		new_file->root = root;
		//kprintf("%s", temp_parent->filename);
		/*
		temp->type=2;
		temp->root=(uint64_t) root;
		while (*temp_filename!='\0')
		{
			temp->filename[i] = *temp_filename;
			i++;
			temp_filename++;
		}			
			temp->filename[i]='\0';
		*/
	}
	return;	
}


void print_File_Descriptor(filedir *root)
{
	kprintf("|Filename :%s , Type: %d Child count: %d|", root->filename, root->type, root->child_count);
	int child_count = root->child_count;
	for(int i=0; i<child_count; i++){
		print_File_Descriptor(root->children[i]);
	}
	/*
	int i=0;
	while(i<=7)
	{
		kprintf("\nFilename :%s , Type: %d",(fileDescriptor+i)->filename,(fileDescriptor+i)->type);
		i++;
	}
	*/
}
