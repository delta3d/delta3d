#include <prefix/dtcoreprefix-src.h>
#include <dtCore/timer.h>
#include <dtUtil/macros.h>

#ifdef DELTA_WIN32
   #define WIN32_LEAN_AND_MEAN
   #include <Windows.h>
   #undef GetClassName
   #undef SendMessage
   void dtCore::AppSleep(unsigned int milliseconds){Sleep(milliseconds);}
#else
   #include <unistd.h>
   void dtCore::AppSleep(unsigned int milliseconds)
   {
      if (milliseconds == 0)
      {
         // usleep with 0 is a no-op, but we WANT appsleep to be like a yield, so
         // sleeping for 1 microsecond should work around that.
         usleep(1);
      }
      else
      {
         usleep((milliseconds) * 1000);
      }
   }
#endif

namespace dtCore
{

//////////////////////////////////////////////////////////////////////////
Timer::Timer()
{

}

//////////////////////////////////////////////////////////////////////////
Timer::~Timer()
{

}

//////////////////////////////////////////////////////////////////////////
const Timer* Timer::Instance()
{
   static Timer sTimer;
   return &sTimer;
}

//////////////////////////////////////////////////////////////////////////
Timer_t Timer::Tick() const
{
   return mTimer.tick();
}

//////////////////////////////////////////////////////////////////////////
double Timer::DeltaSec(Timer_t t1, Timer_t t2) const
{
   return mTimer.delta_s(t1, t2);
}

//////////////////////////////////////////////////////////////////////////
double Timer::DeltaMil(Timer_t t1, Timer_t t2) const
{
   return mTimer.delta_m(t1, t2);
}

//////////////////////////////////////////////////////////////////////////
double Timer::DeltaMicro(Timer_t t1, Timer_t t2) const
{
   return mTimer.delta_u(t1, t2);
}

//////////////////////////////////////////////////////////////////////////
double Timer::DeltaNano(Timer_t t1, Timer_t t2) const
{
   return mTimer.delta_n(t1, t2);
}

//////////////////////////////////////////////////////////////////////////
double Timer::GetSecondsPerTick() const
{
   return mTimer.getSecondsPerTick();
}

//////////////////////////////////////////////////////////////////////////
double Timer::ElapsedSeconds() const
{
   return mTimer.time_s();
}

//////////////////////////////////////////////////////////////////////////
double Timer::ElapsedMilliseconds() const
{
   return mTimer.time_m();
}

//////////////////////////////////////////////////////////////////////////
double Timer::ElapsedMicroseconds() const
{
   return mTimer.time_u();
}

//////////////////////////////////////////////////////////////////////////
double Timer::ElapsedNanoseconds() const
{
   return mTimer.time_n();
}

}//namespace dtCore

