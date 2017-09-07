#include <stdlib.h>

void _start(void) {
  // call main() and exit() here
  char *env[] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, "PATH=/home/nghosh/bin:/home/nghosh/.local/bin:/shared/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games", NULL, NULL, "HOME=/home/nghosh", NULL};
  main(0, NULL, env);
}
