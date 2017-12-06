#ifndef _FILEDIRECTORY_H
#define  _FILEDIRECTORY_H
#include<sys/defs.h>

typedef struct 
{
char *filename;
struct filedir* parent;
struct filedir* root;
struct filedir* children[10];
uint64_t type; //1: directory , 2:file
uint64_t child_count; 
}__attribute__((packed))filedir;

filedir *fileDescriptor;


void print_File_Descriptor(filedir *);
filedir* create_file_entry(char *, int, int);
void create_File_Descriptor_Entry(char* ,int,int, filedir* );
filedir* findParent(char* ,int, filedir* ); 
#endif
