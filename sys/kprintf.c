#include <sys/kprintf.h>
#include <sys/defs.h>
#include <stdarg.h>
#define INT 1
#define STR 2
#define C 3
#define P 4
#define X 5

void kprintf(const char *fmt, ...)
{
	/*
	va_list ap;
	va_start(ap, fmt);
	register const char *temp = fmt;
	register char *temp2;
	for(temp2=(char *)0xb8000; *temp; temp+=1, temp2+=2) *temp2 = *temp;
	return;
	*/
	va_list ap;
        int numberOfParams = 0;
        int args[1024][2];
	int strLen = 0;

	while(fmt[strLen] != '\0') strLen++;

        for(int i=0; i<strLen; i++){
            if(fmt[i] == '%' && fmt[i+1] != '\0'){
                int next_pos = i+1;
                if(fmt[next_pos] == 'd' || 
                    fmt[next_pos] == 's' || 
                    fmt[next_pos] == 'p' || 
                    fmt[next_pos] == 'x' || 
                    fmt[next_pos] == 'c') {

                    args[numberOfParams][0] = i;
                    if(fmt[next_pos] == 'd'){
                        args[numberOfParams][1] = INT;
                    }else if(fmt[next_pos] == 's'){
                        args[numberOfParams][1] = STR;
                    }else if(fmt[next_pos] == 'p'){
                        args[numberOfParams][1] = P;
                    }else if(fmt[next_pos] == 'x'){
                        args[numberOfParams][1] = X;
                    }else if(fmt[next_pos] == 'c'){
                        args[numberOfParams][1] = C;
                    }
                    numberOfParams++;
                    i++;
                }
            } 
        }
	va_start(ap, fmt);
        register int j;
        int i=0;
	
	register char *temp2 = (char *)0xb8000;

        for(j = 1; j<= numberOfParams; j++){
            if(args[i][1] == P){
                int *p = va_arg(ap, int*);
            } else if(args[i][1] == INT){
                int p = va_arg(ap, int);
		//register const char *temp = (char *)p;
		//for(; *temp; temp+=1, temp2+=2) *temp2 = *temp;

            } else if(args[i][1] == X){
                unsigned char* p = va_arg(ap, unsigned char*);
            } else if(args[i][1] == C){
                char p = (char)va_arg(ap, int);
            }  else if(args[i][1] == STR){
                char *p = va_arg(ap, char *);
            }
            i++;
        }
        return;
}
