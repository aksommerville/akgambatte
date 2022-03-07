#include <unistd.h>
#include <sys/time.h>
#include "usec.h"

usec_t getusecs() {
  struct timeval tv={0};
  gettimeofday(&tv,0);
  return tv.tv_sec*1000000+tv.tv_usec;
}

void usecsleep(usec_t usecs) {
  usleep(usecs);
}
