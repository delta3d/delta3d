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
      void StartStatTimer()
      {
         // Call at the beginning of a section.  Call before calling EndStatTimer()
         mTimerStart = mTickClock.Tick();
      }

      /////////////////////////////////////////////////////////////////
      double EndStatTimer(const std::string& attribName, System::SystemStages systemStage)
      {
         // Call this at the end of a section. Call StartStatTimer() first 
         double elapsedTime = mTickClock.DeltaMil(mTimerStart, mTickClock.Tick());
         mTotalFrameTime += elapsedTime; // accumulate till frame end, then reset to 0 next frame.

         // Update stats if we are tracking
         if (mStats != NULL && mStats->collectStats(attribName))
         {
            mStats->setAttribute(mStats->getLatestFrameNumber(), attribName, elapsedTime);
         }

         // Update the system time value so it 
         mSystemStageTimes[systemStage] = elapsedTime;

         return elapsedTime;
      }

      //////////////////////////////////////////////////////////////////
      void SetLastStatTimer(const std::string& attribName, System::SystemStages systemStage)
      {
         // If we are in fixed time step and we are skipping a phase, 
         // then use last frame's value.
         double lastTime = mSystemStageTimes[systemStage];
         mTotalFrameTime += lastTime; // accumulate till frame end, then reset to 0 next frame.
         // Update stats if we are tracking
         if (mStats != NULL && mStats->collectStats(attribName))
         {
            mStats->setAttribute(mStats->getLatestFrameNumber(), attribName, lastTime);
         }
      }

      //////////////////////////////////////////////////////////////////

      dtCore::Timer mTickClock;
      dtCore::Timer_t mTimerStart;
      dtCore::ObserverPtr<osg::Stats> mStats;
      double mTotalFrameTime;

      // Store the time it took for each phase of the system (preframe, frame, ...)
      typedef std::map<System::SystemStages, double> SystemStageTimesMap;
      SystemStageTimesMap mSystemStageTimes;
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
      , mSystemStages(STAGES_DEFAULT)
      , mUseFixedTimeStep(false)
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

      // simFrameTime is our fixed time step. Usually like 1/60 or something.
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

      // If ahead enough that we could draw and still be ahead, then just waste time (do nothing in a loop)
      double previousDrawTime = mSystemImpl->mSystemStageTimes[System::STAGE_FRAME] / 1000.0;
      if (mCorrectSimulationTime + previousDrawTime < mSimulationTime + simFrameTime)
      {
#ifndef DELTA_WIN32
         AppSleep(1); // In Linux, it seems to sleep for 1 like it should
#else
         AppSleep(0); // in Windows, it sleeps a LONG time so we just 'yield'
#endif
         return;
      }

      mSystemImpl->mTotalFrameTime = 0.0;  // reset frame timer for stats

      mSimulationTime      += simFrameTime;
      mSimTimeSinceStartup += simFrameTime;
      mSimulationClockTime += Timer_t(simFrameTime * 1000000);
 
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
      else // Else, skip the draw and just update the stats (w/o this the stats don't work right)
      {
         mSystemImpl->SetLastStatTimer(MESSAGE_CAMERA_SYNCH, System::STAGE_CAMERA_SYNCH);
         mSystemImpl->SetLastStatTimer(MESSAGE_FRAME_SYNCH, System::STAGE_FRAME_SYNCH);
         mSystemImpl->SetLastStatTimer(MESSAGE_FRAME, System::STAGE_FRAME);
      }
      PostFrame(simFrameTime, realFrameTime);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ///private
   void System::SystemStep()
   {
      const Timer_t lastClockTime  = mTickClockTime;
      mTickClockTime = mClock.Tick();

      const double realDT = mClock.DeltaSec(lastClockTime, mTickClockTime);

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

         mSystemImpl->EndStatTimer(MESSAGE_EVENT_TRAVERSAL, System::STAGE_EVENT_TRAVERSAL);
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

         mSystemImpl->EndStatTimer(MESSAGE_POST_EVENT_TRAVERSAL, System::STAGE_POST_EVENT_TRAVERSAL);
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

         mSystemImpl->EndStatTimer(MESSAGE_PRE_FRAME, System::STAGE_PREFRAME);
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

         mSystemImpl->EndStatTimer(MESSAGE_FRAME_SYNCH, System::STAGE_FRAME_SYNCH);
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

         mSystemImpl->EndStatTimer(MESSAGE_CAMERA_SYNCH, System::STAGE_CAMERA_SYNCH);
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

         mSystemImpl->EndStatTimer(MESSAGE_FRAME, System::STAGE_FRAME);
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

         mSystemImpl->EndStatTimer(MESSAGE_POST_FRAME, System::STAGE_POSTFRAME);
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

   ////////////////////////////////////////////////////////////////////////////////
   double System::GetSystemStageTime(System::SystemStages systemStage)
   {
      return mSystemImpl->mSystemStageTimes[systemStage];
   }

}

