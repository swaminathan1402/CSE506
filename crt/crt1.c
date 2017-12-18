#include <stdlib.h>
#include<stdio.h>

long* sp;


void _start(void) {
 int argc = -7;
  char* argv[] = {"None"};
  char* envp[] = {"None"};

__asm__ __volatile__(
    "movq %%rsp, %%rcx;"
    :"=c"(sp)
    :
    :
);

argc = *((int*)(sp+3));



if(argc==0){
    main(argc, NULL, NULL);
}
else{
    //int i=0;
    //for(i=0; i<argc;i++){
    //while(i < argc){
    //    argv[i] = ((char*)(sp+4+8*i));
    //    i++;
    //}
    if(argc == 1){
        argv[0] = ((char*)(sp+4+8*0));
    
    }
    else if(argc == 2){
        argv[0] = ((char*)(sp+4+8*0));
        argv[1] = ((char*)(sp+4+8*1));
    }
    else if(argc == 3){
        argv[0] = ((char*)(sp+4+8*0));
        argv[1] = ((char*)(sp+4+8*1));
        argv[2] = ((char*)(sp+4+8*2));
    }
    else if(argc == 4){
        argv[0] = ((char*)(sp+4+8*0));
        argv[1] = ((char*)(sp+4+8*1));
        argv[2] = ((char*)(sp+4+8*2));
        argv[3] = ((char*)(sp+4+8*3));
    }
    else if(argc == 5){
        argv[0] = ((char*)(sp+4+8*0));
        argv[1] = ((char*)(sp+4+8*1));
        argv[2] = ((char*)(sp+4+8*2));
        argv[3] = ((char*)(sp+4+8*3));
        argv[4] = ((char*)(sp+4+8*4));
    }
    main(argc, argv, NULL);
}
/*
__asm__ __volatile__(
    "addq $0x28,%%rsp;"
    "movq (%%rsp),%%rdi;"
      //  "movq 16(%%rsp,%%rdi,8 ),%%rdx;"
    "movq %%rdi, %0;"
  //  "movq %%rdx, %2;"
    :"=m"(argc) 
    :
    :"memory"
 );
printf("%d  ", argc );
//while(1);	
*/
  main(argc, argv, envp);
  __asm__(
     "movq $60, %%rax;"
	"movq $1, %%rbx;"
	"int $0x80;"
	:
	:
	:
	);
}
















