#include "dtCore/deltatimer.h"

using namespace dtCore;
using namespace osg;

#ifdef WIN32

#include <sys/types.h>
#include <fcntl.h>
#include <windows.h>
#include <winbase.h>
DeltaTimer::DeltaTimer()
{
   _useStandardClock = false;
   //_useStandardClock = true;

   if (_useStandardClock)
   {
#if defined(__CYGWIN__)
      _secsPerTick = 1e-6; // gettimeofday()'s precision.
#else
      _secsPerTick = (1.0 / (double) CLOCKS_PER_SEC);
#endif
   }
   else
   {
      // use a static here to ensure that the time to determine
      // the cpu frequency is not incurred more than once
      // per app execution.
      static double _tempSecsPerClick=0.0;
      if (_tempSecsPerClick==0.0)
      {
         // QueryPerformanceCounter under Windows 2000 Service Pack 3
         // two 2.4 GHz cpus was timed to take about 70 times longer
         // than the RDTSC assembly instruction sequence, but if
         // that counter is available we use it to quickly determine
         // the Time Stamp Counter rate, quickly as in 240 microseconds
         LARGE_INTEGER frequency;
         if(QueryPerformanceFrequency(&frequency))
         {
            //#define DEBUG_TIME_OUTPUT 1
            LARGE_INTEGER QPCstart, QPCstop,
               QPCstartAfter, QPCstopAfter;
            Timer_t TSCstart, TSCstop;
            double QPCSecsPerClock = 1.0/frequency.QuadPart;
            double elapsed, last, current, bound;

            QueryPerformanceCounter(&QPCstart);
            TSCstart=tick();
            QueryPerformanceCounter(&QPCstartAfter);

            current = 0;
            do
            {
               // store the seconds per clock
               last = current;
               // read the clocks
               QueryPerformanceCounter(&QPCstop);
               TSCstop=tick();
               QueryPerformanceCounter(&QPCstopAfter);
               // average before and after to approximate reading
               // both clocks at the same time
               elapsed = ((QPCstop.QuadPart+QPCstopAfter.QuadPart)
                  -(QPCstart.QuadPart+QPCstartAfter.QuadPart))/2.0
                  *QPCSecsPerClock;
               // TSC seconds per clock
               current = elapsed / (TSCstop-TSCstart);
               // calculate a bound to check against
               bound = current/1000000;
               // break if current-bound<last && current+bound>last
            }while(current-bound>last || current+bound<last);
            _tempSecsPerClick = current;
#ifdef DEBUG_TIME_OUTPUT
            fprintf(stderr, "current %e, last %e\n",
               1.0/current, 1.0/last);
            fprintf(stderr, "Total Time %e\n",
               (QPCstop.QuadPart-QPCstart.QuadPart)*QPCSecsPerClock);
#endif
         }
#ifndef DEBUG_TIME_OUTPUT
         else
#endif
         {
            Timer_t start_time = tick();
            Sleep (1000);
            Timer_t end_time = tick();

            _tempSecsPerClick = 1.0/(double)(end_time-start_time);
#ifdef DEBUG_TIME_OUTPUT
            fprintf(stderr, "Sec delay rate %e\n",
               1.0/_tempSecsPerClick);
#endif
         }
      }
      _secsPerTick = _tempSecsPerClick;
   }
}
#endif //WIN32