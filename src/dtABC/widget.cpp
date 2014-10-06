#include <cassert>
#include <dtCore/system.h>
#include <dtCore/mouse.h>
#include <dtCore/keyboard.h>

#include <dtABC/widget.h>
#include <dtCore/deltawin.h>
#include <dtCore/camera.h>
#include <dtCore/scene.h>    ///<\todo needs to be included because of some issue with BaseABC.

#include <dtUtil/datapathutils.h>
#include <dtUtil/exception.h>

#if !defined(__APPLE__) && !defined(_WIN32) && !defined(WIN32) && !defined(__WIN32__)
#include "X11/Xlib.h"
#endif

#include <osgViewer/CompositeViewer>

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

////////////////////////////////////////////////////////////////////////////////
Widget::Widget(const std::string& name /*= "Widget"*/)
   : BaseABC(name)
   , mIsInitialized(false)
{
   RegisterInstance(this);
   CreateInstances();
}

////////////////////////////////////////////////////////////////////////////////
Widget::~Widget()
{
   DeregisterInstance(this);
}

////////////////////////////////////////////////////////////////////////////////
void Widget::Config(const WinData* d /*= NULL*/)
{
   System* sys = &dtCore::System::GetInstance();
   assert(sys);

   if (sys->IsRunning())
   {
      // don't configure twice
      return;
   }

   if (d != NULL)
   {

#ifdef __APPLE__
      WindowData* inheritedWindowData = NULL;
      throw dtUtil::Exception("OSX doesn't support creating an ABC widget with an existing window handle.", __FILE__, __LINE__);
#else
      WindowData* inheritedWindowData = new WindowData(d->hwnd);
#endif
      dtCore::DeltaWin::DeltaWinTraits traits;
      traits.name = "Widget";
      traits.x = d->pos_x;
      traits.y = d->pos_y;
      traits.width = d->width;
      traits.height = d->height;
      traits.showCursor = true;
      traits.fullScreen = false;
      traits.inheritedWindowData = inheritedWindowData;

      mWindow = new dtCore::DeltaWin(traits);
      assert(mWindow.valid());

      GetCamera()->SetWindow(mWindow.get());
      sys->Start();
   }

   BaseABC::Config();

   mCompositeViewer = new osgViewer::CompositeViewer;
   mCompositeViewer->addView(mViewList.front()->GetOsgViewerView());

   mIsInitialized = true;
}

////////////////////////////////////////////////////////////////////////////////
void Widget::Quit(void)
{
   BaseABC::Quit();
   SendMessage(msgStopped);
}

////////////////////////////////////////////////////////////////////////////////
void Widget::SetPath(const std::string& path)
{
   dtUtil::SetDataFilePathList(path);
}

////////////////////////////////////////////////////////////////////////////////
void Widget::PreFrame(const double /*deltaFrameTime*/)
{
}

////////////////////////////////////////////////////////////////////////////////
void Widget::Frame(const double /*deltaFrameTime*/)
{
   mCompositeViewer->frame();
}

////////////////////////////////////////////////////////////////////////////////
void Widget::PostFrame(const double /*deltaFrameTime*/)
{
}

////////////////////////////////////////////////////////////////////////////////
void Widget::Pause(const double /*deltaRealTime*/)
{
}

////////////////////////////////////////////////////////////////////////////////
void Widget::OnMessage(Base::MessageData* data)
{
   assert(data);

   if (data->message == msgStep)
   {
      Step();
      return;
   }

   if (data->message == msgMouseEvent)
   {
      assert(data->userData);
      HandleMouseEvent(*(static_cast<MouseEvent*>(data->userData)));
      return;
   }

   if (data->message == msgKeyboardEvent)
   {
      assert(data->userData);
      HandleKeyboardEvent(*(static_cast<KeyboardEvent*>(data->userData)));
      return;
   }

   if (data->message == msgResize)
   {
      if (mIsInitialized)
      {
         Resize(static_cast<WinRect*>(data->userData));
      }
      return;
   }

   if (data->message == msgAddDrawable)
   {
      AddDrawable(static_cast<dtCore::DeltaDrawable*>(data->userData));
      return;
   }

   if (data->message == msgSetPath)
   {
      SetPath(*(static_cast<std::string*>(data->userData)));
      return;
   }

   if (data->message == msgWindowData)
   {
      Config(static_cast<WinData*>(data->userData));
      return;
   }

   if (data->message == msgQuit)
   {
      Quit();
      return;
   }

   BaseABC::OnMessage(data);
}

////////////////////////////////////////////////////////////////////////////////
void Widget::Step(void)
{
   System* sys = &dtCore::System::GetInstance();
   assert(sys);

   sys->Step();
   //SendMessage(msgRedraw);
}

////////////////////////////////////////////////////////////////////////////////
void Widget::Resize(const WinRect* r)
{
   assert(r);

   // tell OSG that the window has been resized.
   GetWindow()->GetOsgViewerGraphicsWindow()->resized(r->pos_x, r->pos_y, r->width, r->height);
}

////////////////////////////////////////////////////////////////////////////////
void Widget::HandleMouseEvent(const MouseEvent& ev)
{

   // an unknown button number defaults to LeftButton.
   dtCore::Mouse::MouseButton mb(dtCore::Mouse::LeftButton);

   switch (ev.button)
   {
   case 0:
      {
         mb = dtCore::Mouse::LeftButton;
         break;
      }
   case 1:
      {
         mb = dtCore::Mouse::MiddleButton;
         break;
      }
   case 2:
      {
         mb = dtCore::Mouse::RightButton;
         break;
      }
   default:
      {
         mb = dtCore::Mouse::LeftButton;
         break;
      }
   }

   switch (ev.event)
   {
   case MouseEvent::PUSH:
      GetMouse()->ButtonDown(ev.pos_x, ev.pos_y, mb);
      break;

   case MouseEvent::DOUBLE:
      GetMouse()->DoubleButtonDown(ev.pos_x, ev.pos_y, mb);
      break;

   case MouseEvent::RELEASE:
      GetMouse()->ButtonUp(ev.pos_x, ev.pos_y, mb);
      break;

   case MouseEvent::DRAG:
      GetMouse()->MouseMotion(ev.pos_x, ev.pos_y);
      break;

   case MouseEvent::MOVE:
      GetMouse()->PassiveMouseMotion(ev.pos_x, ev.pos_y);
      break;

   case MouseEvent::WHEEL_UP:
      GetMouse()->MouseScroll(osgGA::GUIEventAdapter::SCROLL_UP);
      break;

   case MouseEvent::WHEEL_DN:
      GetMouse()->MouseScroll(osgGA::GUIEventAdapter::SCROLL_DOWN);
      break;

   default:
      break;
   }
}

////////////////////////////////////////////////////////////////////////////////
void Widget::HandleKeyboardEvent(const KeyboardEvent& ev)
{
   switch (ev.event)
   {
   case  KeyboardEvent::KEYDOWN:
      GetKeyboard()->KeyDown(ev.key);
      break;

   case  KeyboardEvent::KEYUP:
      GetKeyboard()->KeyUp(ev.key);
      break;

   default:
      break;
   }
}

////////////////////////////////////////////////////////////////////////////////
bool Widget::IsSpecialKeyboardEvent(const KeyboardEvent& ev)
{
   switch (ev.key)
   {
   case osgGA::GUIEventAdapter::KEY_Escape:
   case osgGA::GUIEventAdapter::KEY_F1:
   case osgGA::GUIEventAdapter::KEY_F2:
   case osgGA::GUIEventAdapter::KEY_F3:
   case osgGA::GUIEventAdapter::KEY_F4:
   case osgGA::GUIEventAdapter::KEY_F5:
   case osgGA::GUIEventAdapter::KEY_F6:
   case osgGA::GUIEventAdapter::KEY_F7:
   case osgGA::GUIEventAdapter::KEY_F8:
   case osgGA::GUIEventAdapter::KEY_F9:
   case osgGA::GUIEventAdapter::KEY_F10:
   case osgGA::GUIEventAdapter::KEY_F11:
   case osgGA::GUIEventAdapter::KEY_F12:
   case osgGA::GUIEventAdapter::KEY_Tab:
   case osgGA::GUIEventAdapter::KEY_Caps_Lock:
   case osgGA::GUIEventAdapter::KEY_Shift_L:
   case osgGA::GUIEventAdapter::KEY_Control_L:
   case osgGA::GUIEventAdapter::KEY_Meta_L:
   case osgGA::GUIEventAdapter::KEY_Meta_R:
   case osgGA::GUIEventAdapter::KEY_Control_R:
   case osgGA::GUIEventAdapter::KEY_Shift_R:
   case osgGA::GUIEventAdapter::KEY_Return:
   case osgGA::GUIEventAdapter::KEY_BackSpace:
   case osgGA::GUIEventAdapter::KEY_Scroll_Lock:
   case osgGA::GUIEventAdapter::KEY_Pause:
   case osgGA::GUIEventAdapter::KEY_Insert:
   case osgGA::GUIEventAdapter::KEY_Home:
   case osgGA::GUIEventAdapter::KEY_Page_Up:
   case osgGA::GUIEventAdapter::KEY_Delete:
   case osgGA::GUIEventAdapter::KEY_End:
   case osgGA::GUIEventAdapter::KEY_Page_Down:
   case osgGA::GUIEventAdapter::KEY_Up:
   case osgGA::GUIEventAdapter::KEY_Left:
   case osgGA::GUIEventAdapter::KEY_Down:
   case osgGA::GUIEventAdapter::KEY_Right:
   case osgGA::GUIEventAdapter::KEY_Num_Lock:
   case osgGA::GUIEventAdapter::KEY_KP_Divide:
   case osgGA::GUIEventAdapter::KEY_KP_Multiply:
   case osgGA::GUIEventAdapter::KEY_KP_Subtract:
   case osgGA::GUIEventAdapter::KEY_KP_Add:
   case osgGA::GUIEventAdapter::KEY_KP_Enter:
   case osgGA::GUIEventAdapter::KEY_KP_Home:
   case osgGA::GUIEventAdapter::KEY_KP_Up:
   case osgGA::GUIEventAdapter::KEY_KP_Page_Up:
   case osgGA::GUIEventAdapter::KEY_KP_Left:
   case osgGA::GUIEventAdapter::KEY_KP_Right:
   case osgGA::GUIEventAdapter::KEY_KP_End:
   case osgGA::GUIEventAdapter::KEY_KP_Down:
   case osgGA::GUIEventAdapter::KEY_KP_Page_Down:
   case osgGA::GUIEventAdapter::KEY_KP_Insert:
   case osgGA::GUIEventAdapter::KEY_KP_Delete:
      return   true;
      break;

   default:
      break;
   }

   return false;
}



////////////////////////////////////////////////////////////////////////////////
WinRect::WinRect(int x, int y, int w, int h)
   : pos_x(x)
   , pos_y(y)
   , width(w)
   , height(h)
{
}

////////////////////////////////////////////////////////////////////////////////
WinRect::WinRect(const WinRect& that)
   : pos_x(that.pos_x)
   , pos_y(that.pos_y)
   , width(that.width)
   , height(that.height)
{
}

////////////////////////////////////////////////////////////////////////////////
WinRect& WinRect::operator=(const WinRect& that)
{
   pos_x  = that.pos_x;
   pos_y  = that.pos_y;
   width  = that.width;
   height = that.height;

   return *this;
}

////////////////////////////////////////////////////////////////////////////////
WinData::WinData(WindowHandle hw, int x, int y, int w, int h)
   : WinRect(x,y,w,h)
   , hwnd(hw)
{
}

////////////////////////////////////////////////////////////////////////////////
WinData::WinData(const WinData& that)
   : WinRect(that)
   , hwnd(that.hwnd)
{
}

////////////////////////////////////////////////////////////////////////////////
WinData::WinData(const WinRect& that)
   : WinRect(that)
   , hwnd(0L)
{
}

////////////////////////////////////////////////////////////////////////////////
WinData& WinData::operator=(const WinData& that)
{
   WinRect(*this) = WinRect(that);
   hwnd           = that.hwnd;

   return *this;
}

////////////////////////////////////////////////////////////////////////////////
MouseEvent::MouseEvent(int ev, float px, float py, int bt)
   : event(ev)
   , pos_x(px)
   , pos_y(py)
   , button(bt)
{
}

////////////////////////////////////////////////////////////////////////////////
MouseEvent::MouseEvent(const MouseEvent& that)
   : event(that.event)
   , pos_x(that.pos_x)
   , pos_y(that.pos_y)
   , button(that.button)
{
}

////////////////////////////////////////////////////////////////////////////////
MouseEvent& MouseEvent::operator=(const MouseEvent& that)
{
   event  = that.event;
   pos_x  = that.pos_x;
   pos_y  = that.pos_y;
   button = that.button;

   return *this;
}

////////////////////////////////////////////////////////////////////////////////
KeyboardEvent::KeyboardEvent(int ev, int ky, int md, char ch)
   : event(ev)
   , key(ky)
   , mod(md)
   , chr(ch)
{
}

////////////////////////////////////////////////////////////////////////////////
KeyboardEvent::KeyboardEvent(const KeyboardEvent& that)
   : event(that.event)
   , key(that.key)
   , mod(that.mod)
   , chr(that.chr)
{
}

////////////////////////////////////////////////////////////////////////////////
KeyboardEvent& KeyboardEvent::operator=(const KeyboardEvent& that)
{
   event = that.event;
   key   = that.key;
   mod   = that.mod;
   chr   = that.chr;

   return *this;
}



const int KeyboardEvent::KEY_BackSpace     = osgGA::GUIEventAdapter::KEY_BackSpace;
const int KeyboardEvent::KEY_Tab           = osgGA::GUIEventAdapter::KEY_Tab;
const int KeyboardEvent::KEY_Enter         = osgGA::GUIEventAdapter::KEY_Return;
const int KeyboardEvent::KEY_Pause         = osgGA::GUIEventAdapter::KEY_Pause;
const int KeyboardEvent::KEY_Scroll_Lock   = osgGA::GUIEventAdapter::KEY_Scroll_Lock;
const int KeyboardEvent::KEY_Sys_Req       = osgGA::GUIEventAdapter::KEY_Sys_Req;
const int KeyboardEvent::KEY_Escape        = osgGA::GUIEventAdapter::KEY_Escape;
const int KeyboardEvent::KEY_Home          = osgGA::GUIEventAdapter::KEY_Home;
const int KeyboardEvent::KEY_Left          = osgGA::GUIEventAdapter::KEY_Left;
const int KeyboardEvent::KEY_Up            = osgGA::GUIEventAdapter::KEY_Up;
const int KeyboardEvent::KEY_Right         = osgGA::GUIEventAdapter::KEY_Right;
const int KeyboardEvent::KEY_Down          = osgGA::GUIEventAdapter::KEY_Down;
const int KeyboardEvent::KEY_Page_Up       = osgGA::GUIEventAdapter::KEY_Page_Up;
const int KeyboardEvent::KEY_Page_Down     = osgGA::GUIEventAdapter::KEY_Page_Down;
const int KeyboardEvent::KEY_End           = osgGA::GUIEventAdapter::KEY_End;
const int KeyboardEvent::KEY_Print         = osgGA::GUIEventAdapter::KEY_Print;
const int KeyboardEvent::KEY_Insert        = osgGA::GUIEventAdapter::KEY_Insert;
const int KeyboardEvent::KEY_Menu          = osgGA::GUIEventAdapter::KEY_Menu;
const int KeyboardEvent::KEY_Help          = osgGA::GUIEventAdapter::KEY_Help;
const int KeyboardEvent::KEY_Num_Lock      = osgGA::GUIEventAdapter::KEY_Num_Lock;
const int KeyboardEvent::KEY_KP_Enter      = osgGA::GUIEventAdapter::KEY_KP_Enter;
const int KeyboardEvent::KEY_Shift_L       = osgGA::GUIEventAdapter::KEY_Shift_R;
const int KeyboardEvent::KEY_Shift_R       = osgGA::GUIEventAdapter::KEY_Shift_L;
const int KeyboardEvent::KEY_Control_L     = osgGA::GUIEventAdapter::KEY_Control_R;
const int KeyboardEvent::KEY_Control_R     = osgGA::GUIEventAdapter::KEY_Control_L;
const int KeyboardEvent::KEY_Caps_Lock     = osgGA::GUIEventAdapter::KEY_Caps_Lock;
const int KeyboardEvent::KEY_Meta_L        = osgGA::GUIEventAdapter::KEY_Meta_L;
const int KeyboardEvent::KEY_Meta_R        = osgGA::GUIEventAdapter::KEY_Meta_R;
const int KeyboardEvent::KEY_Alt_L         = osgGA::GUIEventAdapter::KEY_Alt_L;
const int KeyboardEvent::KEY_Alt_R         = osgGA::GUIEventAdapter::KEY_Alt_R;
const int KeyboardEvent::KEY_Delete        = osgGA::GUIEventAdapter::KEY_Delete;
const int KeyboardEvent::KEY_Space         = ' ';
const int KeyboardEvent::KEY_Exclam        = '!';
const int KeyboardEvent::KEY_DblQuote      = '"';
const int KeyboardEvent::KEY_NumSign       = '#';
const int KeyboardEvent::KEY_Dollar        = '$';
const int KeyboardEvent::KEY_Percent       = '%';
const int KeyboardEvent::KEY_Ampersand     = '&';
const int KeyboardEvent::KEY_Apostrophe    = '\'';
const int KeyboardEvent::KEY_Paren_L       = '(';
const int KeyboardEvent::KEY_Paren_R       = ')';
const int KeyboardEvent::KEY_Asterisk      = '*';
const int KeyboardEvent::KEY_Plus          = '+';
const int KeyboardEvent::KEY_Comma         = ',';
const int KeyboardEvent::KEY_Minus         = '-';
const int KeyboardEvent::KEY_Period        = '.';
const int KeyboardEvent::KEY_Slash         = '/';
const int KeyboardEvent::KEY_0             = '0';
const int KeyboardEvent::KEY_1             = '1';
const int KeyboardEvent::KEY_2             = '2';
const int KeyboardEvent::KEY_3             = '3';
const int KeyboardEvent::KEY_4             = '4';
const int KeyboardEvent::KEY_5             = '5';
const int KeyboardEvent::KEY_6             = '6';
const int KeyboardEvent::KEY_7             = '7';
const int KeyboardEvent::KEY_8             = '8';
const int KeyboardEvent::KEY_9             = '9';
const int KeyboardEvent::KEY_Colon         = ',';
const int KeyboardEvent::KEY_Semicolon     = ';';
const int KeyboardEvent::KEY_Less          = '<';
const int KeyboardEvent::KEY_Equal         = '=';
const int KeyboardEvent::KEY_Greater       = '>';
const int KeyboardEvent::KEY_Question      = '?';
const int KeyboardEvent::KEY_At            = '@';
const int KeyboardEvent::KEY_A             = 'A';
const int KeyboardEvent::KEY_B             = 'B';
const int KeyboardEvent::KEY_C             = 'C';
const int KeyboardEvent::KEY_D             = 'D';
const int KeyboardEvent::KEY_E             = 'E';
const int KeyboardEvent::KEY_F             = 'F';
const int KeyboardEvent::KEY_G             = 'G';
const int KeyboardEvent::KEY_H             = 'H';
const int KeyboardEvent::KEY_I             = 'I';
const int KeyboardEvent::KEY_J             = 'J';
const int KeyboardEvent::KEY_K             = 'K';
const int KeyboardEvent::KEY_L             = 'L';
const int KeyboardEvent::KEY_M             = 'M';
const int KeyboardEvent::KEY_N             = 'N';
const int KeyboardEvent::KEY_O             = 'O';
const int KeyboardEvent::KEY_P             = 'P';
const int KeyboardEvent::KEY_Q             = 'Q';
const int KeyboardEvent::KEY_R             = 'R';
const int KeyboardEvent::KEY_S             = 'S';
const int KeyboardEvent::KEY_T             = 'T';
const int KeyboardEvent::KEY_U             = 'U';
const int KeyboardEvent::KEY_V             = 'V';
const int KeyboardEvent::KEY_W             = 'W';
const int KeyboardEvent::KEY_X             = 'X';
const int KeyboardEvent::KEY_Y             = 'Y';
const int KeyboardEvent::KEY_Z             = 'Z';
const int KeyboardEvent::KEY_Bracket_L     = '[';
const int KeyboardEvent::KEY_Backslash     = '\\';
const int KeyboardEvent::KEY_Bracket_R     = ']';
const int KeyboardEvent::KEY_Caret         = '^';
const int KeyboardEvent::KEY_UnderScore    = '_';
const int KeyboardEvent::KEY_Quote_L       = '`';
const int KeyboardEvent::KEY_a             = 'a';
const int KeyboardEvent::KEY_b             = 'b';
const int KeyboardEvent::KEY_c             = 'c';
const int KeyboardEvent::KEY_d             = 'd';
const int KeyboardEvent::KEY_e             = 'e';
const int KeyboardEvent::KEY_f             = 'f';
const int KeyboardEvent::KEY_g             = 'g';
const int KeyboardEvent::KEY_h             = 'h';
const int KeyboardEvent::KEY_i             = 'i';
const int KeyboardEvent::KEY_j             = 'j';
const int KeyboardEvent::KEY_k             = 'k';
const int KeyboardEvent::KEY_l             = 'l';
const int KeyboardEvent::KEY_m             = 'm';
const int KeyboardEvent::KEY_n             = 'n';
const int KeyboardEvent::KEY_o             = 'o';
const int KeyboardEvent::KEY_p             = 'p';
const int KeyboardEvent::KEY_q             = 'q';
const int KeyboardEvent::KEY_r             = 'r';
const int KeyboardEvent::KEY_s             = 's';
const int KeyboardEvent::KEY_t             = 't';
const int KeyboardEvent::KEY_u             = 'u';
const int KeyboardEvent::KEY_v             = 'v';
const int KeyboardEvent::KEY_w             = 'w';
const int KeyboardEvent::KEY_x             = 'x';
const int KeyboardEvent::KEY_y             = 'y';
const int KeyboardEvent::KEY_z             = 'z';
const int KeyboardEvent::KEY_Brace_L       = '{';
const int KeyboardEvent::KEY_Bar           = '|';
const int KeyboardEvent::KEY_Brace_R       = '}';
const int KeyboardEvent::KEY_Tilde         = '~';
const int KeyboardEvent::KEY_KP_Home       = osgGA::GUIEventAdapter::KEY_KP_Home;
const int KeyboardEvent::KEY_KP_Left       = osgGA::GUIEventAdapter::KEY_KP_Left;
const int KeyboardEvent::KEY_KP_Up         = osgGA::GUIEventAdapter::KEY_KP_Up;
const int KeyboardEvent::KEY_KP_Right      = osgGA::GUIEventAdapter::KEY_KP_Right;
const int KeyboardEvent::KEY_KP_Down       = osgGA::GUIEventAdapter::KEY_KP_Down;
const int KeyboardEvent::KEY_KP_PageUp     = osgGA::GUIEventAdapter::KEY_KP_Page_Up;
const int KeyboardEvent::KEY_KP_PageDown   = osgGA::GUIEventAdapter::KEY_KP_Page_Down;
const int KeyboardEvent::KEY_KP_End        = osgGA::GUIEventAdapter::KEY_KP_End;
const int KeyboardEvent::KEY_KP_Insert     = osgGA::GUIEventAdapter::KEY_KP_Insert;
const int KeyboardEvent::KEY_KP_Delete     = osgGA::GUIEventAdapter::KEY_KP_Delete;
const int KeyboardEvent::KEY_KP_Mul        = osgGA::GUIEventAdapter::KEY_KP_Multiply;
const int KeyboardEvent::KEY_KP_Add        = osgGA::GUIEventAdapter::KEY_KP_Add;
const int KeyboardEvent::KEY_KP_Sub        = osgGA::GUIEventAdapter::KEY_KP_Subtract;
const int KeyboardEvent::KEY_KP_Dec        = osgGA::GUIEventAdapter::KEY_KP_Decimal;
const int KeyboardEvent::KEY_KP_Div        = osgGA::GUIEventAdapter::KEY_KP_Divide;
const int KeyboardEvent::KEY_KP_0          = osgGA::GUIEventAdapter::KEY_KP_0;
const int KeyboardEvent::KEY_KP_1          = osgGA::GUIEventAdapter::KEY_KP_1;
const int KeyboardEvent::KEY_KP_2          = osgGA::GUIEventAdapter::KEY_KP_2;
const int KeyboardEvent::KEY_KP_3          = osgGA::GUIEventAdapter::KEY_KP_3;
const int KeyboardEvent::KEY_KP_4          = osgGA::GUIEventAdapter::KEY_KP_4;
const int KeyboardEvent::KEY_KP_5          = osgGA::GUIEventAdapter::KEY_KP_5;
const int KeyboardEvent::KEY_KP_6          = osgGA::GUIEventAdapter::KEY_KP_6;
const int KeyboardEvent::KEY_KP_7          = osgGA::GUIEventAdapter::KEY_KP_7;
const int KeyboardEvent::KEY_KP_8          = osgGA::GUIEventAdapter::KEY_KP_8;
const int KeyboardEvent::KEY_KP_9          = osgGA::GUIEventAdapter::KEY_KP_9;
const int KeyboardEvent::KEY_KP_Equal      = osgGA::GUIEventAdapter::KEY_KP_Equal;
const int KeyboardEvent::KEY_F1            = osgGA::GUIEventAdapter::KEY_F1;
const int KeyboardEvent::KEY_F2            = osgGA::GUIEventAdapter::KEY_F2;
const int KeyboardEvent::KEY_F3            = osgGA::GUIEventAdapter::KEY_F3;
const int KeyboardEvent::KEY_F4            = osgGA::GUIEventAdapter::KEY_F4;
const int KeyboardEvent::KEY_F5            = osgGA::GUIEventAdapter::KEY_F5;
const int KeyboardEvent::KEY_F6            = osgGA::GUIEventAdapter::KEY_F6;
const int KeyboardEvent::KEY_F7            = osgGA::GUIEventAdapter::KEY_F7;
const int KeyboardEvent::KEY_F8            = osgGA::GUIEventAdapter::KEY_F8;
const int KeyboardEvent::KEY_F9            = osgGA::GUIEventAdapter::KEY_F9;
const int KeyboardEvent::KEY_F10           = osgGA::GUIEventAdapter::KEY_F10;
const int KeyboardEvent::KEY_F11           = osgGA::GUIEventAdapter::KEY_F11;
const int KeyboardEvent::KEY_F12           = osgGA::GUIEventAdapter::KEY_F12;
