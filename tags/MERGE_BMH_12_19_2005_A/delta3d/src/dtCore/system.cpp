// system.cpp: implementation of the System class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/system.h>
#include <dtUtil/log.h>
#include <dtCore/camera.h>
#include <dtCore/cameragroup.h>

using namespace dtCore;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(System)

bool System::mInstanceFlag = false;
System* System::mSystem = NULL;

System::System():
   mRunning(false),
   mShutdownOnWindowClose(true),
   mPaused(false)
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

void System::SetPause( bool paused )
{
   mPaused = paused;
   
   if( mPaused )
   {
      SendMessage( "pause_start" );
   }
   else
   {
      SendMessage( "pause_end" );
   }
}

bool System::GetPause() const
{
   return mPaused;
}

void System::Frame( const double deltaFrameTime)
{
   SendMessage( "frame", (void*)&deltaFrameTime );

   CameraFrame();
}

void System::Pause( const double deltaFrameTime )
{
   SendMessage( "pause", (void*)&deltaFrameTime );      
   
   CameraFrame();
}

void System::Run()
{
   mRunning = true;
   mLastClockTime = mClock.tick();

   #ifdef __APPLE__
   //this loop is a workaround for problems on Mac OS X where startThread is not called
   //in the render surface, which makes "isRunning" return false
   //it should be removed once the OSX bugs with RS are fixed.
   for( int i = 0; i < DeltaWin::GetInstanceCount(); i++ )
   {
      Producer::RenderSurface* rs = DeltaWin::GetInstance(i)->GetRenderSurface();
      rs->useConfigEventThread(false);
      rs->realize();
      rs->waitForRealize();
      rs->fullScreen(false);
      rs->startThread();
   }
   #endif //__APPLE__
   
   while( mRunning )
   {	  
	   mClockTime = mClock.tick();
	   mDt = mClock.delta_s(mLastClockTime, mClockTime);

      if( mPaused )
      {
         Pause(mDt);
      }
      else
      {
         PreFrame(mDt);
         Frame(mDt);
         PostFrame(mDt);
      }

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
   CameraFrame();

   SendMessage("configure");
}

void System::CameraFrame()
{
   Camera::GetCameraGroup()->Frame();
}
