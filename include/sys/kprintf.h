#ifndef __KPRINTF_H
#define __KPRINTF_H

#define VIDEO_MEM_BUF 0xffffffff800b8000

void kprintf(const char *fmt, ...);
void initScreen();
#endif
