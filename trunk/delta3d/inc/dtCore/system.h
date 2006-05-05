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

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace Producer
{
   class RenderSurface;
}
/// @endcond

namespace dtCore
{
   class Camera;
   
   /** 
    * Performs high-level frame coordination. The System is used to control the frame-based coordination of all the 
    * dtCore classes.  There is only one instance of the System available and is
    * always accessable using Instance();
    * Once the application has created all the required instances, System::Run() 
    * can be called and will block until System::Stop() gets called.  By 
    * subscribing to the System, Base derived objects can receive the System 
    * messages "preframe", "frame", "postframe", and "configure".
    * @see AddListener()
    * @see OnMessage()
    */
   class DT_CORE_EXPORT System : public Base 
   {

   public:

      DECLARE_MANAGEMENT_LAYER(System)

   protected:

      virtual ~System();

   public:

      ///Perform any configuration required.  Message: "configure"
      void Config();

      ///Get a pointer to the System
      static System *Instance();

      ///Toggles the running flag to true
      void Start();

      /** Performs one System frame step, but does not perform any
       *  DeltaWin processing.  Use this call if the DeltaWin is embedded inside a
       *  GUI package (QT, FLTK, etc.)
       *  Emits the "preframe", "frame", and "postframe" messages.  Requires the
       *  Start() method is called first.
       */
      void Step();

      /** Performs one complete frame step and performs DeltaWin processing.  This is
       *  usefull if you wish to control the entire System's frame processing.
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
      void SetShutdownOnWindowClose( bool shutdown ) { mShutdownOnWindowClose = shutdown; }

      /*!
      * Returns whether or not the system will shutdown upon the last window being
      * closed.
      */
      bool GetShutdownOnWindowClose() const { return mShutdownOnWindowClose; }

      /**
       * @return the scale of realtime at which the simulation time is running.
       */
      double GetTimeScale() const { return mTimeScale; }

      /**
       * @return the scale of realtime at which the simulation time is running.
       */
      void SetTimeScale(double newTimeScale) { mTimeScale = newTimeScale; }
      
      /**
       * @note the clock time is a 64 bit int in microseconds
       * @return the current real clock
       */
      dtCore::Timer_t GetRealClockTime() const { return mClockTime; }

      /**
       * @note the simulation clock time is a 64 bit int in microseconds
       * @return the current simulation clock
       */
      dtCore::Timer_t GetSimulationClockTime() const { return mSimulationClockTime; }
      
      /**
       * Sets the simulation wall clock time.  This is used for things like time of day.
       * @param newTime the new time in microseconds, like the real clock time.
       */
      void SetSimulationClockTime(dtCore::Timer_t newTime) { mSimulationClockTime = newTime; }

      /**
       * The simulation time starts at 0 at the beginning of the simulation.
       * @return the simulation time in seconds.
       */
      double GetSimulationTime() const { return mSimulationTime; }
      
      /**
       * Sets the simulation time.  It is assumed that part of the simulation is using this exact value to keep track of things.
       * @param newTime the new time in seconds since the start of the simulation for the simualtion time.
       */
      void SetSimulationTime(double newTime) { mSimulationTime = newTime; }
   private:

      System(); ///<private
      static System *mSystem;   ///<The System pointer
      static bool mInstanceFlag;///<Have we created a System yet?
      dtCore::Timer mClock;

      ///time keeping vars
      dtCore::Timer_t mClockTime, mSimulationClockTime, mLastClockTime;
      double mSimulationTime;
      double mTimeScale;
      double mDt;

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

      bool mRunning; ///<Are we currently running?      
      bool mShutdownOnWindowClose;
      bool mPaused;
      
      typedef std::map< Producer::RenderSurface*, Camera* > RenderSurfaceCameraMap;
      RenderSurfaceCameraMap mRenderSurfaceCameraMap;

   };
};



#endif // DELTA_SYSTEM
