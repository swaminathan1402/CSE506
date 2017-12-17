#include <stdlib.h>
#include<stdio.h>

long* sp;


void _start(void) {
 int argc = -7;
  char* argv[] = {"None"};
  char* envp[] = {"None"};


__asm__ __volatile__(
    "mov %%rsp, %%rcx;"
    :"=c"(sp)
    :
    :
);

argc = *((int*)(sp+1));

if(argc==0){
    main(argc, NULL, NULL);
}
else{
    for(int i=0; i<argc;i++){
        argv[i] = ((char*)(sp+2+8*i));
    
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
















