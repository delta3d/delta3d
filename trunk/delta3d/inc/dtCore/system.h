/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
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


#include "dtCore/base.h"
#include "timer.h"

namespace dtCore
{
   /// Performs high-level frame coordination

   /** The System is used to control the frame-based coordination of all the 
     * dtCore classes.  There is only one instance of the System available and is
     * always accessable using Instance();
     * Once the application has created all the required instances, System::Run() 
     * can be called and will block until System::Stop() gets called.  By 
     * subscribing to the System, Base derived objects can receive the System 
     * messages "preframe", "frame", "postframe", and "configure".
     * @see AddListener()
     * @see OnMessage()
     */
   class DT_EXPORT System : public Base 
   {
   public:
      DECLARE_MANAGEMENT_LAYER(System)

      ///Perform any configuration required.  Message: "configure"
      void Config(void);

      ///Get a pointer to the System
      static System *Instance();

      ///Toggles the running flag to true
      void Start(void);

      ///One complete System frame
      void Step(void);

      ///Stop the System from running
      void Stop(void);

      ///Start the System running
      void Run(void);

      ///Is the system running
      bool IsRunning(void) const { return mRunning; }

      virtual ~System();
   private:
      System(); ///<private
      static System *mSystem;   ///<The System pointer
      static bool mInstanceFlag;///<Have we created a System yet?
      Timer mClock;

      ///Stuff to do before the frame. Message: "preframe", delta time in seconds
      void PreFrame( const double deltaFrameTime );

      ///Render the Camera, etc.  Message: "frame", delta time in seconds
      void Frame(  const double deltaFrameTime );

      ///Stuff to do after the frame.  Message: "postframe", delta time in seconds
      void PostFrame( const double deltaFrameTime );

      bool mRunning; ///<Are we currently running?      
   };
};



#endif // DELTA_SYSTEM
