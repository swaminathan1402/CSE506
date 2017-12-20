#include <sys/kprintf.h>
#include <sys/defs.h>
#include <stdarg.h>
#include <stdlib.h>

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

void initScreen(){
	register char *temp = (char *)VIDEO_MEM_BUF;
	*temp = '\0';
	return;
}

int findCursorX(){
	int pos = 0;
	register char *temp = (char *)VIDEO_MEM_BUF;
	 while(*temp){
		pos+=2;
		temp+=2;
	}
	return pos%160;
}

int findCursorY(){
	register char *temp = (char *)VIDEO_MEM_BUF;
	int pos = 0;
	while(*temp){
		pos+=2;
		temp+=2;
	}
	return pos/160;
}

void scroll(){
	register char *flusher = (char *)VIDEO_MEM_BUF;
	while(*flusher){
		*flusher = '\0';
		flusher+=2;
	}
	return;
}

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
	int pos_x = findCursorX();
	int pos_y = findCursorY();
	register char *temp2;
	if(pos_y >= 20){
		scroll();
		pos_x = 0;
		pos_y = 0;
	}
	temp2 = (char *)VIDEO_MEM_BUF + pos_y*160 + pos_x;
	
	va_list ap;
        int numberOfParams = 0;
        //int args[1024][2];
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
		    /*
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
		    */
                    numberOfParams++;
                    i++;
                }
            }
	    else if(fmt[i] == '\n' || fmt[i] == '\t'){
		numberOfParams++;
	    } 
        }
	va_start(ap, fmt);
	//register char *temp2 = (char *)0xb8000;
	const char volatile *ans;
	register const char *temp1 = fmt;

	int isFormatted = (numberOfParams > 0)? 1: 0;
	if(!isFormatted){
		for(; *temp1; temp1+=1, temp2+=2) 
		if(*temp1=='\b')
		{temp2 -=2; 
		 *temp2 ='\0';
		}
		else
		{
		*temp2 = *temp1;
		}
	}
	else {
		//for(j = 1; j<= numberOfParams; j++){
		for(; *temp1;){
		    //if(args[i][1] == P){
		    if(*temp1 == '%' && *(temp1 + 1) == 'p'){
			unsigned long p = va_arg(ap, unsigned long);
			ans = ulongtoa(p);
			for(; *ans; ans+=1, temp2+=2) *temp2 = *ans;
			temp1+=2;  // escaping % and the char after that 

		    } else if(*temp1 == '%' && *(temp1 + 1) == 'd'){//(args[i][1] == INT){
			int p = va_arg(ap, int);
			ans = itoa(p, 10);
			for(; *ans; ans+=1, temp2+=2) *temp2 = *ans;
			temp1+=2;
			
		    } else if(*temp1 == '%' && *(temp1 + 1) == 'x'){//(args[i][1] == X){
			//unsigned char* p = va_arg(ap, unsigned char*);
			int p = va_arg(ap, int);
			ans = itoa(p, 16);
			for(; *ans; ans+=1, temp2+=2) *temp2 = *ans;
			temp1+=2;

		    } else if(*temp1 == '%' && *(temp1 + 1) == 'c'){//(args[i][1] == C){
			char p = (char)va_arg(ap, int);
			ans = &p;
			*temp2 = *ans;
			temp2+=2;
			temp1+=2;

		    }  else if(*temp1 == '%' && *(temp1 + 1) == 's'){//(args[i][1] == STR){
			char *p = va_arg(ap, char *);
			ans = p;
			for(; *ans; ans+=1, temp2+=2) *temp2 = *ans;
			temp1+=2;

		    } else if(*temp1 == '\n'){
			*temp2 = '\0';
			pos_x=findCursorX();
			for (int i= pos_x; i<160; i=i+2)
			{
				*temp2= ' ';
				temp2= temp2+2;
			}
			temp1+=1;
		    } else if(*temp1=='\t'){
			for(int i =0; i<4;i++)
			{
				*temp2=' ';
				temp2 =temp2+2;

			}	

			temp1+=1;
		    } 
			else if(*temp1 =='\b')
			{
			temp2= temp2 -2;
			*temp2 ='\0';			
			}	
			else {
			*temp2 = *temp1;
			temp1+=1;
			temp2+=2;
		    }
		    //i++;
		  }
		//}
	}
	*temp2 = '\0';
	va_end(ap);
        return;
}
