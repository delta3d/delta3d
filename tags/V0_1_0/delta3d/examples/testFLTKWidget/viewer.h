#pragma once

#include <string>

#include <base.h>
#include <window.h>
#include <scene.h>
#include <camera.h>

#include "helperstructs.h"



///Viewer  class to handle all dtCore rendering for the application

/** The Viewer class is a dtCore::Base class object which receives
 *  window events from it's container class and performs all dtCore
 *  rendering.  It communicats with it's container class strictly
 *  through the sig-slot mechanisms.
 *
 *  The Viewer class, in essence, replaces the dtABC::Application
 *  class, allowing single frame cycle steps instead of a run-loop.
 *  Every time Viewer::Step() is called, another frame cycle advances.
 *  This allows the frame cycles to be merged into a windowing system's
 *  idle event handling loop.
 */
class Viewer :  public   dtCore::Base
{
   DECLARE_MANAGEMENT_LAYER(Viewer)

      ///Default path and file names for this example only
      static   const std::string DEF_PATH;
      static   const std::string DEF_FILE;

                              ///Event types receive from the windowing system
               enum           AsyncEvents
                              {
                                 RESIZE   = BIT(0),   ///Window size changed
                              };
   public:
                              /**
                               * Constructor.
                               */
                              Viewer();

                              /**
                               * Destructor.
                               */
      virtual                 ~Viewer();

   private:
                              /**
                               * Base override to receive messages.
                               *
                               * @param data the message to receive
                               */
      virtual  void           OnMessage( dtCore::Base::MessageData* data );

                              /**
                               * Configure dtCore to render in a given window.
                               *
                               * @param data the window handle and size
                               */
      inline   void           Config( const WinData* data = NULL );

                              /**
                               * Handle a window resize event.
                               */
      inline   void           Resize( void );

                              /**
                               * Advance a single frame cycle.
                               */
      inline   void           Step( void );

                              /**
                               * Called durring the frame cycle steps.
                               *
                               * @param deltaFrameTime time since last call
                               */
      inline   void           PreFrame( const double deltaFrameTime );

                              /**
                               * Called durring the frame cycle steps.
                               *
                               * @param deltaFrameTime time since last call
                               */
      inline   void           Frame( const double deltaFrameTime );

                              /**
                               * Called durring the frame cycle steps.
                               *
                               * @param deltaFrameTime time since last call
                               */
      inline   void           PostFrame( const double deltaFrameTime );

                              /**
                               * Inline unified constructor.
                               */
      inline   void           ctor( void );

   private:
               bool           cfg;  ///flag to prevent configuration twice
               dtCore::Camera*   cam;  ///camera into the scene
               int            cmd;  ///packed command cache
               WinRect        rect; ///cached window data for resizes
};
