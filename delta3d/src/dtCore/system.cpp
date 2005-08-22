// system.cpp: implementation of the System class.
//
//////////////////////////////////////////////////////////////////////

#include "dtCore/system.h"
#include <dtUtil/log.h>
#include "dtCore/camera.h"


using namespace dtCore;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(System)

bool System::mInstanceFlag = false;
System* System::mSystem = NULL;

System::System():
mRunning(false)
{
   RegisterInstance(this);
}

System::~System()
{
   DeregisterInstance(this);

   mInstanceFlag = false;
}

System* System::Instance()
{
   if (!mInstanceFlag)
   {
      mSystem = new System();
      mSystem->SetName("System");
      mInstanceFlag = true;
   }
   return mSystem;
}

void System::Frame( const double deltaFrameTime)
{
   SendMessage("frame", (void*)&deltaFrameTime);

   for (int camIdx=0; camIdx<Camera::GetInstanceCount(); camIdx++)
   {
      Camera::GetInstance(camIdx)->Frame();
   }
}


void System::Run()
{
   mRunning = true;
   double deltaFrameTime = 0.f;
   Timer_t clockTime = 0;
   Timer_t lastTick = mClock.tick();

   while (mRunning)
   {	  
	  clockTime = mClock.tick();
	  deltaFrameTime = mClock.delta_s(lastTick, clockTime);

      PreFrame(deltaFrameTime);
      Frame(deltaFrameTime);
      PostFrame(deltaFrameTime);

	  lastTick = clockTime;
   }
   LOG_DEBUG("System: Exiting...");
   SendMessage("exit");
   LOG_DEBUG("System: Done Exiting.");

}

void System::Start()
{
   mRunning = true;
}

void System::Step()
{
   static Timer_t clockTime, last_clockTime, dt;

   if( ! mRunning )
      return;

   //mClock.update();
   clockTime = mClock.tick();
   dt = static_cast<Timer_t>( mClock.delta_s(last_clockTime, clockTime) ); 

   PreFrame(dt);
   Frame(dt);
   PostFrame(dt);

   last_clockTime = clockTime;
}

void System::Stop()
{
   mRunning = false;
}

void System::PreFrame( const double deltaFrameTime )
{
   SendMessage("preframe", (void*)&deltaFrameTime);
}

void System::PostFrame( const double deltaFrameTime )
{
   SendMessage("postframe", (void*)&deltaFrameTime);
}

void System::Config()
{
   for (int camIdx=0; camIdx<Camera::GetInstanceCount(); camIdx++)
   {
      Camera::GetInstance(camIdx)->Frame();
   }

   SendMessage("configure");
}
