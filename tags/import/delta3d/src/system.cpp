// system.cpp: implementation of the System class.
//
//////////////////////////////////////////////////////////////////////

#include "system.h"
#include "notify.h"
#include "camera.h"

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

System* System::GetSystem(void)
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

   while (mRunning)
   {
      clock.update();
      deltaFrameTime = clock.getDeltaTime();

      PreFrame(deltaFrameTime);
      Frame(deltaFrameTime);
      PostFrame(deltaFrameTime);
   }

}

void System::Start()
{
   mRunning = true;
}

void System::Step()
{
   if( ! mRunning )
      return;

   clock.update();

   PreFrame(clock.getDeltaTime());
   Frame(clock.getDeltaTime());
   PostFrame(clock.getDeltaTime());
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
