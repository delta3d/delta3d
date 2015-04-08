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

#ifndef DELTA_WIDGET
#define DELTA_WIDGET

////////////////////////////////////////////////////////////////////////////////

#include <dtABC/baseabc.h>
#include <osgViewer/GraphicsWindow>

#include <dtUtil/mswin.h>


#if defined(WIN32) && !defined(__CYGWIN__)
#include <osgViewer/api/Win32/GraphicsWindowWin32>
typedef HWND WindowHandle;
typedef osgViewer::GraphicsWindowWin32::WindowData WindowData;
#elif defined(__APPLE__)
// passing in a window handle doesn't work on OSX.
typedef void* WindowHandle;
typedef osg::Referenced WindowData;
#else // all other unix
#include <osgViewer/api/X11/GraphicsWindowX11>
typedef Window WindowHandle;
typedef osgViewer::GraphicsWindowX11::WindowData WindowData;
#endif


#if !defined(BIT)
   ///BIT  helper definition for enumerated values and bit packing
   #define  BIT(a)   (1<<a)
#endif

///@cond IGNORE
namespace osgViewer
{
   class CompositeViewer;
}///@endcond

////////////////////////////////////////////////////////////////////////////////

namespace dtABC
{
   // forward references
   struct   WinRect;
   struct   WinData;
   struct   MouseEvent;
   struct   KeyboardEvent;

   ///Widget  class to handle all dtCore rendering for the application

   /** The Widget class is a dtCore::Base class object which receives
    *  window events from it's derived classes and performs all dtCore
    *  rendering.  It communicats with it's derived class strictly
    *  through the sig-slot mechanisms.
    *
    *  The Widget class, in essence, replaces the dtABC::Application
    *  class, allowing single frame cycle steps instead of a run-loop.
    *  Every time Widget::Step() is called, another frame cycle advances.
    *  This allows the frame cycles to be embedded into a windowing system's
    *  idle event handling loop.
    */
   class DT_ABC_EXPORT Widget : public dtABC::BaseABC
   {
      DECLARE_MANAGEMENT_LAYER(Widget)

   public:
      ///Message strings (outgoing)
      static   const char* msgRedraw;
      static   const char* msgStopped;

      ///Message strings (incoming)
      static   const char* msgStep;
      static   const char* msgMouseEvent;
      static   const char* msgKeyboardEvent;
      static   const char* msgResize;
      static   const char* msgAddDrawable;
      static   const char* msgSetPath;
      static   const char* msgWindowData;
      static   const char* msgQuit;


   public:
      Widget(const std::string& name = "Widget");

   protected:

      virtual ~Widget();

   public:

      /**
       * configure the internal components and
       * set system to render in a given window.
       *
       * @param data the window handle and size
       */
      virtual void Config(const WinData* data = NULL);

      ///Quit the application (call's system quit)
      virtual void Quit(void);

      ///Set the data path to use
      virtual void SetPath(const std::string& path);

   protected:
      ///Override for PreFrame
      virtual void PreFrame(const double deltaFrameTime);

      ///Override for Frame
      virtual void Frame(const double deltaFrameTime);

      ///Override for PostFrame
      virtual void PostFrame(const double deltaFrameTime);

      ///Takes the place of PreFrame when game is paused.
      virtual void Pause(const double deltaRealTime);

      ///BaseABC override to receive messages.
      ///This method should be called from derived classes
      virtual void OnMessage(dtCore::Base::MessageData* data);

      bool mIsInitialized;  ///<have we been fully initialized yet

      /// @return the instance of the osgViewer::CompositeViewer
      const osgViewer::CompositeViewer* GetCompositeViewer() const { return mCompositeViewer.get(); }

      /// @return the instance of the osgViewer::CompositeViewer
      osgViewer::CompositeViewer* GetCompositeViewer() { return mCompositeViewer.get(); }


   private:
      ///Advance a single frame cycle.
      inline void Step(void);

      ///Handle a window resize event.
      inline void Resize(const dtABC::WinRect* r);

      ///Handle a mouse events.
      inline void HandleMouseEvent(const MouseEvent& ev);

      ///Handle a keyboard events.
      inline void HandleKeyboardEvent(const KeyboardEvent& ev);

      ///Determine if special keyboard event.
      inline bool IsSpecialKeyboardEvent(const KeyboardEvent& ev);

      dtCore::RefPtr<osgViewer::CompositeViewer> mCompositeViewer;
   };



   ///WinRect struct for passing window dimensions
   struct DT_ABC_EXPORT WinRect
   {
      int pos_x;  //>window x position
      int pos_y;  //>window y position
      int width;  //>window width
      int height; //>window height

      WinRect(int x = 0, int y = 0, int w = 640L, int h = 480L);
      WinRect(const WinRect& that);
      WinRect& operator=(const WinRect& that);
   };



   ///WinData struct for passing window handle and dimensions
   struct DT_ABC_EXPORT WinData : public WinRect
   {
      WindowHandle hwnd;  ///window handle

      WinData(WindowHandle hw = 0L, int x = 0L, int y = 0L, int w = 640L, int h = 480L); //TODO
      WinData(const WinData& that);
      WinData(const WinRect& that);
      WinData& operator=(const WinData& that);
   };



   ///MouseEvent struct for passing mouse events
   struct DT_ABC_EXPORT MouseEvent
   {
      int   event;  //>mouse event
      float pos_x;  //>x coordinate
      float pos_y;  //>y coordinate
      int   button; //>event button

      MouseEvent(int ev = NO_EVENT, float px = 0.0f, float py = 0.0f, int bt = 0L);
      MouseEvent(const MouseEvent& that);
      MouseEvent& operator=(const MouseEvent& that);

      enum  Type
      {
         NO_EVENT = 0,
         PUSH,
         DOUBLE,
         RELEASE,
         ENTER,
         LEAVE,
         DRAG,
         MOVE,
         WHEEL_UP,
         WHEEL_DN,
         DND_ENTER,
         DND_DRAG,
         DND_LEAVE,
         DND_RELEASE
      };
   };



   ///KeyboardEvent struct for passing keyboard events
   struct DT_ABC_EXPORT KeyboardEvent
   {
      int   event; //>keyboard event
      int   key;   //>event key
      int   mod;   //>key modifiers
      char  chr;   //>ascii char of key

      KeyboardEvent(int ev = NO_EVENT, int ky = 0L, int mod = 0L, char ch = 0);
      KeyboardEvent(const KeyboardEvent& that);
      KeyboardEvent& operator=(const KeyboardEvent& that);

      enum  Type
      {
         NO_EVENT = 0L,
         FOCUS,
         UNFOCUS,
         KEYDOWN,
         KEYUP
      };

      enum  Modifier
      {
         SHIFT       = BIT(0L),
         CAPLOCK     = BIT(1L),
         CONTROL     = BIT(2L),
         ALT         = BIT(4L),
         NUMLOCK     = BIT(3L),
         META        = BIT(5L),
         SCROLLLOCK  = BIT(6L)
      };

#if !defined(_WIN32) && !defined(WIN32) && !defined(__WIN32__)
      #undef KEY_0
      #undef KEY_1
      #undef KEY_2
      #undef KEY_3
      #undef KEY_4
      #undef KEY_5
      #undef KEY_6
      #undef KEY_7
      #undef KEY_8
      #undef KEY_9
      #undef KEY_A
      #undef KEY_B
      #undef KEY_C
      #undef KEY_D
      #undef KEY_E
      #undef KEY_F
      #undef KEY_G
      #undef KEY_H
      #undef KEY_I
      #undef KEY_J
      #undef KEY_K
      #undef KEY_L
      #undef KEY_M
      #undef KEY_N
      #undef KEY_O
      #undef KEY_P
      #undef KEY_Q
      #undef KEY_R
      #undef KEY_S
      #undef KEY_T
      #undef KEY_U
      #undef KEY_V
      #undef KEY_W
      #undef KEY_X
      #undef KEY_Y
      #undef KEY_Z
      #undef KEY_F1
      #undef KEY_F2
      #undef KEY_F3
      #undef KEY_F4
      #undef KEY_F5
      #undef KEY_F6
      #undef KEY_F7
      #undef KEY_F8
      #undef KEY_F9
      #undef KEY_F10
      #undef KEY_F11
      #undef KEY_F12
#endif


      static const int KEY_BackSpace;
      static const int KEY_Tab;
      static const int KEY_Enter;
      static const int KEY_Pause;
      static const int KEY_Scroll_Lock;
      static const int KEY_Sys_Req;
      static const int KEY_Escape;
      static const int KEY_Home;
      static const int KEY_Left;
      static const int KEY_Up;
      static const int KEY_Right;
      static const int KEY_Down;
      static const int KEY_Page_Up;
      static const int KEY_Page_Down;
      static const int KEY_End;
      static const int KEY_Print;
      static const int KEY_Insert;
      static const int KEY_Menu;
      static const int KEY_Help;
      static const int KEY_Num_Lock;
      static const int KEY_KP_Enter;
      static const int KEY_Shift_L;
      static const int KEY_Shift_R;
      static const int KEY_Control_L;
      static const int KEY_Control_R;
      static const int KEY_Caps_Lock;
      static const int KEY_Meta_L;
      static const int KEY_Meta_R;
      static const int KEY_Alt_L;
      static const int KEY_Alt_R;
      static const int KEY_Delete;
      static const int KEY_Space;
      static const int KEY_Exclam;
      static const int KEY_DblQuote;
      static const int KEY_NumSign;
      static const int KEY_Dollar;
      static const int KEY_Percent;
      static const int KEY_Ampersand;
      static const int KEY_Apostrophe;
      static const int KEY_Paren_L;
      static const int KEY_Paren_R;
      static const int KEY_Asterisk;
      static const int KEY_Plus;
      static const int KEY_Comma;
      static const int KEY_Minus;
      static const int KEY_Period;
      static const int KEY_Slash;
      static const int KEY_0;
      static const int KEY_1;
      static const int KEY_2;
      static const int KEY_3;
      static const int KEY_4;
      static const int KEY_5;
      static const int KEY_6;
      static const int KEY_7;
      static const int KEY_8;
      static const int KEY_9;
      static const int KEY_Colon;
      static const int KEY_Semicolon;
      static const int KEY_Less;
      static const int KEY_Equal;
      static const int KEY_Greater;
      static const int KEY_Question;
      static const int KEY_At;
      static const int KEY_A;
      static const int KEY_B;
      static const int KEY_C;
      static const int KEY_D;
      static const int KEY_E;
      static const int KEY_F;
      static const int KEY_G;
      static const int KEY_H;
      static const int KEY_I;
      static const int KEY_J;
      static const int KEY_K;
      static const int KEY_L;
      static const int KEY_M;
      static const int KEY_N;
      static const int KEY_O;
      static const int KEY_P;
      static const int KEY_Q;
      static const int KEY_R;
      static const int KEY_S;
      static const int KEY_T;
      static const int KEY_U;
      static const int KEY_V;
      static const int KEY_W;
      static const int KEY_X;
      static const int KEY_Y;
      static const int KEY_Z;
      static const int KEY_Bracket_L;
      static const int KEY_Backslash;
      static const int KEY_Bracket_R;
      static const int KEY_Caret;
      static const int KEY_UnderScore;
      static const int KEY_Quote_L;
      static const int KEY_a;
      static const int KEY_b;
      static const int KEY_c;
      static const int KEY_d;
      static const int KEY_e;
      static const int KEY_f;
      static const int KEY_g;
      static const int KEY_h;
      static const int KEY_i;
      static const int KEY_j;
      static const int KEY_k;
      static const int KEY_l;
      static const int KEY_m;
      static const int KEY_n;
      static const int KEY_o;
      static const int KEY_p;
      static const int KEY_q;
      static const int KEY_r;
      static const int KEY_s;
      static const int KEY_t;
      static const int KEY_u;
      static const int KEY_v;
      static const int KEY_w;
      static const int KEY_x;
      static const int KEY_y;
      static const int KEY_z;
      static const int KEY_Brace_L;
      static const int KEY_Bar;
      static const int KEY_Brace_R;
      static const int KEY_Tilde;
      static const int KEY_KP_Home;
      static const int KEY_KP_Left;
      static const int KEY_KP_Up;
      static const int KEY_KP_Right;
      static const int KEY_KP_Down;
      static const int KEY_KP_PageUp;
      static const int KEY_KP_PageDown;
      static const int KEY_KP_End;
      static const int KEY_KP_Insert;
      static const int KEY_KP_Delete;
      static const int KEY_KP_Mul;
      static const int KEY_KP_Add;
      static const int KEY_KP_Sub;
      static const int KEY_KP_Dec;
      static const int KEY_KP_Div;
      static const int KEY_KP_0;
      static const int KEY_KP_1;
      static const int KEY_KP_2;
      static const int KEY_KP_3;
      static const int KEY_KP_4;
      static const int KEY_KP_5;
      static const int KEY_KP_6;
      static const int KEY_KP_7;
      static const int KEY_KP_8;
      static const int KEY_KP_9;
      static const int KEY_KP_Equal;
      static const int KEY_F1;
      static const int KEY_F2;
      static const int KEY_F3;
      static const int KEY_F4;
      static const int KEY_F5;
      static const int KEY_F6;
      static const int KEY_F7;
      static const int KEY_F8;
      static const int KEY_F9;
      static const int KEY_F10;
      static const int KEY_F11;
      static const int KEY_F12;
   };

} // namespace dtABC

////////////////////////////////////////////////////////////////////////////////

#endif // DELTA_WIDGET
