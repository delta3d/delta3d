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
   , mFrameTime(1.0/60.0)
   , mTimeScale(1.0)
   , mMaxTimeBetweenDraws(30000)
   , mAccumulationTime(0.0)
   , mSystemStages(STAGES_DEFAULT)
   , mUseFixedTimeStep(false)
   , mAccumulateLastRealDt(false)
   , mRunning(false)
   , mShutdownOnWindowClose(true)
   , mPaused(false)
   , mWasPaused(false)
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
   void System::SetShutdownOnWindowClose( bool shutdown )
   {
      mShutdownOnWindowClose = shutdown;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool System::GetShutdownOnWindowClose() const
   {
      return mShutdownOnWindowClose;
   }

   ////////////////////////////////////////////////////////////////////////////////
   double System::GetTimeScale() const
   {
      return mTimeScale;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::SetTimeScale(double newTimeScale)
   {
      mTimeScale = newTimeScale;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Timer_t System::GetRealClockTime() const
   {
      return mRealClockTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Timer_t System::GetSimulationClockTime() const
   {
      return mSimulationClockTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::SetSimulationClockTime(const dtCore::Timer_t &newTime)
   {
      mSimulationClockTime = newTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   double System::GetSimulationTime() const
   {
      return mSimulationTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   double System::GetSimTimeSinceStartup() const
   {
      return mSimTimeSinceStartup;
   }

   ////////////////////////////////////////////////////////////////////////////////
   double System::GetCorrectSimulationTime() const
   {
      return mCorrectSimulationTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::SetFrameRate(double newRate)
   {
      mFrameTime = 1.0/newRate;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::SetMaxTimeBetweenDraws(double newTime)
   {
      mMaxTimeBetweenDraws = newTime * 1000000.0;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::SetUseFixedTimeStep(bool value)
   {
      mUseFixedTimeStep = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   double System::GetFrameRate() const
   {
      return 1.0/mFrameTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool System::GetUsesFixedTimeStep() const
   {
      return mUseFixedTimeStep;
   }

   ////////////////////////////////////////////////////////////////////////////////
   double System::GetMaxTimeBetweenDraws() const
   {
      return mMaxTimeBetweenDraws/1000000.0;
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
   void System::SystemStepFixed(const double realDT)
   {
      mRealClockTime += Timer_t(realDT * 1000000);
      const double simDT = realDT * mTimeScale;

      const float simFrameTime = mFrameTime * mTimeScale;

      if (!mWasPaused)
      {
         mCorrectSimulationTime += simDT;
      }
      else
      {
         mCorrectSimulationTime += simFrameTime;
         mWasPaused = false;
      }

      if (mCorrectSimulationTime + 0.001f < mSimulationTime + simFrameTime)
      {
         // we tried a sleep here, but even passing 1 millisecond was to long.
         mAccumulateLastRealDt = true;
#ifndef DELTA_WIN32
         AppSleep(1);
#endif
         return;
      }

      mAccumulateLastRealDt = false;

      mSimulationTime      += simFrameTime;
      mSimTimeSinceStartup += simFrameTime;
      mSimulationClockTime += Timer_t(simFrameTime * 1000000);

      //const double realFrameTime = realDT + mAccumulationTime;
      const double realFrameTime = mFrameTime;
      PreFrame(simFrameTime, realFrameTime);

      //if we're ahead of the desired sim time, then draw.
      if (mSimulationTime >= mCorrectSimulationTime
         || (mRealClockTime - mLastDrawClockTime) > mMaxTimeBetweenDraws)
      {
         mLastDrawClockTime = mRealClockTime;
         FrameSynch(simFrameTime, realFrameTime);
         Frame(simFrameTime, realFrameTime);
      }
      PostFrame(simFrameTime, realFrameTime);

      mAccumulationTime = 0;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ///private
   void System::SystemStep()
   {
      const Timer_t lastClockTime  = mTickClockTime;
      mTickClockTime = mClock.Tick();

      const double realDT = mClock.DeltaSec(lastClockTime, mTickClockTime);
      if (mAccumulateLastRealDt)
      {
         mAccumulationTime += mClock.DeltaSec(lastClockTime, mTickClockTime);
      }

      if (mPaused)
      {
         mWasPaused = true;
         Pause     (realDT);
         FrameSynch(0.0, realDT);
         Frame     (0.0, realDT);
      }
      else
      {
         if (!mUseFixedTimeStep)
         {
            mWasPaused = false;

            // update real time variable(s)
            mRealClockTime  += Timer_t(realDT * 1000000);

            // update simulation time variable(s)
            const double simDT = realDT * mTimeScale;
            mSimulationTime      += simDT;
            mSimTimeSinceStartup += simDT;
            mSimulationClockTime += Timer_t(simDT * 1000000);

            PreFrame  (simDT, realDT);
            FrameSynch(simDT, realDT);
            Frame     (simDT, realDT);
            PostFrame (simDT, realDT);
         }
         else
         {
            SystemStepFixed(realDT);
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
   void System::InitVars()
   {
      mAccumulationTime = 0;
      mAccumulateLastRealDt = false;
      mTickClockTime = mClock.Tick();
      time_t realTime;
      time(&realTime);
      mRealClockTime = realTime * 1000000;
      mSimTimeSinceStartup = 0;
      mSimulationTime = mCorrectSimulationTime = 0.0;
      mLastDrawClockTime = mRealClockTime;
      mSimulationClockTime = mRealClockTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::Run()
   {
      mRunning = true;
      InitVars();

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
      InitVars();
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
         InitVars();
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

