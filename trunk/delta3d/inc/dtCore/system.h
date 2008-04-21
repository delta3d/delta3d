/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2005 MOVES Institute 
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 2.1 of the License, or (at your option) 
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 *
*/

#ifndef DELTA_SYSTEM
#define DELTA_SYSTEM

// system.h: interface for the System class.
//
//////////////////////////////////////////////////////////////////////


#include <dtCore/base.h>
#include <dtCore/timer.h>

#include <map>

namespace dtCore
{
   class Camera;
   
   /** 
    * Performs high-level frame coordination. The System is used to control the 
    * frame-based coordination of all the dtCore classes.  There is only one 
    * instance of the System available and is always accessable using Instance();
    * Once the application has created all the required instances, System::Run() 
    * can be called and will block until System::Stop() gets called.  By 
    * subscribing to the System, Base derived objects can receive the System 
    * messages "preframe", "frame", "postframe", and "configure".
    *
    * To enable fixed time stepping, you will need to set the rate to the rate you
    * want, the MaxTimeBetweenDraws to something like .1, and the use fixed time step
    * to true
    * ourSystem.SetFrameRate(60);
    * ourSystem.SetMaxTimeBetweenDraws(.1);
    * ourSystem.SetUseFixedTimeStep(true);
    *
    * This will step your system using this fixed frame rate
    *
    * You can also use this with your config file application loads up, the prop-
    * erties are as follows
    *
    * <Properties>
    *    <Property Name="System.SimFrameRate">60</Property>
    *    <Property Name="System.MaxTimeBetweenDraws">0.1</Property>
    *    <Property Name="System.UseFixedTimeStep">true</Property>
    * </Properties>
    *
    * These will automatically get loaded up on startup if your config file is found
    *
    * @see AddListener()
    * @see OnMessage()
    */
   class DT_CORE_EXPORT System : public Base 
   {

   public:
      DECLARE_MANAGEMENT_LAYER(System)

      /** Enumeration of the different stages the System goes through during
        * the update loop.
        */
      enum SystemStages
      {
         STAGE_NONE        = 0x00000000, ///<No update loop stages are performed
         STAGE_PREFRAME    = 0x00000001, ///<"preframe" message
         STAGE_FRAMESYNCH  = 0x00000002,
         STAGE_FRAME       = 0x00000004, ///<"frame" message, plus camera rendering
         STAGE_POSTFRAME   = 0x00000008, ///<"postframe" message
         STAGE_CONFIG      = 0X00000010, ///<"config" message, plus render a camera frame
         STAGES_DEFAULT  = STAGE_FRAMESYNCH|STAGE_PREFRAME|STAGE_FRAME|STAGE_POSTFRAME|STAGE_CONFIG
      };
   
      typedef unsigned int SystemStageFlags;

   protected:

      virtual ~System();

   public:

      ///Perform any configuration required.  Message: "configure"
      void Config();

      ///Get a pointer to the System
      static System& GetInstance();

      static void Destroy();

      /** Set which stages the system will process during an update loop.  This
       * can be used to turn off a particular stage for optimization e.g., if no
       * operations are required in post frame.  To use, supply a bitwise combination
       * of the stage required.
       * @code
       * dtCore::System::GetInstance().SetSystemStages( System::STAGE_PREFRAME|System::STAGE_FRAME );
       * @endcode
       * This is an advanced operation and can render an application incapacitated.
       * Defaults to STAGES_DEFAULT.
       * @param stages The bitwise combination of SystemStages the System should process.
       */
      void SetSystemStages( SystemStageFlags stages );

      /** Get the SystemStages the System is currently operating on.
       * @return The bitwise combination of stages
       */
      SystemStageFlags GetSystemStages() const;

      

      ///Toggles the running flag to true
      void Start();

      /** Performs one System frame step, but does not perform any
       *  DeltaWin processing.  Use this call if the DeltaWin is embedded inside a
       *  GUI package (QT, FLTK, etc.)
       *  Emits the "preframe", "frame", and "postframe" messages.  Requires the
       *  Start() method is called first.
       *  @see SetSystemStages()
       */
      void Step();

      /** Performs one complete System frame step and performs DeltaWin processing.  This is
       *  useful if you wish to control the entire System's frame processing.
       *  Emits the "preframe", "frame", and "postframe" messages. 
       */
      void StepWindow();

      ///Stop the System from running
      void Stop();

      ///Start the System running
      void Run();

      ///Is the system running
      bool IsRunning() const { return mRunning; }

      void SetPause( bool paused );
      bool GetPause() const;
      
      /*!
      * Controls shutdown behavior of the system. Normally the system will only
      * shutdown if Stop() is called. If this function is called with 'true',
      * then the system will perform an additional check in the system loop
      * to see if there are any active windows. If there are none, it will
      * shutdown. The behavior is set to 'true' by default.
      *
      * @param shutdown : If 'shutdown' is true, the system loop will terminate
      * when the last window is closed. Otherwise, the console and system will
      * remain open.
      */
      void SetShutdownOnWindowClose( bool shutdown );

      /*!
      * Returns whether or not the system will shutdown upon the last window being
      * closed.
      */
      bool GetShutdownOnWindowClose() const;

      /**
       * @return the scale of realtime at which the simulation time is running.
       */
      double GetTimeScale() const;

      /**
       * @return the scale of realtime at which the simulation time is running.
       */
      void SetTimeScale(double newTimeScale);
      
      /**
       * @note the clock time is a 64 bit int in microseconds
       * @return the current real clock in microseconds since January 1, 1970
       */
      Timer_t GetRealClockTime() const;


      /**
       * @note the simulation clock time is a 64 bit int in microseconds
       * @return the current simulation clock in the same format as the real clock time but can be changed
       *         and follows the time scale.
       */
      Timer_t GetSimulationClockTime() const;
      
      /**
       * Sets the simulation wall clock time.  This is used for things like time of day.
       * @param newTime the new time in microseconds, like the real clock time.
       */
      void SetSimulationClockTime(const dtCore::Timer_t &newTime);

      /**
       * The simulation time starts at 0 at the beginning of the simulation.
       * @return the simulation time in seconds.
       */
      double GetSimulationTime() const;

      /**
      *	@note SimTimeSinceStartup is reflective of the total amount of time that has been simulated.  
      *          it starts at 0 and goes up in microseconds.  This time cannot be changed but does scale
      *          with the simulation time.
      */
      double GetSimTimeSinceStartup() const;
      
      /** 
       * This is the Simulation time that would be based on actual passage of time. This won't
       * match the simulation time always when SetUseFixedTimeStep(true) is called.  The value
       * is probably not of any use to a developer.  This accessor exists for the sake of unit testsing.
       */
      double GetCorrectSimulationTime() const;
      
      /**
       * Sets the simulation time.  It is assumed that part of the simulation is using this exact value to keep track of things.
       * @param newTime the new time in seconds since the start of the simulation for the simualtion time.
       */
      void SetSimulationTime(double newTime);

      /// this is the amount it should step by, only valid in mUseFixedTimeRate == true
      void SetFrameRate(double newRate);
      
      /// your minimum number of frames you want it to draw, only valid in mUseFixedTimeRate == true
      void SetMaxTimeBetweenDraws(double newTime);

      /// Set to make the system step by the fixed amount of time.
      void SetUseFixedTimeStep(bool value);

      /// return the frame step, in case others need to use this.
      double GetFrameRate() const;

      /// return to see if we are using the fixed time stepping feature of the engine.
      bool GetUsesFixedTimeStep() const;

      /// mostly for unit test, other places in code may need this though
      double GetMaxTimeBetweenDraws() const;

   private:

      System(); ///<private
      static System *mSystem;   ///<The System pointer
      static bool mInstanceFlag;///<Have we created a System yet?
      Timer mClock;

      /// time keeping variable.  This clock is used for calculating accurate time deltas using
      /// system dependent algorithms.  The value is not necessarily human understandable.
      Timer_t mTickClockTime;
      
      //The real world time (UTC) and a simulated, settable version of it. They are both
      // in microseconds since January 1, 1970.
      Timer_t mRealClockTime, mSimulationClockTime, mSimTimeSinceStartup;
      Timer_t mLastDrawClockTime;
      double mSimulationTime;      
      double mCorrectSimulationTime;
      double mFrameTime;
      double mTimeScale;
      double mDt;
      double mMaxTimeBetweenDraws;
      double mAccumulationTime;
      
      SystemStageFlags mSystemStages;

      bool mUseFixedTimeStep;
      bool mAccumulateLastRealDt;
      bool mRunning; ///<Are we currently running?
      bool mShutdownOnWindowClose;
      bool mPaused;
      bool mWasPaused;
      
      
      // will step the system with a fixed time step.
      void SystemStepFixed();

      //initializes internal variables at the start of a run.
      void InitVars();

      /**
       * @param deltaSimTime The change in simulation time is seconds.
       * @param deltaRealTime The change in real time in seconds.
       */
      void FrameSynch(const double deltaSimTime, const double deltaRealTime);

      /**
       * Stuff to do before the frame. Message: "preframe", delta real and time in seconds
       * @param deltaSimTime The change in simulation time is seconds.
       * @param deltaRealTime The change in real time in seconds.
       */
      void PreFrame(const double deltaSimTime, const double deltaRealTime);

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

      void Pause( const double deltaRealTime );

      ///Intenal helper that calls Producer::Camera::frame(bool doSwap)
      ///with the proper value for doSwap.
      void CameraFrame();

      ///One System frame
      void SystemStep();

   };
}

#endif // DELTA_SYSTEM
