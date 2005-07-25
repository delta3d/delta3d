#include <sys/time.h>

double Prof_get_time(void)
{
   struct timeval tv;
   gettimeofday(&tv, 0);
   return double(tv.tv_sec*1000000+tv.tv_usec);
}
