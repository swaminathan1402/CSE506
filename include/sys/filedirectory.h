#ifndef _FILEDIRECTORY_H
#define  _FILEDIRECTORY_H
#include<sys/defs.h>

typedef struct 
{
char filename[16];
uint64_t parent;
uint64_t root;
uint64_t children[10];
uint64_t type; //1: directory , 2:file
//uint64_t start_point_addr;
uint64_t child_count; 
}__attribute__((packed))filedir;

filedir *fileDescriptor;


void print_File_Descriptor();
void create_File_Descriptor_Entry(char* ,int,int, filedir* );
filedir* findParent(char* ,int ); 
#endif
