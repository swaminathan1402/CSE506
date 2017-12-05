#ifndef _FILEDIRECTORY_H
#define  _FILEDIRECTORY_H
#include<sys/defs.h>

typedef struct 
{
char filename[100];
struct filedir* parent;
struct filedir* root;
struct filedir* children[10];
uint64_t type; //0: directory , 1:file
//uint64_t start_point_addr;
uint64_t child_count; 
}filedir;

filedir *fileDescriptor[100];


void print_File_Descriptor();
void create_File_Descriptor_Entry(char* ,int,int, filedir* );
filedir* findParent(char* ,int ); 
#endif
