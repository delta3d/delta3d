#include <assert.h>

#include <FL/Fl.H>
#include <FL/x.H>

#include "helperstructs.h"
#include "viewer.h"
#include "fl_p51_window.h"



using namespace   dtCore;



IMPLEMENT_MANAGEMENT_LAYER(Fl_P51_Window)



/**
 * Default Constructor.
 */
Fl_P51_Window::Fl_P51_Window()
:  Parent(WinRect::DEF_X, WinRect::DEF_Y, WinRect::DEF_W, WinRect::DEF_H),
   viewer(NULL)
{
   RegisterInstance( this );
   ctor();
}



/**
 * Constructor.
 *
 * @param w window width (right)
 * @param h window height (bottom)
 * @param label window's label
 */
Fl_P51_Window::Fl_P51_Window( int w, int h, const char* label /*= 0L*/ )
:  Parent(w, h, label),
   viewer(NULL)
{
   RegisterInstance( this );
   ctor();
}



/**
 * Constructor.
 *
 * @param x horizontal window position (left)
 * @param y vertical window position (top)
 * @param w window width (right)
 * @param h window height (bottom)
 * @param label window's label
 */
Fl_P51_Window::Fl_P51_Window( int x, int y, int w, int h, const char* label /*= 0L*/ )
:  Parent(x, y, w, h, label),
   viewer(NULL)
{
   RegisterInstance( this );
   ctor();
}



/**
 * Destructor.
 */
Fl_P51_Window::~Fl_P51_Window()
{
   if( viewer )
   {
      delete   viewer;
      viewer = NULL;
   }

   DeregisterInstance( this );
}



/**
 * Base override to receive messages.
 *
 * @param data the message to receive
 */
void
Fl_P51_Window::OnMessage( MessageData* data )
{
   assert( data );

   if( data->message == "redraw" )
   {
      Parent::redraw();
      return;
   }
}



/**
 * Fl_Window override to draw the widget.
 */
void
Fl_P51_Window::draw()
{
   Parent::draw();
}




/**
 * Fl_Window override to configure the widget
 * and prepare it for visibility on the screen.
 */
void
Fl_P51_Window::show()
{
   Parent::show();

   WinData  windata( fl_xid(this),
                     Parent::x(),
                     Parent::y(),
                     Parent::w(),
                     Parent::h() );

   SendMessage( "windata", &windata );
   Fl::add_idle( idle_callback, this );
}



/**
 * Fl_Window override to update a window resize event.
 * Passes the event to the Viewer class object.
 *
 * @param x horizontal window position (left)
 * @param y vertical window position (top)
 * @param w window width (right)
 * @param h window height (bottom)
 */
void
Fl_P51_Window::resize( int x, int y, int w, int h )
{
   Parent::resize( x, y, w, h );

   WinRect rect(x, y, w, h);
   SendMessage( "resize", &rect );
}



/**
 * Fl_Window override to handle event's from FLTK.
 * 
 * @param event a single event from FLTK
 * 
 * @return non-zero on handled, zero on not handled
 */
int
Fl_P51_Window::handle( int event )
{
   bool  handled(false);

   switch( event )
   {
      case  FL_SHOW:
         return   Parent::handle( FL_SHOW );
         break;

      case  FL_PUSH:
         return   0;
         break;

      case  FL_DRAG:
         return   0;
         break;

      case  FL_RELEASE:
         return   0;
         break;

      case  FL_MOUSEWHEEL:
         return   0;
         break;

      case FL_KEYDOWN:
         return   0;
         break;

      case  FL_KEYUP:
         return   0;
         break;

      default:
         return   0;
         break;
   }
}



/**
 * Static callback, repeatedly called when FLTK is idle.
 * Function casts void pointer to this object's type
 * then calls the overloaded (non-static) callback.
 * 
 * @param data pointer to this object cast as void*
 */
void
Fl_P51_Window::idle_callback( void* data )
{
   assert( data );
   static_cast<Fl_P51_Window*>(data)->idle_callback();
}



/**
 * Callback, repeatedly called when FLTK is idle.
 * Calls the Viewer's step function to advance one
 * frame of the 3D scene.
 */
void
Fl_P51_Window::idle_callback( void )
{
   SendMessage( "step" );
}



/**
 * Inline unified constructor.
 */
void
Fl_P51_Window::ctor( void )
{
   end();
   viewer = new Viewer;
   assert( viewer );

   AddSender( viewer );
   viewer->AddSender( this );
}
