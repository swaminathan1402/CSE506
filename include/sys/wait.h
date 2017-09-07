#ifndef _WAIT_H
#define _WAIT_H

#define	_W_INT(i)	(i)
#define	_WSTATUS(x)	(_W_INT(x) & 0177)
#define WIFEXITED(x)	(_WSTATUS(x) == 0)
#define WEXITSTATUS(x)	(_W_INT(x) >> 8)

#endif 
