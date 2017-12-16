#include<stdio.h>
#include<syscall.h>

int yield(){
	__asm__ __volatile__(
		
		"movq $24, %%rax;"
		"int $0x80;"
		:
		:
		:"rax"
	);
	return 1;
}


void clean_zombies(){
	__asm__ __volatile__(
		"movq $130, %%rax;"
		"int $0x80;"
		:
		:
		:
	);
}


int main(){
	//syscall_write(0,"sbush>",6);
	while(1){
		clean_zombies();
		yield();
	}
	return 0;
}
