#ifndef _FILEDESCRIPTOR_H
#define _FIILEDESCRIPTOR_H
#include<sys/tarfs.h>
#include<sys/elf64.h>

struct file
{
 struct file* parent;
 struct file* root ; 
 struct file* child[10] ;
 char filename[100];
 int type;
}fileDescriptor[100];

#endif

