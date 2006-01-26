#include <iostream>
#include <string>
#include <dtCore/timer.h>
#include <dtUtil/log.h>

using namespace dtCore;
using namespace dtUtil;

// borrowed from osg/Timer.cpp...

const Timer* Timer::instance()
{
   static Timer s_timer;
   return &s_timer;
}

#ifdef WIN32

#include <sys/types.h>
#include <fcntl.h>
#include <windows.h>
#include <winbase.h>
Timer::Timer()
{
   LARGE_INTEGER frequency;
   if(QueryPerformanceFrequency(&frequency))
   {
      _secsPerTick = 1.0/(double)frequency.QuadPart;
   }
   else
   {
      _secsPerTick = 1.0;
      LOG_ERROR("Timer::Timer() unable to use QueryPerformanceFrequency, ");
      LOG_ERROR("timing code will be wrong.");
   }
}

Timer_t Timer::tick() const
{
   LARGE_INTEGER qpc;
   if (QueryPerformanceCounter(&qpc))
   {
      return qpc.QuadPart;
   }
   else
   {
      LOG_ERROR("Timer::Timer() unable to use QueryPerformanceFrequency, ");
      LOG_ERROR("timing code will be wrong.");
      return 0;
   }
}

#else

#include <sys/time.h>

Timer::Timer( void )
{
   _secsPerTick = (1.0 / (double) 1000000);
}

Timer_t Timer::tick() const
{
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return ((osg::Timer_t)tv.tv_sec)*1000000+(osg::Timer_t)tv.tv_usec;
}
#endif
