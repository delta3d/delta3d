// system.h: interface for the System class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSTEM_H__41AEFCFC_3F25_4467_8F12_D10A5605A1A4__INCLUDED_)
#define AFX_SYSTEM_H__41AEFCFC_3F25_4467_8F12_D10A5605A1A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "base.h"
#include "ul.h"

namespace dtCore
{
   /// Performs high-level frame coordination

   /** The System is used to control the frame-based coordination of all the 
     * dtCore classes.  There is only one instance of the System available and is
     * always accessable using GetSystem();
     * Once the application has created all the required instances, System::Run() 
     * can be called and will block until System::Stop() gets called.  By 
     * subscribing to the System, Base derived objects can receive the System 
     * messages "preframe", "frame", "postframe", and "configure".
     * @see AddListener()
     * @see OnMessage()
     */
   class System : public Base 
   {
   public:
      DECLARE_MANAGEMENT_LAYER(System)

      ///Perform any configuration required.  Message: "configure"
      void Config(void);

      ///Get a pointer to the System
      static System *GetSystem(void);

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
      ulClock  clock;

      ///Stuff to do before the frame. Message: "preframe", delta time in seconds
      void PreFrame( const double deltaFrameTime );

      ///Render the Camera, etc.  Message: "frame", delta time in seconds
      void Frame(  const double deltaFrameTime );

      ///Stuff to do after the frame.  Message: "postframe", delta time in seconds
      void PostFrame( const double deltaFrameTime );

      bool mRunning; ///<Are we currently running?      
   };
};


#endif // !defined(AFX_SYSTEM_H__41AEFCFC_3F25_4467_8F12_D10A5605A1A4__INCLUDED_)
