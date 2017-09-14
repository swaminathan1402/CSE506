#include<kprintf.h>
#include<sys/defs.h>

extern void timer_handler()
{
static int seconds_from_boot=0;
static int timetick=0;
timetick++;
if(timetick%18==0)
{
	seconds_from_boot++;
	kprintf1("%d seconds from boot",seconds_from_boot ,100,20);
}
return;
}

