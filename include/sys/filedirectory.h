#ifndef _FILEDIRECTORY_H
#define  _FILEDIRECTORY_H
#include<sys/defs.h>

typedef struct 
{
char *filename;
struct filedir* parent;
struct filedir* root;
struct filedir* children[20];
int type; //1: directory , 2:file
int child_count; 
uint64_t entry_pointer;
uint64_t ref_count;
uint64_t filesz;
}__attribute__((packed))filedir;

filedir *fileDescriptor;
filedir* currentfile;
char* currentPath ;

void print_File_Descriptor(filedir *);
filedir* create_file_entry(char *, int, int, uint64_t);
void create_File_Descriptor_Entry(char* ,int,int, filedir*,uint64_t );
int search(char *);
filedir* findParent(char* ,int, filedir* ); 
void increase_ref_count(int );
void decrease_ref_count(int );
int readDents(int , char*);
int readFromFile(int , char* ,int );
char* getCurrentPath(char*, int);
int setCurrentPath(char*);
#endif
