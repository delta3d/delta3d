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
      , mRealClockTime(0)
      , mSimulationClockTime(0)
      , mLastDrawClockTime(0)
      , mSimulationTime(0.0)
      , mCorrectSimulationTime(0.0)
      , mFrameTime(1.0/60.0)
      , mTimeScale(1.0)
      , mMaxTimeBetweenDraws(30000)
      , mMaxSimulationStep(1000000000)
      , mSystemStages(System::STAGES_DEFAULT)
      , mUseFixedTimeStep(true)
      , mRunning(false)
      , mShutdownOnWindowClose(true)
      , mPaused(false)
      , mWasPaused(false)
      {
      }
      ~SystemImpl()
      {
         mStats = NULL;
         mTickClockTime = mClock.Tick();
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
      void FinishFrameStats()
      {
         mSystemStageTimes[System::STAGES_ALL] = mTotalFrameTime;

         if (mStats != NULL)
         {
            int frameNumber = mStats->getLatestFrameNumber();

            // set the stats for the full time it took EVERYTHING this frame
            if (mStats->collectStats("FullDeltaFrameTime"))
            {
               mStats->setAttribute(frameNumber, "FullDeltaFrameTime", mTotalFrameTime);
            }

            // The amount of time it took for OSG to update & draw versus the rest of our frame time
            if (mStats->collectStats("UpdatePlusDrawTime"))
            {
               double updateTime, renderTime;
               mStats->getAttribute(frameNumber, "Update traversal time taken", updateTime);
               mStats->getAttribute(frameNumber, "Rendering traversals time taken", renderTime);

               double totalDrawTime = 1000.0 * (updateTime + renderTime);
               mStats->setAttribute(frameNumber, "UpdatePlusDrawTime", totalDrawTime);
               double frameTime = mSystemStageTimes[System::STAGE_FRAME];
               mStats->setAttribute(frameNumber, "FrameMinusDrawAndUpdateTime", frameTime - totalDrawTime);
            }
         }
      }

      //////////////////////////////////////////////////////////////////
      /**
       * @param deltaSimTime The change in simulation time is seconds.
       * @param deltaRealTime The change in real time in seconds.
       */
      void EventTraversal(const double deltaSimTime, const double deltaRealTime);

      /**
       * @param deltaSimTime The change in simulation time is seconds.
       * @param deltaRealTime The change in real time in seconds.
       */
      void PostEventTraversal(const double deltaSimTime, const double deltaRealTime);

      /**
       * Stuff to do before the frame. Message: "preframe", delta real and time in seconds
       * @param deltaSimTime The change in simulation time is seconds.
       * @param deltaRealTime The change in real time in seconds.
       */
      void PreFrame(const double deltaSimTime, const double deltaRealTime);

      /**
       * @param deltaSimTime The change in simulation time is seconds.
       * @param deltaRealTime The change in real time in seconds.
       */
      void FrameSynch(const double deltaSimTime, const double deltaRealTime);

      /**
       * @param deltaSimTime The change in simulation time is seconds.
       * @param deltaRealTime The change in real time in seconds.
       */
      void CameraSynch(const double deltaSimTime, const double deltaRealTime);

      /**
       * Render the Camera, etc.  Message: "frame", delta time in seconds
       * @param deltaSimTime The change in simulation time is seconds.
       * @param deltaRealTime The change in real time in seconds.
       */
      void Frame(const double deltaSimTime, const double deltaRealTime);

      /**
       * Stuff to do after the frame.  Message: "postframe", delta time in seconds
       * @param deltaSimTime The change in simulation time is seconds.
       * @param deltaRealTime The change in real time in seconds.
       */
      void PostFrame(const double deltaSimTime, const double deltaRealTime);

      void SystemStepFixed(const double realDT);

      // initializes internal variables at the start of a run.
      void InitVars();


      void Pause(const double deltaRealTime);

      ///Intenal helper that calls Producer::Camera::frame(bool doSwap)
      ///with the proper value for doSwap.
      void CameraFrame();

      ///One System frame
      void SystemStep(float realDt = 0.0f);

      /// A passthough hack so impl methods can send messages
      void SendMessage(const std::string& message = "", void* data = NULL)
      {
         System::GetInstance().SendMessage(message, data);
      }

      dtCore::Timer mTickClock;
      dtCore::Timer_t mTimerStart;
      dtCore::ObserverPtr<osg::Stats> mStats;
      double mTotalFrameTime;

      // Store the time it took for each phase of the system (preframe, frame, ...)
      typedef std::map<System::SystemStages, double> SystemStageTimesMap;
      SystemStageTimesMap mSystemStageTimes;

      static System* mSystem;   ///<The System pointer
      static bool mInstanceFlag;///<Have we created a System yet?
      Timer mClock;

      /// time keeping variable.  This clock is used for calculating accurate time deltas using
      /// system dependent algorithms.  The value is not necessarily human understandable.
      Timer_t mTickClockTime;

      // The real world time (UTC) and a simulated, set-able version of it. They are both
      // in microseconds since January 1, 1970.
      Timer_t mRealClockTime, mSimulationClockTime;
      Timer_t mLastDrawClockTime;
      double mSimulationTime;
      double mSimTimeSinceStartup;
      double mCorrectSimulationTime;
      double mFrameTime;
      double mTimeScale;
      double mMaxTimeBetweenDraws;
      double mMaxSimulationStep;

      System::SystemStageFlags mSystemStages;

      bool mUseFixedTimeStep;
      bool mRunning; ///<Are we currently running?
      bool mShutdownOnWindowClose;
      bool mPaused;
      bool mWasPaused;

   };

   bool SystemImpl::mInstanceFlag = false;
   System* SystemImpl::mSystem = NULL;

   ////////////////////////////////////////////////////////////////////////////////
   System::System()
   {
      RegisterInstance(this);

      mSystemImpl = new SystemImpl();
   }

   ////////////////////////////////////////////////////////////////////////////////
   System::~System()
   {
      DeregisterInstance(this);

      SystemImpl::mInstanceFlag = false;

      delete mSystemImpl;
   }

   ////////////////////////////////////////////////////////////////////////////////
   System& System::GetInstance()
   {
      if (!SystemImpl::mInstanceFlag)
      {
         SystemImpl::mSystem = new System();
         SystemImpl::mSystem->SetName("System");
         SystemImpl::mInstanceFlag = true;
      }
      return *SystemImpl::mSystem;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::Destroy()
   {
      delete SystemImpl::mSystem;
      SystemImpl::mSystem = NULL;
      SystemImpl::mInstanceFlag = false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::SetShutdownOnWindowClose(bool shutdown)
   {
      mSystemImpl->mShutdownOnWindowClose = shutdown;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool System::GetShutdownOnWindowClose() const
   {
      return mSystemImpl->mShutdownOnWindowClose;
   }

   ////////////////////////////////////////////////////////////////////////////////
   double System::GetTimeScale() const
   {
      return mSystemImpl->mTimeScale;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::SetTimeScale(double newTimeScale)
   {
      mSystemImpl->mTimeScale = newTimeScale;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Timer_t System::GetRealClockTime() const
   {
      return mSystemImpl->mRealClockTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Timer_t System::GetSimulationClockTime() const
   {
      return mSystemImpl->mSimulationClockTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::SetSimulationClockTime(const dtCore::Timer_t& newTime)
   {
      mSystemImpl->mSimulationClockTime = newTime;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void System::SetMaxSimulationStep(double stepSize)
   {
      mSystemImpl->mMaxSimulationStep = stepSize;
   }

   ////////////////////////////////////////////////////////////////////////////////
   double System::GetMaxSimulationStep() const
   {
      return mSystemImpl->mMaxSimulationStep;
   }

   ////////////////////////////////////////////////////////////////////////////////
   double System::GetSimulationTime() const
   {
      return mSystemImpl->mSimulationTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   double System::GetSimTimeSinceStartup() const
   {
      return mSystemImpl->mSimTimeSinceStartup;
   }

   ////////////////////////////////////////////////////////////////////////////////
   double System::GetCorrectSimulationTime() const
   {
      return mSystemImpl->mCorrectSimulationTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::SetFrameRate(double newRate)
   {
      mSystemImpl->mFrameTime = 1.0 / newRate;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::SetMaxTimeBetweenDraws(double newTime)
   {
      mSystemImpl->mMaxTimeBetweenDraws = newTime * 1000000.0;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::SetUseFixedTimeStep(bool value)
   {
      mSystemImpl->mUseFixedTimeStep = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   double System::GetFrameRate() const
   {
      return 1.0/mSystemImpl->mFrameTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool System::GetUsesFixedTimeStep() const
   {
      return mSystemImpl->mUseFixedTimeStep;
   }

   ////////////////////////////////////////////////////////////////////////////////
   double System::GetMaxTimeBetweenDraws() const
   {
      return mSystemImpl->mMaxTimeBetweenDraws / 1000000.0;
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
      mSystemImpl->mSimulationTime = newTime;
      mSystemImpl->mCorrectSimulationTime = newTime;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::SetPause(bool paused)
   {
      // don't send out a message unless it actually changes.
      if (mSystemImpl->mPaused == paused)
      {
         return;
      }

      mSystemImpl->mPaused = paused;

      if (mSystemImpl->mPaused)
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
      return mSystemImpl->mPaused;
   }


   ////////////////////////////////////////////////////////////////////////////////
   void System::SetSystemStages(SystemStageFlags stages)
   {
      mSystemImpl->mSystemStages = stages;
   }

   ////////////////////////////////////////////////////////////////////////////////
   System::SystemStageFlags System::GetSystemStages() const
   {
      return mSystemImpl->mSystemStages;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SystemImpl::Pause(const double deltaRealTime)
   {
      SendMessage(System::MESSAGE_PAUSE, const_cast<double*>(&deltaRealTime));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SystemImpl::SystemStepFixed(const double realDT)
   {
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
      if (mCorrectSimulationTime + (0.5 * simFrameTime) /*+ previousDrawTime*/ < mSimulationTime + simFrameTime)
      {
#ifndef DELTA_WIN32
         AppSleep(1); // On Linux and OS X, it seems to sleep for 1 like it should, and 0 is a nop.
#else
         AppSleep(0); // On Windows, sleeping 0 does a minimal sleep, but 1 is too long.
#endif
         return;
      }

      mTotalFrameTime = 0.0;  // reset frame timer for stats

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
         SetLastStatTimer(System::MESSAGE_CAMERA_SYNCH, System::STAGE_CAMERA_SYNCH);
         SetLastStatTimer(System::MESSAGE_FRAME_SYNCH, System::STAGE_FRAME_SYNCH);
         SetLastStatTimer(System::MESSAGE_FRAME, System::STAGE_FRAME);
      }

      PostFrame(simFrameTime, realFrameTime);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ///private
   void SystemImpl::SystemStep(float realDeltaOverride)
   {
      double realDT = realDeltaOverride;
      if (realDeltaOverride < FLT_EPSILON)
      {
         const Timer_t lastClockTime  = mTickClockTime;
         mTickClockTime = mClock.Tick();

         realDT = mClock.DeltaSec(lastClockTime, mTickClockTime);
      }

      // update real time variable(s)
      mRealClockTime += Timer_t(realDT * 1000000);

      if (mPaused)
      {
         mTotalFrameTime = 0.0;  // reset frame timer for stats
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
            mTotalFrameTime = 0.0;  // reset frame timer for stats
            mWasPaused = false;

            // update simulation time variable(s)
            double simDT = realDT * mTimeScale;
            if (simDT > mMaxSimulationStep)
            {
               simDT = mMaxSimulationStep;
            }
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

      FinishFrameStats();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::StepWindow()
   {
      if (!mSystemImpl->mRunning)
      {
         return;
      }

      Step();

      // FIXME TODO how to check if GraphicsWindow is always running ??
      // this implementation in really the good way
      if (mSystemImpl->mShutdownOnWindowClose)
      {
         bool areGraphicsWindow = false;
         for (int i = 0; i < DeltaWin::GetInstanceCount() && !areGraphicsWindow; ++i)
         {
             areGraphicsWindow = areGraphicsWindow || DeltaWin::GetInstance(i)->GetOsgViewerGraphicsWindow()->valid();
         }

         mSystemImpl->mRunning = mSystemImpl->mRunning && areGraphicsWindow;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SystemImpl::InitVars()
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

      while (mSystemImpl->mRunning)
      {
         StepWindow();
      }

      LOG_DEBUG("System: Exiting...");
      SendMessage(MESSAGE_EXIT);
      LOG_DEBUG("System: Done Exiting.");
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool System::IsRunning() const
   {
      return mSystemImpl->mRunning;
   }


   ////////////////////////////////////////////////////////////////////////////////
   void System::Start()
   {
      mSystemImpl->mRunning = true;
      mSystemImpl->InitVars();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::Step(float realDt)
   {
      static bool first = true;

      if (!mSystemImpl->mRunning)
      {
         return;
      }

      if (first)
      {
         mSystemImpl->InitVars();
         first = false;
      }

      mSystemImpl->SystemStep(realDt);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::Stop()
   {
      mSystemImpl->mRunning = false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SystemImpl::EventTraversal(const double deltaSimTime, const double deltaRealTime)
   {
      if (dtUtil::Bits::Has(mSystemStages, System::STAGE_EVENT_TRAVERSAL))
      {
         StartStatTimer();

         double userData[2] = { deltaSimTime, deltaRealTime };
         SendMessage(System::MESSAGE_EVENT_TRAVERSAL, userData);

         EndStatTimer(System::MESSAGE_EVENT_TRAVERSAL, System::STAGE_EVENT_TRAVERSAL);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SystemImpl::PostEventTraversal(const double deltaSimTime, const double deltaRealTime)
   {
      if (dtUtil::Bits::Has(mSystemStages, System::STAGE_POST_EVENT_TRAVERSAL))
      {
         StartStatTimer();

         double userData[2] = { deltaSimTime, deltaRealTime };
         SendMessage(System::MESSAGE_POST_EVENT_TRAVERSAL, userData);

         EndStatTimer(System::MESSAGE_POST_EVENT_TRAVERSAL, System::STAGE_POST_EVENT_TRAVERSAL);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SystemImpl::PreFrame(const double deltaSimTime, const double deltaRealTime)
   {
      if (dtUtil::Bits::Has(mSystemStages, System::STAGE_PREFRAME))
      {
         StartStatTimer();

         double userData[2] = { deltaSimTime, deltaRealTime };
         SendMessage(System::MESSAGE_PRE_FRAME, userData);

         EndStatTimer(System::MESSAGE_PRE_FRAME, System::STAGE_PREFRAME);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SystemImpl::FrameSynch(const double deltaSimTime, const double deltaRealTime)
   {
      if (dtUtil::Bits::Has(mSystemStages, System::STAGE_FRAME_SYNCH))
      {
         StartStatTimer();

         double userData[2] = { deltaSimTime, deltaRealTime };
         SendMessage(System::MESSAGE_FRAME_SYNCH, userData);

         EndStatTimer(System::MESSAGE_FRAME_SYNCH, System::STAGE_FRAME_SYNCH);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SystemImpl::CameraSynch(const double deltaSimTime, const double deltaRealTime)
   {
      if (dtUtil::Bits::Has(mSystemStages, System::STAGE_CAMERA_SYNCH))
      {
         StartStatTimer();

         double userData[2] = { deltaSimTime, deltaRealTime };
         SendMessage(System::MESSAGE_CAMERA_SYNCH, userData);

         EndStatTimer(System::MESSAGE_CAMERA_SYNCH, System::STAGE_CAMERA_SYNCH);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SystemImpl::Frame(const double deltaSimTime, const double deltaRealTime)
   {
      if (dtUtil::Bits::Has(mSystemStages, System::STAGE_FRAME))
      {
         StartStatTimer();

         double userData[2] = { deltaSimTime, deltaRealTime };
         SendMessage(System::MESSAGE_FRAME, userData );

         EndStatTimer(System::MESSAGE_FRAME, System::STAGE_FRAME);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SystemImpl::PostFrame(const double deltaSimTime, const double deltaRealTime)
   {
      if (dtUtil::Bits::Has(mSystemStages, System::STAGE_POSTFRAME))
      {
         StartStatTimer();

         double userData[2] = { deltaSimTime, deltaRealTime };
         SendMessage(System::MESSAGE_POST_FRAME, userData);

         EndStatTimer(System::MESSAGE_POST_FRAME, System::STAGE_POSTFRAME);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void System::Config()
   {
      if (dtUtil::Bits::Has(mSystemImpl->mSystemStages, System::STAGE_CONFIG))
      {
         SendMessage(System::MESSAGE_CONFIG);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   double System::GetSystemStageTime(System::SystemStages systemStage)
   {
      return mSystemImpl->mSystemStageTimes[systemStage];
   }

}

