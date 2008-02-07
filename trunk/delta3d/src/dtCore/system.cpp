// system.cpp: implementation of the System class.
//
//////////////////////////////////////////////////////////////////////

#include <prefix/dtcoreprefix-src.h>
#include <dtCore/system.h>
#include <dtUtil/log.h>
#include <dtUtil/bits.h>
#include <dtCore/view.h>
#include <dtCore/deltawin.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>
#include <dtCore/keyboardmousehandler.h> //due to include of scene.h
#include <dtCore/keyboard.h>//due to include of scene.h

#include <osgViewer/GraphicsWindow>
#include <ctime>

using namespace dtUtil;

namespace dtCore
{
   IMPLEMENT_MANAGEMENT_LAYER(System)

   bool System::mInstanceFlag = false;
   System* System::mSystem = 0;

   ////////////////////////////////////////////////////////////////////////////////
   System::System()
   : mRealClockTime(0)
   , mSimulationClockTime(0)
   , mLastDrawClockTime(0)
   , mSimulationTime(0.0)
   , mCorrectSimulationTime(0.0)
   , mFrameStep(1.0f/60.0f)
   , mTimeScale(1.0f)
   , mDt(0.0)
   , mMaxTimeBetweenDraws(100000)
   , mUseFixedTimeStep(false)
   , mRunning(false)
   , mShutdownOnWindowClose(true)
   , mPaused(false)
   , mWasPaused(false)
   , mSystemStages(STAGES_DEFAULT)
   {
      mTickClockTime = mClock.Tick();
      RegisterInstance(this);
   }

   ////////////////////////////////////////////////////////////////////////////////
   System::~System()
   {
      DeregisterInstance(this);

      mInstanceFlag = false;
   }

   ////////////////////////////////////////////////////////////////////////////////
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

   ////////////////////////////////////////////////////////////////////////////////
   void System::Destroy()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::SetSimulationTime(double newTime) 
   { 
      mSimulationTime = newTime;
      mCorrectSimulationTime = newTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
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

   ////////////////////////////////////////////////////////////////////////////////
   bool System::GetPause() const
   {
      return mPaused;
   }


   ////////////////////////////////////////////////////////////////////////////////
   void System::SetSystemStages( SystemStageFlags stages )
   {
      mSystemStages = stages;
   }

   ////////////////////////////////////////////////////////////////////////////////
   System::SystemStageFlags System::GetSystemStages() const
   {
      return mSystemStages;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::Frame(const double deltaSimTime, const double deltaRealTime)
   {
      if (dtUtil::Bits::Has(mSystemStages, System::STAGE_FRAME))
      {
         double userData[2] = { deltaSimTime, deltaRealTime };
         SendMessage( "frame", userData );
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::Pause( const double deltaRealTime )
   {
      SendMessage( "pause", const_cast<double*>(&deltaRealTime) );      
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::SystemStepFixed()
   {
      double mSimDt = mDt * mTimeScale;         
      if(mWasPaused == false)
      {
         mCorrectSimulationTime += mSimDt;
      }
      else
      {
         mCorrectSimulationTime += mFrameStep;
         mWasPaused = false;
      }

      if (mCorrectSimulationTime + 0.001f < mSimulationTime + mFrameStep)
      {
         // we tried a sleep here, but even passing 1 millisecond was to long.
#ifndef DELTA_WIN32
         AppSleep(1);
#endif
         return;
      }

      mSimulationTime += mFrameStep;
      mSimulationClockTime += Timer_t(mFrameStep * 1000000); 

      PreFrame(mFrameStep, mDt);

      //if we're ahead of the desired sim time, then draw.
      if (mSimulationTime >= mCorrectSimulationTime 
         || (mRealClockTime - mLastDrawClockTime) > mMaxTimeBetweenDraws)
      {
         mLastDrawClockTime = mRealClockTime;
         FrameSynch(mFrameStep, mDt);
         Frame(mFrameStep, mDt);
      }
      PostFrame(mFrameStep, mDt);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ///private
   void System::SystemStep()
   {
      Timer_t lastClockTime  = mTickClockTime;
      mTickClockTime = mClock.Tick();
      mDt = mClock.DeltaSec(lastClockTime, mTickClockTime);

      if( mPaused )
      {
         mWasPaused = true;
         Pause(mDt);
         FrameSynch(0.0, mDt);
         Frame(0.0, mDt);
      }
      else
      {
         double mSimDt = mDt * mTimeScale;         
         mRealClockTime  += Timer_t(mDt * 1000000);

         if(!mUseFixedTimeStep)
         {
            mSimulationTime += mSimDt;
            mSimulationClockTime += Timer_t(mSimDt * 1000000); 

            PreFrame(mSimDt, mDt);
            FrameSynch(mSimDt, mDt);
            Frame(mSimDt, mDt);
            PostFrame(mSimDt, mDt);
         }
         else
         {
            SystemStepFixed();
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::StepWindow()
   {
      SystemStep();

      if( mShutdownOnWindowClose ) // FIXME how to check if GraphicsWindow is always running ??
                                   // this implementation in really the good way
      {
         bool areGraphicsWindow = false;
         for( int i = 0; i < DeltaWin::GetInstanceCount() && !areGraphicsWindow; i++ )
         {
             areGraphicsWindow = areGraphicsWindow || DeltaWin::GetInstance(i)->GetOsgViewerGraphicsWindow()->valid();
         }

         mRunning = mRunning && areGraphicsWindow;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::Run()
   {
      mRunning = true;
      mTickClockTime = mClock.Tick();
      time_t realTime;
      time(&realTime); 
      mRealClockTime = realTime * 1000000;
      mLastDrawClockTime = mRealClockTime;
      mSimulationClockTime = mRealClockTime;

      while( mRunning )
      {	  
         StepWindow();
      }

      LOG_DEBUG("System: Exiting...");
      SendMessage("exit");
      LOG_DEBUG("System: Done Exiting.");
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::Start()
   {
      mRunning = true;
      mTickClockTime = mClock.Tick();
      time_t realTime;
      time(&realTime); 
      mRealClockTime = realTime * 1000000;
      mLastDrawClockTime = mRealClockTime;
      mSimulationClockTime = mRealClockTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
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
         mLastDrawClockTime = mRealClockTime;
         mSimulationClockTime = mRealClockTime;
         first = false;
      }

      SystemStep();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::Stop()
   {
      mRunning = false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::FrameSynch( const double deltaSimTime, const double deltaRealTime )
   {
      if (dtUtil::Bits::Has(mSystemStages, System::STAGE_FRAMESYNCH))
      {
         double userData[2] = { deltaSimTime, deltaRealTime };
         SendMessage("framesynch", userData);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::PreFrame( const double deltaSimTime, const double deltaRealTime )
   {
      if (dtUtil::Bits::Has(mSystemStages, System::STAGE_PREFRAME))
      {
         double userData[2] = { deltaSimTime, deltaRealTime };
         SendMessage("preframe", userData);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::PostFrame( const double deltaSimTime, const double deltaRealTime )
   {
      if (dtUtil::Bits::Has(mSystemStages, System::STAGE_POSTFRAME))
      {
         double userData[2] = { deltaSimTime, deltaRealTime };
         SendMessage("postframe", userData);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::Config()
   {
      if (dtUtil::Bits::Has(mSystemStages, System::STAGE_CONFIG))
      {
         SendMessage("configure");
      }
   }
}
