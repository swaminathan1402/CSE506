#include<sys/kprintf.h>
#include<sys/defs.h>
#include<sys/pic.h>

extern void timer_handler()
{
int temp , t;
static int volatile seconds_from_boot=0;
static int volatile timetick=0;
timetick+=1;
if(timetick%18==0)
{
	seconds_from_boot++;
//	kprintf("%d seconds from boot", seconds_from_boot);
}
temp=seconds_from_boot;

char *timer =(char*) (0xb8000+160*25-10);	
if(temp==0 )
{
	*timer ='0';

}
while(temp!=0)
{		
	t=temp%10;
	char buf =(char)(t + 48);	
	temp=temp/10;
	*timer = buf; 
	timer =timer-2;
}
end_of_interrupt(0x0);
return;
}

