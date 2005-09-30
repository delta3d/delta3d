// system.cpp: implementation of the System class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/system.h>
#include <dtUtil/log.h>
#include <dtCore/camera.h>

using namespace dtCore;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(System)

bool System::mInstanceFlag = false;
System* System::mSystem = NULL;

System::System():
   mRunning(false),
   mShutdownOnWindowClose(true)
{
   mClockTime = 0;
   mLastClockTime = mClock.tick();
   mDt = 0.0;
   RegisterInstance(this);
}

System::~System()
{
   DeregisterInstance(this);

   mInstanceFlag = false;
}

System* System::Instance()
{
   if( !mInstanceFlag )
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

   for( int camIdx = 0; camIdx < Camera::GetInstanceCount(); camIdx++ )
   {
      Camera::GetInstance(camIdx)->Frame();
   }
}


void System::Run()
{
   mRunning = true;
   mLastClockTime = mClock.tick();

   while( mRunning )
   {	  
	   mClockTime = mClock.tick();
	   mDt = mClock.delta_s(mLastClockTime, mClockTime);

      PreFrame(mDt);
      Frame(mDt);
      PostFrame(mDt);

	   mLastClockTime = mClockTime;

      if( mShutdownOnWindowClose )
      {
         bool renderSurfaceIsRunning = false;
         for( int i = 0; i < DeltaWin::GetInstanceCount() && !renderSurfaceIsRunning; i++ )
         {
            renderSurfaceIsRunning = renderSurfaceIsRunning || DeltaWin::GetInstance(i)->GetRenderSurface()->isRunning();
         }

         mRunning = mRunning && renderSurfaceIsRunning;
      }
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
   static bool first = true;

   if( !mRunning )
   {
      return;
   }

   if(first)
   {
      mLastClockTime = mClock.tick();
      first = false;
   }

   mClockTime = mClock.tick();
   mDt = mClock.delta_s(mLastClockTime, mClockTime); 

   PreFrame(mDt);
   Frame(mDt);
   PostFrame(mDt);

   mLastClockTime = mClockTime;
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
   for( int camIdx = 0; camIdx < Camera::GetInstanceCount(); camIdx++ )
   {
      Camera::GetInstance(camIdx)->Frame();
   }

   SendMessage("configure");
}
