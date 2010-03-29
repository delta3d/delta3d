// system.cpp: implementation of the System class.
//
//////////////////////////////////////////////////////////////////////

#include <prefix/dtcoreprefix.h>
#include <dtCore/system.h>
#include <dtUtil/log.h>
#include <dtUtil/bits.h>
#include <dtUtil/mswinmacros.h>
#include <dtCore/deltawin.h>

#include <osgViewer/GraphicsWindow>
#include <ctime>

//#include <sstream>
#include <osg/Stats>

using namespace dtUtil;

namespace dtCore
{
   IMPLEMENT_MANAGEMENT_LAYER(System)

   bool System::mInstanceFlag = false;
   System* System::mSystem = 0;

   const dtUtil::RefString System::MESSAGE_EVENT_TRAVERSAL("eventtraversal");
   const dtUtil::RefString System::MESSAGE_POST_EVENT_TRAVERSAL("posteventtraversal");
   const dtUtil::RefString System::MESSAGE_PRE_FRAME("preframe");
   const dtUtil::RefString System::MESSAGE_CAMERA_SYNCH("camerasynch");
   const dtUtil::RefString System::MESSAGE_FRAME_SYNCH("framesynch");
   const dtUtil::RefString System::MESSAGE_FRAME("frame");
   const dtUtil::RefString System::MESSAGE_POST_FRAME("postframe");
   const dtUtil::RefString System::MESSAGE_CONFIG("configure");
   const dtUtil::RefString System::MESSAGE_PAUSE("pause");
   const dtUtil::RefString System::MESSAGE_PAUSE_START("pause_start");
   const dtUtil::RefString System::MESSAGE_PAUSE_END("pause_end");
   const dtUtil::RefString System::MESSAGE_EXIT("exit");


   /// A wrapper for data like stats to prevent includes wherever system.h is used - uses the pimple pattern (like view)
   class SystemImpl
   {
   public:
      SystemImpl()
         : mTimerStart(0)
         , mTotalFrameTime(0.0)
      {
      }
      ~SystemImpl()
      {
         mStats = NULL;
      }


      /////////////////////////////////////////////////////////////////
      float EndStatTimer(const std::string& attribName)
      {
         // Call this at the end of a section. User is responsible for calling StartStatTimer()
         // first.

         double elapsedTime = 0.0;
         if (mStats != NULL && mStats->collectStats(attribName))
         {
            elapsedTime = mTickClock.DeltaMil(mTimerStart, mTickClock.Tick());
            mStats->setAttribute(mStats->getLatestFrameNumber(), attribName, elapsedTime);
         }
         // accumulates till the end of frame, then reset back to 0 next frame.
         mTotalFrameTime += elapsedTime;

         return elapsedTime;
      }

      /////////////////////////////////////////////////////////////////
      void StartStatTimer()
      {
         // Call at the beginning of a section.  User is responsible for calling this before
         // calling EndStatTimer()
         if (mStats != NULL)
         {
            mTimerStart = mTickClock.Tick();
         }
      }

      dtCore::Timer mTickClock;
      dtCore::Timer_t mTimerStart;
      dtCore::ObserverPtr<osg::Stats> mStats;
      double mTotalFrameTime;
   };


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

      mSystemImpl = new SystemImpl();
   }

   ////////////////////////////////////////////////////////////////////////////////
   System::~System()
   {
      DeregisterInstance(this);

      mInstanceFlag = false;

      delete mSystemImpl;
   }

   ////////////////////////////////////////////////////////////////////////////////
   System& System::GetInstance()
   {
      if (!mInstanceFlag)
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
   void System::SetShutdownOnWindowClose(bool shutdown)
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
   void System::SetSimulationClockTime(const dtCore::Timer_t& newTime)
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
      mFrameTime = 1.0 / newRate;
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
      return mMaxTimeBetweenDraws / 1000000.0;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::SetStats(osg::Stats* newValue)
   {
      // Holds onto stats on the impl - part of pimple pattern to hide includes
      mSystemImpl->mStats = newValue;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Stats* System::GetStats()
   {
      // Holds onto stats on the impl - part of pimple pattern to hide includes
      return mSystemImpl->mStats.get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool System::IsStatsOn()
   {
      return (GetStats() != NULL);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::SetSimulationTime(double newTime)
   {
      mSimulationTime = newTime;
      mCorrectSimulationTime = newTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::SetPause(bool paused)
   {
      // don't send out a message unless it actually changes.
      if (mPaused == paused)
      {
         return;
      }

      mPaused = paused;

      if (mPaused)
      {
         SendMessage(MESSAGE_PAUSE_START);
      }
      else
      {
         SendMessage(MESSAGE_PAUSE_END);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool System::GetPause() const
   {
      return mPaused;
   }


   ////////////////////////////////////////////////////////////////////////////////
   void System::SetSystemStages(SystemStageFlags stages)
   {
      mSystemStages = stages;
   }

   ////////////////////////////////////////////////////////////////////////////////
   System::SystemStageFlags System::GetSystemStages() const
   {
      return mSystemStages;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::Pause(const double deltaRealTime)
   {
      SendMessage(MESSAGE_PAUSE, const_cast<double*>(&deltaRealTime));
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

      mSystemImpl->mTotalFrameTime = 0.0;  // reset frame timer for stats
      mAccumulateLastRealDt = false;

      mSimulationTime      += simFrameTime;
      mSimTimeSinceStartup += simFrameTime;
      mSimulationClockTime += Timer_t(simFrameTime * 1000000);

      //const double realFrameTime = realDT + mAccumulationTime;
      const double realFrameTime = mFrameTime;
      EventTraversal(simFrameTime, realFrameTime);
      PostEventTraversal(simFrameTime, realFrameTime);
      PreFrame(simFrameTime, realFrameTime);

      //if we're ahead of the desired sim time, then draw.
      if (mSimulationTime >= mCorrectSimulationTime
         || (mRealClockTime - mLastDrawClockTime) > mMaxTimeBetweenDraws)
      {
         mLastDrawClockTime = mRealClockTime;
         CameraSynch(simFrameTime, realFrameTime);
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
         mSystemImpl->mTotalFrameTime = 0.0;  // reset frame timer for stats
         mWasPaused = true;
         EventTraversal(0.0, realDT);
         PostEventTraversal(0.0, realDT);
         Pause(realDT);
         CameraSynch(0.0, realDT);
         FrameSynch(0.0, realDT);
         Frame(0.0, realDT);
      }
      else
      {
         if (!mUseFixedTimeStep)
         {
            mSystemImpl->mTotalFrameTime = 0.0;  // reset frame timer for stats
            mWasPaused = false;

            // update real time variable(s)
            mRealClockTime  += Timer_t(realDT * 1000000);

            // update simulation time variable(s)
            const double simDT = realDT * mTimeScale;
            mSimulationTime      += simDT;
            mSimTimeSinceStartup += simDT;
            mSimulationClockTime += Timer_t(simDT * 1000000);

            EventTraversal(simDT, realDT);
            PostEventTraversal(simDT, realDT);
            PreFrame(simDT, realDT);
            CameraSynch(simDT, realDT);
            FrameSynch(simDT, realDT);
            Frame(simDT, realDT);
            PostFrame(simDT, realDT);
         }
         else
         {
            SystemStepFixed(realDT);
         }
      }

      // set our full delta processing time as an attribute
      if (IsStatsOn())
      {
         mSystemImpl->mStats->setAttribute(mSystemImpl->mStats->getLatestFrameNumber(),
            "FullDeltaFrameTime", mSystemImpl->mTotalFrameTime);
      }


   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::StepWindow()
   {
      SystemStep();

      // FIXME how to check if GraphicsWindow is always running ??
      // this implementation in really the good way
      if (mShutdownOnWindowClose)
      {
         bool areGraphicsWindow = false;
         for (int i = 0; i < DeltaWin::GetInstanceCount() && !areGraphicsWindow; ++i)
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
      mRealClockTime = Timer_t(realTime) * 1000000;
      mSimTimeSinceStartup = 0;
      mSimulationTime = mCorrectSimulationTime = 0.0;
      mLastDrawClockTime = mRealClockTime;
      mSimulationClockTime = mRealClockTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::Run()
   {
      Start(); ///Automatically start the System when Run.

      while (mRunning)
      {
         StepWindow();
      }

      LOG_DEBUG("System: Exiting...");
      SendMessage(MESSAGE_EXIT);
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

      if (!mRunning)
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
   void System::EventTraversal(const double deltaSimTime, const double deltaRealTime)
   {
      if (dtUtil::Bits::Has(mSystemStages, System::STAGE_EVENT_TRAVERSAL))
      {
         mSystemImpl->StartStatTimer();

         double userData[2] = { deltaSimTime, deltaRealTime };
         SendMessage(MESSAGE_EVENT_TRAVERSAL, userData);

         mSystemImpl->EndStatTimer(MESSAGE_EVENT_TRAVERSAL);

      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::PostEventTraversal(const double deltaSimTime, const double deltaRealTime)
   {
      if (dtUtil::Bits::Has(mSystemStages, System::STAGE_POST_EVENT_TRAVERSAL))
      {
         mSystemImpl->StartStatTimer();

         double userData[2] = { deltaSimTime, deltaRealTime };
         SendMessage(MESSAGE_POST_EVENT_TRAVERSAL, userData);

         mSystemImpl->EndStatTimer(MESSAGE_POST_EVENT_TRAVERSAL);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::PreFrame(const double deltaSimTime, const double deltaRealTime)
   {
      if (dtUtil::Bits::Has(mSystemStages, System::STAGE_PREFRAME))
      {
         mSystemImpl->StartStatTimer();

         double userData[2] = { deltaSimTime, deltaRealTime };
         SendMessage(MESSAGE_PRE_FRAME, userData);

         mSystemImpl->EndStatTimer(MESSAGE_PRE_FRAME);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::FrameSynch(const double deltaSimTime, const double deltaRealTime)
   {
      if (dtUtil::Bits::Has(mSystemStages, System::STAGE_FRAME_SYNCH))
      {
         mSystemImpl->StartStatTimer();

         double userData[2] = { deltaSimTime, deltaRealTime };
         SendMessage(MESSAGE_FRAME_SYNCH, userData);

         mSystemImpl->EndStatTimer(MESSAGE_FRAME_SYNCH);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::CameraSynch(const double deltaSimTime, const double deltaRealTime)
   {
      if (dtUtil::Bits::Has(mSystemStages, System::STAGE_CAMERA_SYNCH))
      {
         mSystemImpl->StartStatTimer();

         double userData[2] = { deltaSimTime, deltaRealTime };
         SendMessage(MESSAGE_CAMERA_SYNCH, userData);

         mSystemImpl->EndStatTimer(MESSAGE_CAMERA_SYNCH);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::Frame(const double deltaSimTime, const double deltaRealTime)
   {
      if (dtUtil::Bits::Has(mSystemStages, System::STAGE_FRAME))
      {
         mSystemImpl->StartStatTimer();

         double userData[2] = { deltaSimTime, deltaRealTime };
         SendMessage(MESSAGE_FRAME, userData );

         mSystemImpl->EndStatTimer(MESSAGE_FRAME);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::PostFrame(const double deltaSimTime, const double deltaRealTime)
   {
      if (dtUtil::Bits::Has(mSystemStages, System::STAGE_POSTFRAME))
      {
         mSystemImpl->StartStatTimer();

         double userData[2] = { deltaSimTime, deltaRealTime };
         SendMessage(MESSAGE_POST_FRAME, userData);

         mSystemImpl->EndStatTimer(MESSAGE_POST_FRAME);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::Config()
   {
      if (dtUtil::Bits::Has(mSystemStages, System::STAGE_CONFIG))
      {
         SendMessage(MESSAGE_CONFIG);
      }
   }
}

