#ifndef DELTA_WIDGET
#define DELTA_WIDGET

#include <FL/X.h>

#include <baseabc.h>



#if   !  defined(BIT)
   ///BIT  helper definition for enumerated values and bit packing
   #define  BIT(a)   (1<<a)
#endif



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
   class DT_EXPORT Widget :  public   dtABC::BaseABC
   {
      DECLARE_MANAGEMENT_LAYER(Widget)


   public:
      ///Message strings (out going)
      static   const char* msgRedraw;
      static   const char* msgStopped;

      ///Message strings (in comming)
      static   const char* msgStep;
      static   const char* msgMouseEvent;
      static   const char* msgKeyboardEvent;
      static   const char* msgResize;
      static   const char* msgAddDrawable;
      static   const char* msgSetPath;
      static   const char* msgWindowData;
      static   const char* msgQuit;


   public:
                              Widget( std::string name = "Widget" );
      virtual                 ~Widget();

      /**
       * configure the internal components and
       * set system to render in a given window.
       *
       * @param data the window handle and size
       */
      virtual  void           Config( const WinData* data = NULL );

      ///Quit the application (call's system quit)
      virtual  void           Quit( void );

      ///Set the data path to use
      virtual  void           SetPath( std::string path );

   protected:
      ///Override for PreFrame
      virtual  void           PreFrame( const double deltaFrameTime );

      ///Override for Frame
      virtual  void           Frame( const double deltaFrameTime );

      ///Override for PostFrame
      virtual  void           PostFrame( const double deltaFrameTime );

      ///BaseABC override to receive messages.
      ///This method should be called from derived classes
      virtual  void           OnMessage( dtCore::Base::MessageData* data );

   private:
      ///Advance a single frame cycle.
      inline   void           Step( void );

      ///Handle a window resize event.
      inline   void           Resize( const dtABC::WinRect* r );

      ///Handle a mouse events.
      inline   void           HandleMouseEvent( const MouseEvent& ev );

      ///Handle a keyboard events.
      inline   void           HandleKeyboardEvent( const KeyboardEvent& ev );

      ///Determin if special keyboard event.
      inline   bool           IsSpecialKeyboardEvent( const KeyboardEvent& ev );
   };



   ///WinRect struct for passing window dimensions
   struct   WinRect
   {
      int   pos_x;                  ///window x position
      int   pos_y;                  ///window y position
      int   width;                  ///window width
      int   height;                 ///window height

      WinRect( int x = 0, int y = 0, int w = 640L, int h = 480L );
      WinRect( const WinRect& that );
      WinRect& operator=( const WinRect& that );
   };



   ///WinData struct for passing window handle and dimensions
   struct   WinData   :  public   WinRect
   {
      HWND  hwnd;                   ///window handle

      WinData( HWND hw = 0L, int x = 0L, int y = 0L, int w = 640L, int h = 480L );
      WinData( const WinData& that );
      WinData( const WinRect& that );
      WinData& operator=( const WinData& that );
   };



   ///MouseEvent struct for passing mouse events
   struct   MouseEvent
   {
      int   event;                  ///mouse event
      float pos_x;                  ///x coordinate
      float pos_y;                  ///y coordinate
      int   button;                 ///event button

      MouseEvent( int ev = NO_EVENT, float px = 0.f, float py = 0.f, int bt = 0L );
      MouseEvent( const MouseEvent& that );
      MouseEvent& operator=( const MouseEvent& that );

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
               DND_RELEASE,
            };
   };



   ///KeyboardEvent struct for passing keyboard events
   struct   KeyboardEvent
   {
      int   event;                  ///keyboard event
      int   key;                    ///event key
      int   mod;                    ///key modifiers
      char  chr;                    ///ascii char of key

      KeyboardEvent( int ev = NO_EVENT, int ky = 0L, int mod = 0L, char ch = 0 );
      KeyboardEvent( const KeyboardEvent& that );
      KeyboardEvent& operator=( const KeyboardEvent& that );

      enum  Type
            {
               NO_EVENT = 0L,
               FOCUS,
               UNFOCUS,
               KEYDOWN,
               KEYUP,
            };

      enum  Modifier
            {
               SHIFT       = BIT(0L),
               CAPLOCK     = BIT(1L),
               CONTROL     = BIT(2L),
               ALT         = BIT(4L),
               NUMLOCK     = BIT(3L),
               META        = BIT(5L),
               SCROLLLOCK  = BIT(6L),
            };

      static   const int   KEY_BackSpace;
      static   const int   KEY_Tab;
      static   const int   KEY_Enter;
      static   const int   KEY_Pause;
      static   const int   KEY_Scroll_Lock;
      static   const int   KEY_Sys_Req;
      static   const int   KEY_Escape;
      static   const int   KEY_Home;
      static   const int   KEY_Left;
      static   const int   KEY_Up;
      static   const int   KEY_Right;
      static   const int   KEY_Down;
      static   const int   KEY_Page_Up;
      static   const int   KEY_Page_Down;
      static   const int   KEY_End;
      static   const int   KEY_Print;
      static   const int   KEY_Insert;
      static   const int   KEY_Menu;
      static   const int   KEY_Help;
      static   const int   KEY_Num_Lock;
      static   const int   KEY_KP_Enter;
      static   const int   KEY_Shift_L;
      static   const int   KEY_Shift_R;
      static   const int   KEY_Control_L;
      static   const int   KEY_Control_R;
      static   const int   KEY_Caps_Lock;
      static   const int   KEY_Meta_L;
      static   const int   KEY_Meta_R;
      static   const int   KEY_Alt_L;
      static   const int   KEY_Alt_R;
      static   const int   KEY_Delete;
      static   const int   KEY_Space;
      static   const int   KEY_Exclam;
      static   const int   KEY_DblQuote;
      static   const int   KEY_NumSign;
      static   const int   KEY_Dollar;
      static   const int   KEY_Percent;
      static   const int   KEY_Ampersand;
      static   const int   KEY_Apostrophe;
      static   const int   KEY_Paren_L;
      static   const int   KEY_Paren_R;
      static   const int   KEY_Asterisk;
      static   const int   KEY_Plus;
      static   const int   KEY_Comma;
      static   const int   KEY_Minus;
      static   const int   KEY_Period;
      static   const int   KEY_Slash;
      static   const int   KEY_0;
      static   const int   KEY_1;
      static   const int   KEY_2;
      static   const int   KEY_3;
      static   const int   KEY_4;
      static   const int   KEY_5;
      static   const int   KEY_6;
      static   const int   KEY_7;
      static   const int   KEY_8;
      static   const int   KEY_9;
      static   const int   KEY_Colon;
      static   const int   KEY_Semicolon;
      static   const int   KEY_Less;
      static   const int   KEY_Equal;
      static   const int   KEY_Greater;
      static   const int   KEY_Question;
      static   const int   KEY_At;
      static   const int   KEY_A;
      static   const int   KEY_B;
      static   const int   KEY_C;
      static   const int   KEY_D;
      static   const int   KEY_E;
      static   const int   KEY_F;
      static   const int   KEY_G;
      static   const int   KEY_H;
      static   const int   KEY_I;
      static   const int   KEY_J;
      static   const int   KEY_K;
      static   const int   KEY_L;
      static   const int   KEY_M;
      static   const int   KEY_N;
      static   const int   KEY_O;
      static   const int   KEY_P;
      static   const int   KEY_Q;
      static   const int   KEY_R;
      static   const int   KEY_S;
      static   const int   KEY_T;
      static   const int   KEY_U;
      static   const int   KEY_V;
      static   const int   KEY_W;
      static   const int   KEY_X;
      static   const int   KEY_Y;
      static   const int   KEY_Z;
      static   const int   KEY_Bracket_L;
      static   const int   KEY_Backslash;
      static   const int   KEY_Bracket_R;
      static   const int   KEY_Caret;
      static   const int   KEY_UnderScore;
      static   const int   KEY_Quote_L;
      static   const int   KEY_a;
      static   const int   KEY_b;
      static   const int   KEY_c;
      static   const int   KEY_d;
      static   const int   KEY_e;
      static   const int   KEY_f;
      static   const int   KEY_g;
      static   const int   KEY_h;
      static   const int   KEY_i;
      static   const int   KEY_j;
      static   const int   KEY_k;
      static   const int   KEY_l;
      static   const int   KEY_m;
      static   const int   KEY_n;
      static   const int   KEY_o;
      static   const int   KEY_p;
      static   const int   KEY_q;
      static   const int   KEY_r;
      static   const int   KEY_s;
      static   const int   KEY_t;
      static   const int   KEY_u;
      static   const int   KEY_v;
      static   const int   KEY_w;
      static   const int   KEY_x;
      static   const int   KEY_y;
      static   const int   KEY_z;
      static   const int   KEY_Brace_L;
      static   const int   KEY_Bar;
      static   const int   KEY_Brace_R;
      static   const int   KEY_Tilde;
      static   const int   KEY_KP_Home;
      static   const int   KEY_KP_Left;
      static   const int   KEY_KP_Up;
      static   const int   KEY_KP_Right;
      static   const int   KEY_KP_Down;
      static   const int   KEY_KP_PageUp;
      static   const int   KEY_KP_PageDown;
      static   const int   KEY_KP_End;
      static   const int   KEY_KP_Insert;
      static   const int   KEY_KP_Delete;
      static   const int   KEY_KP_Mul;
      static   const int   KEY_KP_Add;
      static   const int   KEY_KP_Sub;
      static   const int   KEY_KP_Dec;
      static   const int   KEY_KP_Div;
      static   const int   KEY_KP_0;
      static   const int   KEY_KP_1;
      static   const int   KEY_KP_2;
      static   const int   KEY_KP_3;
      static   const int   KEY_KP_4;
      static   const int   KEY_KP_5;
      static   const int   KEY_KP_6;
      static   const int   KEY_KP_7;
      static   const int   KEY_KP_8;
      static   const int   KEY_KP_9;
      static   const int   KEY_KP_Equal;
      static   const int   KEY_F1;
      static   const int   KEY_F2;
      static   const int   KEY_F3;
      static   const int   KEY_F4;
      static   const int   KEY_F5;
      static   const int   KEY_F6;
      static   const int   KEY_F7;
      static   const int   KEY_F8;
      static   const int   KEY_F9;
      static   const int   KEY_F10;
      static   const int   KEY_F11;
      static   const int   KEY_F12;
   };

};

#endif // DELTA_WIDGET
