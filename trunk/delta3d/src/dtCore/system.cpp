// system.cpp: implementation of the System class.
//
//////////////////////////////////////////////////////////////////////

#include "dtCore/system.h"
#include "dtCore/notify.h"
#include "dtCore/camera.h"


using namespace dtCore;

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

System* System::GetSystem()
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
   double clockTime = 0.f;
   double lastTick = clock.tick();

   while (mRunning)
   {	  
	  clockTime = clock.tick();
	  deltaFrameTime = clock.delta_s(lastTick, clockTime);

      PreFrame(deltaFrameTime);
      Frame(deltaFrameTime);
      PostFrame(deltaFrameTime);

	  lastTick = clockTime;
   }
   Notify(DEBUG_INFO, "System: Exiting...");
   SendMessage("exit");
   Notify(DEBUG_INFO, "System: Done Exiting.");

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

   //clock.update();
   clockTime = clock.tick();
   dt = clock.delta_s(last_clockTime, clockTime); 

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
