#pragma once
#include <string>
#include <FL/Fl_Window.h>

#include "base.h"



///Fl_P51_Window  class to bridge the gap between FLTK and dtCore

/** The Fl_P51_Window class is an FLTK window that contains the Viewer
 *  class object.  It is also a dtCore::Base class so that communication
 *  can be made between the two objects via their sig-slot mechanisms.
 *
 *  Fl_P51_Window receives events from FLTK and relays them to the
 *  Viewer class object, and receives events from the Viewer class
 *  object and relays them to FLTK.
 */
class Fl_P51_Window  :  public   Fl_Window,  public   dtCore::Base
{
   DECLARE_MANAGEMENT_LAYER(Fl_P51_Window)

   typedef  Fl_Window   Parent;

   public:
                           /**
                            * Constructors.
                            *
                            * @param x horizontal window position (left)
                            * @param y vertical window position (top)
                            * @param w window width (right)
                            * @param h window height (bottom)
                            * @param label window's label
                            */
                           Fl_P51_Window();
                           Fl_P51_Window( int w, int h, const char* label = 0L );
                           Fl_P51_Window( int x, int y, int w, int h, const char* label = 0L );

                           /**
                            * Destructor.
                            */
      virtual              ~Fl_P51_Window();

                           /**
                            * Base override to receive messages.
                            *
                            * @param data the message to receive
                            */
      virtual  void        OnMessage( MessageData* data );

   protected:
                           /**
                            * Fl_Window override to draw the widget.
                            */
      virtual  void        draw( void );

                           /**
                            * Fl_Window override to configure the widget
                            * and prepare it for visibility on the screen.
                            */
      virtual  void        show( void );

                           /**
                            * Fl_Window override to update a window resize event.
                            * Passes the event to the Viewer class object.
                            *
                            * @param x horizontal window position (left)
                            * @param y vertical window position (top)
                            * @param w window width (right)
                            * @param h window height (bottom)
                            */
      virtual  void        resize( int x, int y, int w, int h );

                           /**
                            * Fl_Window override to handle event's from FLTK.
                            * 
                            * @param event a single event from FLTK
                            * 
                            * @return non-zero on handled, zero on not handled
                            */
      virtual  int         handle( int event );

                           /**
                            * Static callback, repeatedly called when FLTK is idle.
                            * Function casts void pointer to this object's type
                            * then calls the overloaded (non-static) callback.
                            * 
                            * @param data pointer to this object cast as void*
                            */
      static   void        idle_callback( void* data );

                           /**
                            * Callback, repeatedly called when FLTK is idle.
                            * Calls the Viewer's step function to advance one
                            * frame of the 3D scene.
                            */
               void        idle_callback( void );

   private:
                           /**
                            * Inline unified constructor.
                            */
      inline   void        ctor( void );

   private:
                           ///Pointer to Viewer class object this class contains
               Base*       viewer;
};
