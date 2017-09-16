#include<sys/kprintf.h>
#include<sys/defs.h>
#include<sys/pic.h>
extern void timer_handler()
{
static int volatile seconds_from_boot=0;
static int volatile timetick=0;
timetick+=1;
if(timetick%18==0)
{
	seconds_from_boot++;
	kprintf("%d seconds from boot", seconds_from_boot);
}
end_of_interrupt(0x0);
return;
}

