#include <iostream>
#include <string>
#include "dtCore/timer.h"
#include "dtCore/notify.h"

using namespace dtCore;
//using namespace osg;
using namespace std;

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
      Notify(NOTICE)<<"Error: Timer::Timer() unable to use QueryPerformanceFrequency, "<<std::endl;
      Notify(NOTICE)<<"timing code will be wrong, Windows error code: "<<GetLastError()<<std::endl;
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
      Notify(NOTICE)<<"Error: Timer::Timer() unable to use QueryPerformanceCounter, "<<std::endl;
      Notify(NOTICE)<<"timing code will be wrong, Windows error code: "<<GetLastError()<<std::endl;
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