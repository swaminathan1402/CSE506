#include<sys/filedirectory.h>
#include<sys/string.h>
#include<sys/kprintf.h>
#include<sys/memory.h>
filedir* findParent(char* dir,int index, filedir *root)
{
	//char *parent = dir;
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

filedir* create_file_entry(char *filename, int index, int filesz ,uint64_t file){
	filename[strlen(filename)]='\0';
	filedir* temp = (filedir*)fileDescriptor+index;
	temp->filename = filename;
	temp->parent = NULL;
	temp->root = NULL;
	temp->child_count = 0;
	temp->ref_count=0;
	temp->entry_pointer=file; 
	if(filesz == 0)
		temp->type = 1; // directory
	else
		temp->type = 2; // file 
	temp->filesz= filesz;

	return temp;
}

int readfromFile(int index, char* buffer, int count )
{
	int i ;
	filedir* temp = (filedir*)fileDescriptor+index;
	if(temp->type==1)
	{
	return 0; 
	}
	else 
	{
		char* file_ptr= (char*)temp->entry_pointer;
		for( i =0; i<temp->filesz;i++){
			*(buffer +i)= *(file_ptr + i);
			if(i==count)
			break;
		}
		return temp->filesz;
	}


}

void increase_ref_count (int index)
{
filedir* temp = (filedir*) fileDescriptor +index;
temp->ref_count++;
}

void decrease_ref_count(int index)
{
filedir* temp = (filedir*) fileDescriptor +index;
temp->ref_count--;
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
	new_parent[len]= '\0';
    char *parent = new_parent;
    return parent;
}

int search( char* filename )
{
int i=0;
filedir* temp = (filedir*)fileDescriptor+i;  
while(temp->filename!= '\0')
{
if(strcmp(temp->filename, filename)==1)
return i;
i ++;
temp= (filedir*)fileDescriptor + i;
}
return -1;
}

void create_File_Descriptor_Entry(char* filename, int index ,int filesz, filedir* root, uint64_t file)
{
	/*
        filedir* temp_parent ;
	char* temp_filename= filename;
	char* temp_parentname ;
	char* parentname;
	filedir* temp = (filedir *)root + index; 
	*/
	filedir *new_file = create_file_entry(filename, index, filesz, file);
	//int i=0;
	if(filesz==0)
	{
		new_file->parent = (filedir *) root;
		new_file->root = (filedir*)root;
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

char* getCurrentPath(char* buffer ,int count)
{
	if(count>strlen(currentPath))
	{
	memcpy(buffer, currentPath,count);
	}
	else
	{
	memcpy(buffer, currentPath,strlen(currentPath));
	buffer[strlen(currentPath)]='\0';
	}
	return buffer; 
}

int  setCurrentPath(char* buffer)
{
	
      filedir* temp;
	if(strcmp (buffer, "..")==1)
	{
		if(strcmp(currentfile->filename, "root/")==1)
		return -1;
		else
		{
		temp =currentfile->parent;
		memcpy(buffer,temp->filename,strlen(temp->filename) );
		}

	}
	else if(strcmp(buffer, "/")==1 ||strcmp(buffer, "...")==1)
	{
		if(strcmp (currentfile->filename , "root/")==1)
		return -1;
		else 
		{
			temp= currentfile->root;
			memcpy(buffer, temp->filename ,strlen(temp->filename));
		}
	}
	else
 	{ 
	int index = search(buffer);	
	if(index==-1)
	{
	//kprintf("\nFile not present\n");
	return -1;
	}
	 temp=(filedir*)fileDescriptor+index;
	}

	if(temp->type==2)
	return -1;
	
	currentfile = temp; 
	memcpy(currentPath, buffer, strlen(buffer));
	currentPath[strlen(buffer)]='\0';
	return 1;
	
}
/*char*  converttoString(uint64_t address)
{
int i=0;
int temp;
char string[9]; 
	while(address!=0)
	{
	 	 temp= (address & 0xf) ;
		switch(temp)
		{
		case 0:
		string[i]='0';
		break;

		case 1: 
		string[i]='1';
		break;

		case 2:
		string[i]='2';
		break;

		case 3:
		string[i]='3';
		break;

		case 4:
		string[i]='4';
		break;

		case 5:
		string[i]='5';
		break;

		case 6:
		string[i]='6';
		break;

		case 7:
		string[i]='7';
		break;

		case 8:
		string[i]='8';
		break;

		case 9:
		string[i]='9';
		break;

		case 10:
		string[i]='a';
		break;

		case 11:
		string[i]='b';
		break;

		case 12:
		string[i]='c';
		break;

		case 13:
		string[i]='d';
		break;
		
		case 14:
		string[i]='e';
		break;
		
		case 15:
		string[i]='f';
		break;
	}	
	address= address>>4;
	i++;
	}	
 string[8]='\0';
return string;
}
*/

int readDents(int index, char* buffer)
{
	//char *temp_please = buffer;
	filedir* temp = (filedir*)fileDescriptor+ index;
	int bPos=0;
	int i;
	if(temp->child_count==0)
	{
	 return 0;
	}
	for(i=0;i< temp->child_count; i++)
	{
		
		filedir* temp1 = (filedir*)temp->children[i];
		char *temp2 = (temp1->filename);
		//*(buffer+bPos)=strlen(temp1->filename) +1;
		bPos++;
		kprintf("%s\n", temp2);
		

	}
	return bPos;		
}


void print_File_Descriptor(filedir* root)
{
	//kprintf("|Filename :%s , Type: %d Child count: %d|", root->filename, root->type, root->child_count);
	int child_count = root->child_count;
	for(int i=0; i<child_count; i++){
		print_File_Descriptor((filedir*) root->children[i]);
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
