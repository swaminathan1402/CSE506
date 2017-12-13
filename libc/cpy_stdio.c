#include <stdarg.h>
#include <stdlib.h>
#include <syscall.h>

char volatile *ulongtoa(unsigned long value){
	//char *buffer = (char *)malloc(sizeof(char) * 1024);
	char volatile buffer[1024] = "\0"; 
	char base16[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
	unsigned long volatile temp = value;
	unsigned long volatile remainder;
	int index = 0;
	while(temp != 0){
		temp = temp / 16;
		index++;
	}
	buffer[index] = '\0';
	index--;
	while(index >= 0){
		remainder = value - 16 * (value / 16);
		if(remainder < 10){
			buffer[index] = base16[remainder];
		} else {
			buffer[index] = (remainder - 10) + 'a';
		}
		value = value/16;
		index--;
	}
	char volatile *p = buffer;
	return p;
}

char volatile *itoa(int value, int base){
	//char *buffer = (char *)malloc(sizeof(char) * 1024);
	char volatile buffer[1024];
	char base10[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	char base16[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	int volatile index = 0;
	int volatile final, temp, store, limit;
	if(value==0)
	{
	buffer[0]='0';
	buffer[1]='\0';
	char volatile *p = buffer;
	return p;
	}	
	if(value < 0){
		buffer[index] = '-';
		value *= -1;
	}
	temp = value;
	while(temp != 0){
		temp = temp / base;
		index++;
	}
	limit = (buffer[0] == '-')? 1 : 0;
	if(limit == 0) index-=1;
	final = index + 1;
	while(value != 0/*index >= limit*/){
		store = value % base;
		if(base == 10){
			buffer[index] = base10[store];
		}
		else if(base == 16) {
			buffer[index] = base16[store];
		}
		value = value / base;
		index--;
	}
	buffer[final] = '\0';
	char volatile *p = buffer;
	return p;
}


int printf(const char *fmt, ...){
	
	va_list ap;
        int numberOfParams = 0;
	int strLen = 0;

	while(fmt[strLen] != '\0') strLen++;
	//syscall_write(0, fmt, strLen);

        //syscall_write(0, fmt, strLen);
        
        for(int i=0; i<strLen; i++){
            if(fmt[i] == '%' && fmt[i+1] != '\0'){
                int next_pos = i+1;
                if(fmt[next_pos] == 'd' || 
                    fmt[next_pos] == 's' || 
                    fmt[next_pos] == 'p' || 
                    fmt[next_pos] == 'x' || 
                    fmt[next_pos] == 'c') {
                    numberOfParams++;
                    i++;
                }
            }/*
	    else if(fmt[i] == '\n' || fmt[i] == '\t'){
		numberOfParams++;
	    }*/
        }
	va_start(ap, fmt);
	const char volatile *ans;
	register const char *temp1 = fmt;
	int isFormatted = (numberOfParams > 0)? 1: 0;
	register char *temp2 = "\0";
	register char *ret_str = temp2;
        //syscall_write(0, temp1, strLen);
        int new_str_len = 0; 
	if(!isFormatted){
            syscall_write(0, fmt, strLen);
	}
	
	else {
	    //syscall_write(0, temp1, strLen);
	    syscall_write(0, "Xere", 4);
		for(; *temp1 != NULL;){
		    if(*temp1 == '%' && *(temp1 + 1) == 'p'){
			syscall_write(0, "\n", 1);
			unsigned long p = va_arg(ap, unsigned long);
			ans = ulongtoa(p);
			for(; *ans; ans+=1, temp2+=1) *temp2 = *ans;
			temp1+=2;  // escaping % and the char after that 

		    } else if(*temp1 == '%' && *(temp1 + 1) == 'd'){//(args[i][1] == INT){
			syscall_write(0, "dere\n", 5);
			int p = va_arg(ap, int);
			ans = itoa(p, 10);
			for(; *ans; ans+=1, temp2+=1) *temp2 = *ans;
			temp1+=2;
			
		    } else if(*temp1 == '%' && *(temp1 + 1) == 'x'){//(args[i][1] == X){
			syscall_write(0, "\n", 1);
			int p = va_arg(ap, int);
			ans = itoa(p, 16);
			for(; *ans; ans+=1, temp2+=1) *temp2 = *ans;
			temp1+=2;

		    } else if(*temp1 == '%' && *(temp1 + 1) == 'c'){//(args[i][1] == C){
			syscall_write(0, "\n", 1);
			char p = (char)va_arg(ap, int);
			ans = &p;
			*temp2 = *ans;
			temp2+=1;
			temp1+=2;

		    }  else if(*temp1 == '%' && *(temp1 + 1) == 's'){//(args[i][1] == STR){
			syscall_write(0, "\n", 1);
			char *p = va_arg(ap, char *);
			ans = p;
			for(; *ans; ans+=1, temp2+=1) *temp2 = *ans;
			temp1+=2;

		    }else{
		        *temp2 = *temp1;
		        temp1 += 1;
		        temp2 += 1;
		    }
		  }
	
	}
	*temp2 = '\0';
	int n_str_len = 0;
	while(ret_str[n_str_len] != '\0') n_str_len++;
	syscall_write(0, "output\n", 7);
        syscall_write(0, ret_str, n_str_len);

	va_end(ap);
	
        return;

}
