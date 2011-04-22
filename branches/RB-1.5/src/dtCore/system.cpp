// system.cpp: implementation of the System class.
//
//////////////////////////////////////////////////////////////////////

#include <prefix/dtcoreprefix-src.h>
#include <dtCore/system.h>
#include <dtUtil/log.h>
#include <dtCore/camera.h>
#include <dtCore/cameragroup.h>
#include <dtCore/deltawin.h>
#include <ctime>

using namespace dtUtil;

namespace dtCore
{

IMPLEMENT_MANAGEMENT_LAYER(System)

bool System::mInstanceFlag = false;
System* System::mSystem = 0;

System::System():
   mSimulationTime(0.0),
   mTimeScale(1.0f),
   mRunning(false),
   mShutdownOnWindowClose(true),
   mPaused(false)
{
   mTickClockTime = mClock.Tick();
   mDt = 0.0;
   RegisterInstance(this);
}

System::~System()
{
   DeregisterInstance(this);

   mInstanceFlag = false;
}

System& System::GetInstance()
{
   if( !mInstanceFlag )
   {
      mSystem = new System();
      mSystem->SetName("System");
      mInstanceFlag = true;
   }
   return *mSystem;
}

void System::Destroy()
{
}

void System::SetPause( bool paused )
{
   //don't send out a message unless it actually changes.
   if( mPaused == paused )
   {
      return;
   }
      
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

void System::Frame(const double deltaSimTime, const double deltaRealTime)
{
   double userData[2] = { deltaSimTime, deltaRealTime };
   SendMessage( "frame", userData );

   CameraFrame();
}

void System::Pause( const double deltaRealTime )
{
   SendMessage( "pause", const_cast<double*>(&deltaRealTime) );      
   
   CameraFrame();
}

///private
void System::SystemStep()
{
   Timer_t lastClockTime  = mTickClockTime;
   mTickClockTime = mClock.Tick();
   mDt = mClock.DeltaSec(lastClockTime, mTickClockTime);

   if( mPaused )
   {
      Pause(mDt);
   }
   else
   {
      //scale time.
      double mSimDt = mDt * mTimeScale;         
      mSimulationTime += mSimDt;
      mRealClockTime  += Timer_t(mDt * 1000000);
      mSimulationClockTime += Timer_t(mSimDt * 1000000); 

      PreFrame(mSimDt, mDt);
      Frame(mSimDt, mDt);
      PostFrame(mSimDt, mDt);
   }

}

void System::StepWindow()
{
   SystemStep();

   if( mShutdownOnWindowClose )
   {
      bool renderSurfaceIsRunning = false;
      for( int i = 0; i < DeltaWin::GetInstanceCount() && !renderSurfaceIsRunning; i++ )
      {
         renderSurfaceIsRunning = renderSurfaceIsRunning || DeltaWin::GetInstance(i)->GetRenderSurface()->isRunning();
      }

      mRunning = mRunning && renderSurfaceIsRunning;
   }

   for( int i = 0; i < DeltaWin::GetInstanceCount(); i++ )
   {
      DeltaWin::GetInstance(i)->Update();
   }
}


void System::Run()
{
   mRunning = true;
   mTickClockTime = mClock.Tick();
   time_t realTime;
   time(&realTime); 
   mRealClockTime = realTime * 1000000;
   mSimulationClockTime = mRealClockTime;

   //This should have been ifdef'd, not commented out.
   #ifdef __APPLE__   
   for( int i = 0; i < DeltaWin::GetInstanceCount(); i++ )
   {
      Producer::RenderSurface* rs = DeltaWin::GetInstance(i)->GetRenderSurface();
      rs->useConfigEventThread(false);
      rs->realize();
      rs->waitForRealize();
      rs->fullScreen(false);
      rs->startThread();
   }
   #endif
   
   while( mRunning )
   {	  
      StepWindow();     
   }

   LOG_DEBUG("System: Exiting...");
   SendMessage("exit");
   LOG_DEBUG("System: Done Exiting.");
}

void System::Start()
{
   mRunning = true;
   mTickClockTime = mClock.Tick();
   time_t realTime;
   time(&realTime); 
   mRealClockTime = realTime * 1000000;
   mSimulationClockTime = mRealClockTime;
}

void System::Step()
{
   static bool first = true;

   if ( !mRunning )
   {
      return;
   }

   if (first)
   {
      mTickClockTime = mClock.Tick();
      time_t realTime;
      time(&realTime); 
      mRealClockTime = realTime * 1000000;
      mSimulationClockTime = mRealClockTime;
      first = false;
   }

   SystemStep();
}

void System::Stop()
{
   mRunning = false;
}

void System::PreFrame( const double deltaSimTime, const double deltaRealTime )
{
   double userData[2] = { deltaSimTime, deltaRealTime };
   SendMessage("preframe", userData);
}

void System::PostFrame( const double deltaSimTime, const double deltaRealTime )
{
   double userData[2] = { deltaSimTime, deltaRealTime };
   SendMessage("postframe", userData);
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

}