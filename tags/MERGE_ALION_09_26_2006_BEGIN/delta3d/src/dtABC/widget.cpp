#include <cassert>
#include <dtCore/system.h>
#include <dtCore/globals.h>
#include <dtCore/mouse.h>
#include <dtCore/keyboard.h>

#include <dtABC/widget.h>
#include <dtCore/deltawin.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>    ///<\todo needs to be included because of some issue with BaseABC.

#if !defined(__APPLE__) && !defined(_WIN32) && !defined(WIN32) && !defined(__WIN32__)
#include "X11/Xlib.h"
#endif

/** name spaces */
using namespace   dtABC;
using namespace   dtCore;



IMPLEMENT_MANAGEMENT_LAYER(Widget)



/** static member variables */
const char* Widget::msgRedraw          = "redraw";
const char* Widget::msgStopped         = "stopped";

const char* Widget::msgStep            = "step";
const char* Widget::msgMouseEvent      = "mouse_event";
const char* Widget::msgKeyboardEvent   = "keyboard_event";
const char* Widget::msgResize          = "resize";
const char* Widget::msgAddDrawable     = "adddrawable";
const char* Widget::msgSetPath         = "setpath";
const char* Widget::msgWindowData      = "windata";
const char* Widget::msgQuit            = "quit";


Widget::Widget( const std::string& name /*= "Widget"*/ )
:  BaseABC(name)
{
   RegisterInstance( this );
   CreateInstances();
}

Widget::~Widget()
{
   DeregisterInstance( this );
}

void Widget::Config( const WinData* d /*= NULL*/ )
{
   System*  sys   = System::Instance();
   assert( sys );

   if( sys->IsRunning() )
      // don't configure twice
      return;

   if( d != NULL )
   {
      dtCore::RefPtr<Producer::Camera> pc = mCamera->GetCamera();
      assert( pc.get() );

      dtCore::RefPtr<Producer::RenderSurface> rs = new Producer::RenderSurface;
      assert( rs.get() );

      dtCore::RefPtr<Producer::InputArea> ia = new Producer::InputArea;
      assert( ia.get() );
      
      rs->setWindow( d->hwnd );
      rs->setWindowRectangle( d->pos_x, d->pos_y, d->width, d->height );
      rs->setInputRectangle( Producer::RenderSurface::InputRectangle( d->pos_x, d->width, d->pos_y, d->height ) );

      ia->addRenderSurface( rs.get() );

      pc->setRenderSurface( rs.get() );

      mWindow  = new dtCore::DeltaWin( "Widget", rs.get(), ia.get() );
      assert( mWindow.get() );

      mKeyboard = mWindow->GetKeyboard();
      mMouse = mWindow->GetMouse();
      sys->Start();
   }
   
   BaseABC::Config();
}

void Widget::Quit( void )
{
   BaseABC::Quit();
   SendMessage( msgStopped );
}

void Widget::SetPath( std::string path )
{
   SetDataFilePathList( path );
}

void Widget::PreFrame( const double deltaFrameTime )
{
}

void Widget::Frame( const double deltaFrameTime )
{
}

void Widget::PostFrame( const double deltaFrameTime )
{
}

void Widget::OnMessage( Base::MessageData* data )
{
   assert( data );

   if( data->message == msgStep )
   {
      Step();
      return;
   }

   if( data->message == msgMouseEvent )
   {
      assert( data->userData );
      HandleMouseEvent( *(static_cast<MouseEvent*>(data->userData)) );
      return;
   }

   if( data->message == msgKeyboardEvent )
   {
      assert( data->userData );
      HandleKeyboardEvent( *(static_cast<KeyboardEvent*>(data->userData)) );
      return;
   }

   if( data->message == msgResize )
   {
      Resize( static_cast<WinRect*>(data->userData) );
      return;
   }

   if( data->message == msgAddDrawable )
   {
      AddDrawable( static_cast<dtCore::DeltaDrawable*>(data->userData) );
      return;
   }

   if( data->message == msgSetPath )
   {
      SetPath( *(static_cast<std::string*>(data->userData)) );
      return;
   }

   if( data->message == msgWindowData )
   {
      Config( static_cast<WinData*>(data->userData) );
      return;
   }

   if( data->message == msgQuit )
   {
      Quit();
      return;
   }

   BaseABC::OnMessage( data );
}

void Widget::Step( void )
{
   System*  sys   = System::Instance();
   assert( sys );

   sys->Step();
   //SendMessage( msgRedraw );
}

void Widget::Resize( const WinRect* r )
{
   assert( r );

   Producer::Camera* pcam  = mCamera->GetCamera();
   assert( pcam );

   Producer::RenderSurface*   prs   = pcam->getRenderSurface();
   assert( prs );

   prs->setWindowRectangle( r->pos_x, r->pos_y, r->width, r->height, false );
}

void Widget::HandleMouseEvent( const MouseEvent& ev )
{
   assert( mMouse.get() );

   // an unknown button number defaults to LeftButton.
   dtCore::Mouse::MouseButton mb( dtCore::Mouse::LeftButton );
   
   switch( ev.button )
   {
      case 0 :
      {
         mb = dtCore::Mouse::LeftButton;
         break;
      }
      case 1 :
      {
         mb = dtCore::Mouse::MiddleButton;
         break;
      }
      case 2 :
      {
         mb = dtCore::Mouse::RightButton;
         break;
      }
      default :
      {
         mb = dtCore::Mouse::LeftButton;
         break;
      }
   }

   switch( ev.event )
   {
       case  MouseEvent::PUSH:
         mMouse->ButtonDown( ev.pos_x, ev.pos_y, mb );
         break;

      case  MouseEvent::DOUBLE:
         mMouse->DoubleButtonDown( ev.pos_x, ev.pos_y, mb );
         break;

      case  MouseEvent::RELEASE:
         mMouse->ButtonUp( ev.pos_x, ev.pos_y, mb );
         break;

      case  MouseEvent::DRAG:
         mMouse->MouseMotion( ev.pos_x, ev.pos_y );
         break;

      case  MouseEvent::MOVE:
         mMouse->PassiveMouseMotion( ev.pos_x, ev.pos_y );
         break;

      case  MouseEvent::WHEEL_UP:
         mMouse->MouseScroll( Producer::KeyboardMouseCallback::ScrollUp );
         break;

      case  MouseEvent::WHEEL_DN:
         mMouse->MouseScroll( Producer::KeyboardMouseCallback::ScrollDown );
         break;

      default:
         break;
   }
}

void Widget::HandleKeyboardEvent( const KeyboardEvent& ev )
{
   assert( mKeyboard.get() );

   switch( ev.event )
   {
   case  KeyboardEvent::KEYDOWN:
      mKeyboard->KeyDown( Producer::KeyCharacter(ev.key) );
      break;

   case  KeyboardEvent::KEYUP:
      mKeyboard->KeyUp( Producer::KeyCharacter(ev.key) );
      break;

   default:
      break;
   }
}

bool Widget::IsSpecialKeyboardEvent( const KeyboardEvent& ev )
{
   switch( ev.key )
   {
      case  Producer::KeyChar_Escape:
      case  Producer::KeyChar_F1:
      case  Producer::KeyChar_F2:
      case  Producer::KeyChar_F3:
      case  Producer::KeyChar_F4:
      case  Producer::KeyChar_F5:
      case  Producer::KeyChar_F6:
      case  Producer::KeyChar_F7:
      case  Producer::KeyChar_F8:
      case  Producer::KeyChar_F9:
      case  Producer::KeyChar_F10:
      case  Producer::KeyChar_F11:
      case  Producer::KeyChar_F12:
      case  Producer::KeyChar_Tab:
      case  Producer::KeyChar_Caps_Lock:
      case  Producer::KeyChar_Shift_L:
      case  Producer::KeyChar_Control_L:
      case  Producer::KeyChar_Meta_L:
      case  Producer::KeyChar_Meta_R:
      case  Producer::KeyChar_Control_R:
      case  Producer::KeyChar_Shift_R:
      case  Producer::KeyChar_Return:
      case  Producer::KeyChar_BackSpace:
      case  Producer::KeyChar_Scroll_Lock:
      case  Producer::KeyChar_Pause:
      case  Producer::KeyChar_Insert:
      case  Producer::KeyChar_Home:
      case  Producer::KeyChar_Page_Up:
      case  Producer::KeyChar_Delete:
      case  Producer::KeyChar_End:
      case  Producer::KeyChar_Page_Down:
      case  Producer::KeyChar_Up:
      case  Producer::KeyChar_Left:
      case  Producer::KeyChar_Down:
      case  Producer::KeyChar_Right:
      case  Producer::KeyChar_Num_Lock:
      case  Producer::KeyChar_KP_Divide:
      case  Producer::KeyChar_KP_Multiply:
      case  Producer::KeyChar_KP_Subtract:
      case  Producer::KeyChar_KP_Add:
      case  Producer::KeyChar_KP_Enter:
      case  Producer::KeyChar_KP_Home:
      case  Producer::KeyChar_KP_Up:
      case  Producer::KeyChar_KP_Page_Up:
      case  Producer::KeyChar_KP_Left:
      case  Producer::KeyChar_KP_Right:
      case  Producer::KeyChar_KP_End:
      case  Producer::KeyChar_KP_Down:
      case  Producer::KeyChar_KP_Page_Down:
      case  Producer::KeyChar_KP_Insert:
      case  Producer::KeyChar_KP_Delete:
         return   true;
         break;

      default:
         break;
   }

   return   false;
}



/** Helper struct constructors */

/**
 * Default Constructor.
 *
 * @param x horizontal position
 * @param y vertical position
 * @param w width
 * @param h height
 */
WinRect::WinRect( int x, int y, int w, int h )
:  pos_x(x),
   pos_y(y),
   width(w),
   height(h)
{
}



/**
 * Copy Constructor.
 *
 * @param that object to copy from
 */
WinRect::WinRect( const WinRect& that )
:  pos_x(that.pos_x),
   pos_y(that.pos_y),
   width(that.width),
   height(that.height)
{
}



/**
 * Copy Operator.
 *
 * @param that object to copy from
 *
 * @return a reference to this object
 */
WinRect&
WinRect::operator=( const WinRect& that )
{
   pos_x    = that.pos_x;
   pos_y    = that.pos_y;
   width    = that.width;
   height   = that.height;

   return   *this;
}



/**
 * Default Constructor.
 *
 * @param hw window handle
 * @param x horizontal position
 * @param y vertical position
 * @param w width
 * @param h height
 */
WinData::WinData( Producer::Window hw, int x, int y, int w, int h )
:  WinRect(x,y,w,h),
   hwnd(hw)
{
}



/**
 * Copy Constructor.
 *
 * @param that object to copy from
 */
WinData::WinData( const WinData& that )
:  WinRect(that),
   hwnd(that.hwnd)
{
}



/**
 * Copy Constructor.
 *
 * @param that object to copy from
 */
WinData::WinData( const WinRect& that )
:  WinRect(that),
   hwnd(0L)
{
}



/**
 * Copy Operator.
 *
 * @param that object to copy from
 *
 * @return a reference to this object
 */
WinData&
WinData::operator=( const WinData& that )
{
   WinRect(*this) = WinRect(that);
   hwnd           = that.hwnd;

   return   *this;
}



/**
 * Default Constructor.
 *
 * @param ev mouse event
 * @param px horizontal position
 * @param py vertical position
 * @param bt the button number
 */
MouseEvent::MouseEvent( int ev, float px, float py, int bt )
:  event(ev),
   pos_x(px),
   pos_y(py),
   button(bt)
{
}



/**
 * Copy Constructor.
 *
 * @param that object to copy from
 */
MouseEvent::MouseEvent( const MouseEvent& that )
:  event(that.event),
   pos_x(that.pos_x),
   pos_y(that.pos_y),
   button(that.button)
{
}



/**
 * Copy Operator.
 *
 * @param that object to copy from
 *
 * @return a reference to this object
 */
MouseEvent&
MouseEvent::operator=( const MouseEvent& that )
{
   event    = that.event;
   pos_x    = that.pos_x;
   pos_y    = that.pos_y;
   button   = that.button;

   return   *this;
}



KeyboardEvent::KeyboardEvent( int ev, int ky, int md, char ch )
:  event(ev),
   key(ky),
   mod(md),
   chr(ch)
{
}



/**
 * Copy Constructor.
 *
 * @param that object to copy from
 */
KeyboardEvent::KeyboardEvent( const KeyboardEvent& that )
:  event(that.event),
   key(that.key),
   mod(that.mod),
   chr(that.chr)
{
}



/**
 * Copy Operator.
 *
 * @param that object to copy from
 *
 * @return a reference to this object
 */
KeyboardEvent&
KeyboardEvent::operator=( const KeyboardEvent& that )
{
   event = that.event;
   key   = that.key;
   mod   = that.mod;
   chr   = that.chr;

   return   *this;
}



const int   KeyboardEvent::KEY_BackSpace     = Producer::KeyChar_BackSpace;
const int   KeyboardEvent::KEY_Tab           = Producer::KeyChar_Tab;
const int   KeyboardEvent::KEY_Enter         = Producer::KeyChar_Return;
const int   KeyboardEvent::KEY_Pause         = Producer::KeyChar_Pause;
const int   KeyboardEvent::KEY_Scroll_Lock   = Producer::KeyChar_Scroll_Lock;
const int   KeyboardEvent::KEY_Sys_Req       = Producer::KeyChar_Sys_Req;
const int   KeyboardEvent::KEY_Escape        = Producer::KeyChar_Escape;
const int   KeyboardEvent::KEY_Home          = Producer::KeyChar_Home;
const int   KeyboardEvent::KEY_Left          = Producer::KeyChar_Left;
const int   KeyboardEvent::KEY_Up            = Producer::KeyChar_Up;
const int   KeyboardEvent::KEY_Right         = Producer::KeyChar_Right;
const int   KeyboardEvent::KEY_Down          = Producer::KeyChar_Down;
const int   KeyboardEvent::KEY_Page_Up       = Producer::KeyChar_Page_Up;
const int   KeyboardEvent::KEY_Page_Down     = Producer::KeyChar_Page_Down;
const int   KeyboardEvent::KEY_End           = Producer::KeyChar_End;
const int   KeyboardEvent::KEY_Print         = Producer::KeyChar_Print;
const int   KeyboardEvent::KEY_Insert        = Producer::KeyChar_Insert;
const int   KeyboardEvent::KEY_Menu          = Producer::KeyChar_Menu;
const int   KeyboardEvent::KEY_Help          = Producer::KeyChar_Help;
const int   KeyboardEvent::KEY_Num_Lock      = Producer::KeyChar_Num_Lock;
const int   KeyboardEvent::KEY_KP_Enter      = Producer::KeyChar_KP_Enter;
const int   KeyboardEvent::KEY_Shift_L       = Producer::KeyChar_Shift_R;
const int   KeyboardEvent::KEY_Shift_R       = Producer::KeyChar_Shift_L;
const int   KeyboardEvent::KEY_Control_L     = Producer::KeyChar_Control_R;
const int   KeyboardEvent::KEY_Control_R     = Producer::KeyChar_Control_L;
const int   KeyboardEvent::KEY_Caps_Lock     = Producer::KeyChar_Caps_Lock;
const int   KeyboardEvent::KEY_Meta_L        = Producer::KeyChar_Meta_L;
const int   KeyboardEvent::KEY_Meta_R        = Producer::KeyChar_Meta_R;
const int   KeyboardEvent::KEY_Alt_L         = Producer::KeyChar_Alt_L;
const int   KeyboardEvent::KEY_Alt_R         = Producer::KeyChar_Alt_R;
const int   KeyboardEvent::KEY_Delete        = Producer::KeyChar_Delete;
const int   KeyboardEvent::KEY_Space         = Producer::KeyChar_space;
const int   KeyboardEvent::KEY_Exclam        = Producer::KeyChar_exclam;
const int   KeyboardEvent::KEY_DblQuote      = Producer::KeyChar_quotedbl;
const int   KeyboardEvent::KEY_NumSign       = Producer::KeyChar_numbersign;
const int   KeyboardEvent::KEY_Dollar        = Producer::KeyChar_dollar;
const int   KeyboardEvent::KEY_Percent       = Producer::KeyChar_percent;
const int   KeyboardEvent::KEY_Ampersand     = Producer::KeyChar_ampersand;
const int   KeyboardEvent::KEY_Apostrophe    = Producer::KeyChar_apostrophe;
const int   KeyboardEvent::KEY_Paren_L       = Producer::KeyChar_parenleft;
const int   KeyboardEvent::KEY_Paren_R       = Producer::KeyChar_parenright;
const int   KeyboardEvent::KEY_Asterisk      = Producer::KeyChar_asterisk;
const int   KeyboardEvent::KEY_Plus          = Producer::KeyChar_plus;
const int   KeyboardEvent::KEY_Comma         = Producer::KeyChar_comma;
const int   KeyboardEvent::KEY_Minus         = Producer::KeyChar_minus;
const int   KeyboardEvent::KEY_Period        = Producer::KeyChar_period;
const int   KeyboardEvent::KEY_Slash         = Producer::KeyChar_slash;
const int   KeyboardEvent::KEY_0             = Producer::KeyChar_0;
const int   KeyboardEvent::KEY_1             = Producer::KeyChar_1;
const int   KeyboardEvent::KEY_2             = Producer::KeyChar_2;
const int   KeyboardEvent::KEY_3             = Producer::KeyChar_3;
const int   KeyboardEvent::KEY_4             = Producer::KeyChar_4;
const int   KeyboardEvent::KEY_5             = Producer::KeyChar_5;
const int   KeyboardEvent::KEY_6             = Producer::KeyChar_6;
const int   KeyboardEvent::KEY_7             = Producer::KeyChar_7;
const int   KeyboardEvent::KEY_8             = Producer::KeyChar_8;
const int   KeyboardEvent::KEY_9             = Producer::KeyChar_9;
const int   KeyboardEvent::KEY_Colon         = Producer::KeyChar_colon;
const int   KeyboardEvent::KEY_Semicolon     = Producer::KeyChar_semicolon;
const int   KeyboardEvent::KEY_Less          = Producer::KeyChar_less;
const int   KeyboardEvent::KEY_Equal         = Producer::KeyChar_equal;
const int   KeyboardEvent::KEY_Greater       = Producer::KeyChar_greater;
const int   KeyboardEvent::KEY_Question      = Producer::KeyChar_question;
const int   KeyboardEvent::KEY_At            = Producer::KeyChar_at;
const int   KeyboardEvent::KEY_A             = Producer::KeyChar_A;
const int   KeyboardEvent::KEY_B             = Producer::KeyChar_B;
const int   KeyboardEvent::KEY_C             = Producer::KeyChar_C;
const int   KeyboardEvent::KEY_D             = Producer::KeyChar_D;
const int   KeyboardEvent::KEY_E             = Producer::KeyChar_E;
const int   KeyboardEvent::KEY_F             = Producer::KeyChar_F;
const int   KeyboardEvent::KEY_G             = Producer::KeyChar_G;
const int   KeyboardEvent::KEY_H             = Producer::KeyChar_H;
const int   KeyboardEvent::KEY_I             = Producer::KeyChar_I;
const int   KeyboardEvent::KEY_J             = Producer::KeyChar_J;
const int   KeyboardEvent::KEY_K             = Producer::KeyChar_K;
const int   KeyboardEvent::KEY_L             = Producer::KeyChar_L;
const int   KeyboardEvent::KEY_M             = Producer::KeyChar_M;
const int   KeyboardEvent::KEY_N             = Producer::KeyChar_N;
const int   KeyboardEvent::KEY_O             = Producer::KeyChar_O;
const int   KeyboardEvent::KEY_P             = Producer::KeyChar_P;
const int   KeyboardEvent::KEY_Q             = Producer::KeyChar_Q;
const int   KeyboardEvent::KEY_R             = Producer::KeyChar_R;
const int   KeyboardEvent::KEY_S             = Producer::KeyChar_S;
const int   KeyboardEvent::KEY_T             = Producer::KeyChar_T;
const int   KeyboardEvent::KEY_U             = Producer::KeyChar_U;
const int   KeyboardEvent::KEY_V             = Producer::KeyChar_V;
const int   KeyboardEvent::KEY_W             = Producer::KeyChar_W;
const int   KeyboardEvent::KEY_X             = Producer::KeyChar_X;
const int   KeyboardEvent::KEY_Y             = Producer::KeyChar_Y;
const int   KeyboardEvent::KEY_Z             = Producer::KeyChar_Z;
const int   KeyboardEvent::KEY_Bracket_L     = Producer::KeyChar_bracketleft;
const int   KeyboardEvent::KEY_Backslash     = Producer::KeyChar_backslash;
const int   KeyboardEvent::KEY_Bracket_R     = Producer::KeyChar_bracketright;
const int   KeyboardEvent::KEY_Caret         = Producer::KeyChar_asciicircum;
const int   KeyboardEvent::KEY_UnderScore    = Producer::KeyChar_underscore;
const int   KeyboardEvent::KEY_Quote_L       = Producer::KeyChar_quoteleft;
const int   KeyboardEvent::KEY_a             = Producer::KeyChar_a;
const int   KeyboardEvent::KEY_b             = Producer::KeyChar_b;
const int   KeyboardEvent::KEY_c             = Producer::KeyChar_c;
const int   KeyboardEvent::KEY_d             = Producer::KeyChar_d;
const int   KeyboardEvent::KEY_e             = Producer::KeyChar_e;
const int   KeyboardEvent::KEY_f             = Producer::KeyChar_f;
const int   KeyboardEvent::KEY_g             = Producer::KeyChar_g;
const int   KeyboardEvent::KEY_h             = Producer::KeyChar_h;
const int   KeyboardEvent::KEY_i             = Producer::KeyChar_i;
const int   KeyboardEvent::KEY_j             = Producer::KeyChar_j;
const int   KeyboardEvent::KEY_k             = Producer::KeyChar_k;
const int   KeyboardEvent::KEY_l             = Producer::KeyChar_l;
const int   KeyboardEvent::KEY_m             = Producer::KeyChar_m;
const int   KeyboardEvent::KEY_n             = Producer::KeyChar_n;
const int   KeyboardEvent::KEY_o             = Producer::KeyChar_o;
const int   KeyboardEvent::KEY_p             = Producer::KeyChar_p;
const int   KeyboardEvent::KEY_q             = Producer::KeyChar_q;
const int   KeyboardEvent::KEY_r             = Producer::KeyChar_r;
const int   KeyboardEvent::KEY_s             = Producer::KeyChar_s;
const int   KeyboardEvent::KEY_t             = Producer::KeyChar_t;
const int   KeyboardEvent::KEY_u             = Producer::KeyChar_u;
const int   KeyboardEvent::KEY_v             = Producer::KeyChar_v;
const int   KeyboardEvent::KEY_w             = Producer::KeyChar_w;
const int   KeyboardEvent::KEY_x             = Producer::KeyChar_x;
const int   KeyboardEvent::KEY_y             = Producer::KeyChar_y;
const int   KeyboardEvent::KEY_z             = Producer::KeyChar_z;
const int   KeyboardEvent::KEY_Brace_L       = Producer::KeyChar_braceleft;
const int   KeyboardEvent::KEY_Bar           = Producer::KeyChar_bar;
const int   KeyboardEvent::KEY_Brace_R       = Producer::KeyChar_braceright;
const int   KeyboardEvent::KEY_Tilde         = Producer::KeyChar_asciitilde;
const int   KeyboardEvent::KEY_KP_Home       = Producer::KeyChar_KP_Home;
const int   KeyboardEvent::KEY_KP_Left       = Producer::KeyChar_KP_Left;
const int   KeyboardEvent::KEY_KP_Up         = Producer::KeyChar_KP_Up;
const int   KeyboardEvent::KEY_KP_Right      = Producer::KeyChar_KP_Right;
const int   KeyboardEvent::KEY_KP_Down       = Producer::KeyChar_KP_Down;
const int   KeyboardEvent::KEY_KP_PageUp     = Producer::KeyChar_KP_Page_Up;
const int   KeyboardEvent::KEY_KP_PageDown   = Producer::KeyChar_KP_Page_Down;
const int   KeyboardEvent::KEY_KP_End        = Producer::KeyChar_KP_End;
const int   KeyboardEvent::KEY_KP_Insert     = Producer::KeyChar_KP_Insert;
const int   KeyboardEvent::KEY_KP_Delete     = Producer::KeyChar_KP_Delete;
const int   KeyboardEvent::KEY_KP_Mul        = Producer::KeyChar_KP_Multiply;
const int   KeyboardEvent::KEY_KP_Add        = Producer::KeyChar_KP_Add;
const int   KeyboardEvent::KEY_KP_Sub        = Producer::KeyChar_KP_Subtract;
const int   KeyboardEvent::KEY_KP_Dec        = Producer::KeyChar_KP_Decimal;
const int   KeyboardEvent::KEY_KP_Div        = Producer::KeyChar_KP_Divide;
const int   KeyboardEvent::KEY_KP_0          = Producer::KeyChar_KP_0;
const int   KeyboardEvent::KEY_KP_1          = Producer::KeyChar_KP_1;
const int   KeyboardEvent::KEY_KP_2          = Producer::KeyChar_KP_2;
const int   KeyboardEvent::KEY_KP_3          = Producer::KeyChar_KP_3;
const int   KeyboardEvent::KEY_KP_4          = Producer::KeyChar_KP_4;
const int   KeyboardEvent::KEY_KP_5          =  Producer::KeyChar_KP_5;
const int   KeyboardEvent::KEY_KP_6          = Producer::KeyChar_KP_6;
const int   KeyboardEvent::KEY_KP_7          = Producer::KeyChar_KP_7;
const int   KeyboardEvent::KEY_KP_8          = Producer::KeyChar_KP_8;
const int   KeyboardEvent::KEY_KP_9          = Producer::KeyChar_KP_9;
const int   KeyboardEvent::KEY_KP_Equal      = Producer::KeyChar_KP_Equal;
const int   KeyboardEvent::KEY_F1            = Producer::KeyChar_F1;
const int   KeyboardEvent::KEY_F2            = Producer::KeyChar_F2;
const int   KeyboardEvent::KEY_F3            = Producer::KeyChar_F3;
const int   KeyboardEvent::KEY_F4            = Producer::KeyChar_F4;
const int   KeyboardEvent::KEY_F5            = Producer::KeyChar_F5;
const int   KeyboardEvent::KEY_F6            = Producer::KeyChar_F6;
const int   KeyboardEvent::KEY_F7            = Producer::KeyChar_F7;
const int   KeyboardEvent::KEY_F8            = Producer::KeyChar_F8;
const int   KeyboardEvent::KEY_F9            = Producer::KeyChar_F9;
const int   KeyboardEvent::KEY_F10           = Producer::KeyChar_F10;
const int   KeyboardEvent::KEY_F11           = Producer::KeyChar_F11;
const int   KeyboardEvent::KEY_F12           = Producer::KeyChar_F12;
