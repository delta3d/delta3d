#ifndef DELTA_FL_DT_WINDOW
#define DELTA_FL_DT_WINDOW


#include <cassert>

#include <FL/Fl.h>
#include <FL/X.h>
#include <FL/Fl_Window.h>

#include "widget.h"



template<class T>
class Fl_DT_Window  :  public   T, public   Fl_Window
{
   typedef  Fl_Window   Parent;

public:
   /**
    * Default constructor.
    */
   Fl_DT_Window()
      :  T(),
         Parent(0L, 0L, 640L, 480L),
         mEvent(0L)
   {
      assert( IS_A(this, dtABC::Widget*) );
      ctor();
   }



   /**
    * Constructor.
    *
    * @param w window width (right)
    * @param h window height (bottom)
    * @param label window's label
    */
   Fl_DT_Window( int w, int h, const char* label = 0L )
      :  T(),
         Parent(0L, 0L, w, h, label),
         mEvent(0L)
   {
      assert( IS_A(this, dtABC::Widget*) );
      ctor();
   }



   /**
    * Constructors.
    *
    * @param x horizontal window position (left)
    * @param y vertical window position (top)
    * @param w window width (right)
    * @param h window height (bottom)
    * @param label window's label
    */
   Fl_DT_Window( int x, int y, int w, int h, const char* label = 0L )
      :  T(),
         Parent(x, y, w, h, label),
         mEvent(0L)
   {
      assert( IS_A(this, Widget*) );
      ctor();
   }



   /**
    * Destructor.
    */
   virtual
   ~Fl_DT_Window()
   {
   }



   /**
    * Fl_Window override to handle event's from FLTK.
    * 
    * @param event a single event from FLTK
    * 
    * @return non-zero on handled, zero on not handled
    */
   virtual  int
   handle( int event )
   {
      switch( event )
      {
         case  FL_SHOW:
            return   Parent::handle( FL_SHOW );
            break;

         case  FL_PUSH:
            if( mEvent & BIT(FL_PUSH) )
            {
               dtABC::MouseEvent ev(
                                       FL_PUSH,
                                       float(Fl::event_x()),
                                       float(Fl::event_y()),
                                       Fl::event_button()
                                    );

               TranslateMouseEvent( ev );
               SendMessage( msgMouseEvent, &ev );
               return   1;
            }
            break;

         case  FL_RELEASE:
            if( mEvent & BIT(FL_RELEASE) )
            {
               dtABC::MouseEvent ev(
                                       FL_RELEASE,
                                       float(Fl::event_x()),
                                       float(Fl::event_y()),
                                       Fl::event_button()
                                    );

               TranslateMouseEvent( ev );
               SendMessage( msgMouseEvent, &ev );
               return   1;
            }
            break;

         case  FL_ENTER:
            if( mEvent & BIT(FL_ENTER) )
            {
               dtABC::MouseEvent ev( FL_ENTER );

               TranslateMouseEvent( ev );
               SendMessage( msgMouseEvent, &ev );
               return   1;
            }
            break;

         case  FL_LEAVE:
            if( mEvent & BIT(FL_LEAVE) )
            {
               dtABC::MouseEvent ev( FL_LEAVE );

               TranslateMouseEvent( ev );
               SendMessage( msgMouseEvent, &ev );
               return   1;
            }
            break;

         case  FL_DRAG:
            if( mEvent & BIT(FL_DRAG) )
            {
               dtABC::MouseEvent ev(
                                       FL_DRAG,
                                       float(Fl::event_x()),
                                       float(Fl::event_y())
                                    );

               TranslateMouseEvent( ev );
               SendMessage( msgMouseEvent, &ev );
               return   1;
            }
            break;

         case  FL_MOVE:
            if( mEvent & BIT(FL_MOVE) )
            {
               dtABC::MouseEvent ev(
                                       FL_MOVE,
                                       float(Fl::event_x()),
                                       float(Fl::event_y())
                                    );

               TranslateMouseEvent( ev );
               SendMessage( msgMouseEvent, &ev );
               return   1;
            }
            break;

         case  FL_MOUSEWHEEL:
            if( mEvent & BIT(FL_MOUSEWHEEL) )
            {
               dtABC::MouseEvent   ev(
                                          FL_MOUSEWHEEL,
                                          0.0f,
                                          float(Fl::event_dy())
                                      );

               TranslateMouseEvent( ev );
               SendMessage( msgMouseEvent, &ev );
               return   1;
            }
            break;

         case FL_FOCUS:
            if( mEvent & BIT(FL_FOCUS) )
            {
               dtABC::KeyboardEvent   ev( FL_FOCUS );

               TranslateKeyboardEvent( ev );
               SendMessage( msgKeyboardEvent, &ev );
               return   1;
            }
            break;

         case FL_UNFOCUS:
            if( mEvent & BIT(FL_UNFOCUS) )
            {
               dtABC::KeyboardEvent   ev( FL_UNFOCUS );

               TranslateKeyboardEvent( ev );
               SendMessage( msgKeyboardEvent, &ev );
               return   1;
            }
            break;

         case FL_KEYDOWN:
            if( mEvent & BIT(FL_KEYDOWN) )
            {
               dtABC::KeyboardEvent   ev(
                                             FL_KEYDOWN,
                                             Fl::event_key(),
                                             Fl::event_state(),
                                             (Fl::event_text()!=0L)? *Fl::event_text(): '*'
                                         );

               TranslateKeyboardEvent( ev );
               SendMessage( msgKeyboardEvent, &ev );
               return   1;
            }
            break;

         case  FL_KEYUP:
            if( mEvent & BIT(FL_KEYUP) )
            {
               dtABC::KeyboardEvent   ev(
                                             FL_KEYUP,
                                             Fl::event_key(),
                                             Fl::event_state(),
                                             (Fl::event_text()!=0L)? *Fl::event_text(): '*'
                                         );

               TranslateKeyboardEvent( ev );
               SendMessage( msgKeyboardEvent, &ev );
               return   1;
            }
            break;

         case  FL_DND_ENTER:
            if( mEvent & BIT(FL_DND_ENTER) )
            {
               dtABC::MouseEvent ev( FL_DND_ENTER );

               TranslateMouseEvent( ev );
               SendMessage( msgMouseEvent, &ev );
               return   1;
            }
            break;

         case  FL_DND_LEAVE:
            if( mEvent & BIT(FL_DND_LEAVE) )
            {
               dtABC::MouseEvent ev( FL_DND_LEAVE );

               TranslateMouseEvent( ev );
               SendMessage( msgMouseEvent, &ev );
               return   1;
            }
            break;

         case  FL_DND_DRAG:
            if( mEvent & BIT(FL_DND_DRAG) )
            {
               dtABC::MouseEvent ev(
                                       FL_DND_DRAG,
                                       float(Fl::event_x()),
                                       float(Fl::event_y())
                                    );

               TranslateMouseEvent( ev );
               SendMessage( msgMouseEvent, &ev );
               return   1;
            }
            break;

         case  FL_DND_RELEASE:
            if( mEvent & BIT(FL_DND_RELEASE) )
            {
               dtABC::MouseEvent ev(
                                       FL_DND_RELEASE,
                                       float(Fl::event_x()),
                                       float(Fl::event_y()),
                                       Fl::event_button()
                                    );

               TranslateMouseEvent( ev );
               SendMessage( msgMouseEvent, &ev );
               return   1;
            }
            break;

         default:
            break;
      }

      return   0L;
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
   virtual  void
   resize( int x, int y, int w, int h )
   {
      Parent::resize( x, y, w, h );

      dtABC::WinRect rect(x, y, w, h);
      SendMessage( msgResize, &rect );
   }



   /**
    * Fl_Window::show() override to configure the widget
    * and prepare it for visibility on the screen.
    */
   virtual  void
   show( void )
   {
      Parent::show();

      dtABC::WinData  windata(  fl_xid(this),
                                 Parent::x(),
                                 Parent::y(),
                                 Parent::w(),
                                 Parent::h() );

      SendMessage( msgWindowData, &windata );
      Fl::add_idle( idle_callback, this );
   }



   /**
    * Set/unset which events get handled.
    *
    * @Param ev the event to set
    * @Param set flag to set or unset the event
    */
   virtual  void
   SetEvent( Fl_Event ev, bool set = true )
   {
      switch( ev )
      {
         case  FL_PUSH:
         case  FL_RELEASE:
         case  FL_ENTER:
         case  FL_LEAVE:
         case  FL_DRAG:
         case  FL_FOCUS:
         case  FL_UNFOCUS:
         case  FL_KEYDOWN:
         case  FL_KEYUP:
         case  FL_MOVE:
         case  FL_MOUSEWHEEL:
         case  FL_DND_ENTER:
         case  FL_DND_DRAG:
         case  FL_DND_LEAVE:
         case  FL_DND_RELEASE:
            if( set )
            {
               mEvent   |= BIT(ev);
            }
            else
            {
               mEvent   &= ~BIT(ev);
            }
            break;

         default:
            break;
      }
   }



protected:
   /**
    * Base::OnMessage override to receive messages.
    *
    * @param data the message to receive
    */
   virtual  void
   OnMessage( dtCore::Base::MessageData* data )
   {
      assert( data );

      if( data->message == msgStopped )
      {
         Parent::hide();
         Fl::first_window()->hide();
         return;
      }

      if( data->message == msgRedraw )
      {
         Parent::redraw();
         return;
      }

      T::OnMessage( data );
   }



   /**
    * Static callback, repeatedly called when FLTK is idle.
    * Function casts void pointer to this object's type
    * then calls the overloaded (non-static) callback.
    * 
    * @param data pointer to this object cast as void*
    */
   static   void
   idle_callback( void* data )
   {
      assert( data );
      static_cast<Fl_DT_Window*>(data)->idle_callback();
   }



   /**
    * Callback, repeatedly called when FLTK is idle.
    * Calls the Viewer's step function to advance one
    * frame of the 3D scene.
    */
   virtual  void
   idle_callback( void )
   {
      SendMessage( msgStep );
   }



   /**
    * Translate the mouse event from FLTK data to dtABC data.
    *
    * @Param ev the event to translate
    */
   virtual  void
   TranslateMouseEvent( dtABC::MouseEvent& ev )
   {
      switch( ev.event )
      {
         case  FL_PUSH:
            ev.event = ev.PUSH;
            break;

         case  FL_RELEASE:
            ev.event = ev.RELEASE;
            break;

         case  FL_ENTER:
            ev.event = ev.ENTER;
            return;
            break;

         case  FL_LEAVE:
            ev.event = ev.LEAVE;
            return;
            break;

         case  FL_DRAG:
            ev.event = ev.DRAG;
            break;

         case  FL_MOVE:
            ev.event = ev.MOVE;
            break;

         case  FL_MOUSEWHEEL:
            ev.event =  (Fl::event_dy()>0)?
                        ev.WHEEL_UP:
                        (
                           (Fl::event_dy()<0)?
                           ev.WHEEL_DN:
                           ev.NO_EVENT
                        );
            return;
            break;

         case  FL_DND_ENTER:
            ev.event = ev.DND_ENTER;
            return;
            break;

         case  FL_DND_DRAG:
            ev.event = ev.DND_DRAG;
            break;

         case  FL_DND_LEAVE:
            ev.event = ev.DND_LEAVE;
            return;
            break;

         case  FL_DND_RELEASE:
            ev.event = ev.DND_RELEASE;
            break;

         default:
            return;
            break;
      }

      ev.pos_x =  (ev.pos_x / ( float(Fl_Widget::w() - 1L) * 0.5f )) - 1.f;
      ev.pos_y = ((ev.pos_y / ( float(Fl_Widget::h() - 1L) * 0.5f )) - 1.f ) * -1.f;
   }



   /**
    * Translate the mouse event from FLTK data to dtCore data.
    *
    * @Param ev the event to translate
    */
   virtual  void
   TranslateKeyboardEvent( dtABC::KeyboardEvent& ev )
   {
      const int   shf(ev.SHIFT|ev.CAPLOCK);
      const int   mod(ev.mod);
      ev.mod   = 0L;


      switch( ev.event )
      {
         case  FL_FOCUS:
            ev.event = ev.FOCUS;
            return;
            break;

         case  FL_UNFOCUS:
            ev.event = ev.UNFOCUS;
            return;
            break;

         case  FL_KEYDOWN:
            ev.event = ev.KEYDOWN;
            break;

         case  FL_KEYUP:
            ev.event = ev.KEYUP;
            break;

         default:
            return;
            break;
      }

      if( mod & FL_SHIFT )
         ev.mod   |= ev.SHIFT;

      if( mod & FL_CAPS_LOCK )
         ev.mod   |= ev.CAPLOCK;

      if( mod & FL_CTRL )
         ev.mod   |= ev.CONTROL;

      if( mod & FL_ALT )
         ev.mod   |= ev.ALT;

      if( mod & FL_NUM_LOCK )
         ev.mod   |= ev.NUMLOCK;

      if( mod & FL_META )
         ev.mod   |= ev.META;

      if( mod & FL_SCROLL_LOCK )
         ev.mod   |= ev.SCROLLLOCK;


      switch( ev.key )
      {
         case  FL_BackSpace:
            ev.key   = ev.KEY_BackSpace;
            break;

         case  FL_Tab:
            ev.key   = ev.KEY_Tab;
            break;

         case  FL_Enter:
            ev.key   = ev.KEY_Enter;
            break;

         case  FL_Pause:
            ev.key   = ev.KEY_Pause;
            break;

         case  FL_Scroll_Lock:
            ev.key   = ev.KEY_Scroll_Lock;
            break;

         case  FL_Escape:
            ev.key   = ev.KEY_Escape;
            break;

         case  FL_Home:
            ev.key   = ev.KEY_Home;
            break;

         case  FL_Left:
            ev.key   = ev.KEY_Left;
            break;

         case  FL_Up:
            ev.key   = ev.KEY_Up;
            break;

         case  FL_Right:
            ev.key   = ev.KEY_Right;
            break;

         case  FL_Down:
            ev.key   = ev.KEY_Down;
            break;

         case  FL_Page_Up:
            ev.key   = ev.KEY_Page_Up;
            break;

         case  FL_Page_Down:
            ev.key   = ev.KEY_Page_Down;
            break;

         case  FL_End:
            ev.key   = ev.KEY_End;
            break;

         case  FL_Print:
            ev.key   = ev.KEY_Print;
            break;

         case  FL_Insert:
            ev.key   = ev.KEY_Insert;
            break;

         case  FL_Menu:
            ev.key   = ev.KEY_Menu;
            break;

         case  FL_Help:
            ev.key   = ev.KEY_Help;
            break;

         case  FL_Num_Lock:
            ev.key   = ev.KEY_Num_Lock;
            break;

         case  FL_KP + '9':
            if( ev.mod & ev.NUMLOCK )
               ev.key   = ev.KEY_KP_9;
            else
               ev.key   = ev.KEY_KP_PageUp;
            break;

         case  FL_KP + '8':
            if( ev.mod & ev.NUMLOCK )
               ev.key   = ev.KEY_KP_8;
            else
               ev.key   = ev.KEY_KP_Up;
            break;

         case  FL_KP + '7':
            if( ev.mod & ev.NUMLOCK )
               ev.key   = ev.KEY_KP_7;
            else
               ev.key   = ev.KEY_KP_Home;
            break;

         case  FL_KP + '6':
            if( ev.mod & ev.NUMLOCK )
               ev.key   = ev.KEY_KP_6;
            else
               ev.key   = ev.KEY_KP_Right;
            break;

         case  FL_KP + '5':
            ev.key   = ev.KEY_KP_5;
            break;

         case  FL_KP + '4':
            if( ev.mod & ev.NUMLOCK )
               ev.key   = ev.KEY_KP_4;
            else
               ev.key   = ev.KEY_KP_Left;
            break;

         case  FL_KP + '3':
            if( ev.mod & ev.NUMLOCK )
               ev.key   = ev.KEY_KP_3;
            else
               ev.key   = ev.KEY_KP_PageDown;
            break;

         case  FL_KP + '2':
            if( ev.mod & ev.NUMLOCK )
               ev.key   = ev.KEY_KP_2;
            else
               ev.key   = ev.KEY_KP_Down;
            break;

         case  FL_KP + '1':
            if( ev.mod & ev.NUMLOCK )
               ev.key   = ev.KEY_KP_1;
            else
               ev.key   = ev.KEY_KP_End;
            break;

         case  FL_KP + '0':
            if( ev.mod & ev.NUMLOCK )
               ev.key   = ev.KEY_KP_0;
            else
               ev.key   = ev.KEY_KP_Insert;
            break;

         case  FL_KP + '.':
            if( ev.mod & ev.NUMLOCK )
               ev.key   = ev.KEY_KP_Dec;
            else
               ev.key   = ev.KEY_KP_Delete;
            break;

         case  FL_KP + '/':
            ev.key   = ev.KEY_KP_Div;
            break;

         case  FL_KP + '*':
            ev.key   = ev.KEY_KP_Mul;
            break;

         case  FL_KP + '-':
            ev.key   = ev.KEY_KP_Sub;
            break;

         case  FL_KP + '+':
            ev.key   = ev.KEY_KP_Add;
            break;

         case  FL_KP_Enter:
            ev.key   = ev.KEY_KP_Enter;
            break;

         case  FL_F + 1:
            ev.key   = ev.KEY_F1;
            break;

         case  FL_F + 2:
            ev.key   = ev.KEY_F2;
            break;

         case  FL_F + 3:
            ev.key   = ev.KEY_F3;
            break;

         case  FL_F + 4:
            ev.key   = ev.KEY_F4;
            break;

         case  FL_F + 5:
            ev.key   = ev.KEY_F5;
            break;

         case  FL_F + 6:
            ev.key   = ev.KEY_F6;
            break;

         case  FL_F + 7:
            ev.key   = ev.KEY_F7;
            break;

         case  FL_F + 8:
            ev.key   = ev.KEY_F8;
            break;

         case  FL_F + 9:
            ev.key   = ev.KEY_F9;
            break;

         case  FL_F + 10:
            ev.key   = ev.KEY_F10;
            break;

         case  FL_F + 11:
            ev.key   = ev.KEY_F11;
            break;

         case  FL_F + 12:
            ev.key   = ev.KEY_F12;
            break;

         case  FL_Shift_L:
            ev.key   = ev.KEY_Shift_L;
            break;

         case  FL_Shift_R:
            ev.key   = ev.KEY_Shift_R;
            break;

         case  FL_Control_L:
            ev.key   = ev.KEY_Control_L;
            break;

         case  FL_Control_R:
            ev.key   = ev.KEY_Control_R;
            break;

         case  FL_Caps_Lock:
            ev.key   = ev.KEY_Caps_Lock;
            break;

         case  FL_Meta_L:
            ev.key   = ev.KEY_Meta_L;
            break;

         case  FL_Meta_R:
            ev.key   = ev.KEY_Meta_R;
            break;

         case  FL_Alt_L:
            ev.key   = ev.KEY_Alt_L;
            break;

         case  FL_Alt_R:
            ev.key   = ev.KEY_Alt_R;
            break;

         case  FL_Delete:
            ev.key   = ev.KEY_Delete;
            break;

         case  '`':
            if( ev.mod & ev.SHIFT )
               ev.key   = ev.KEY_Tilde;
            else
               ev.key   = ev.KEY_Quote_L;
            break;

         case  '1':
            if( ev.mod & ev.SHIFT )
               ev.key   = ev.KEY_Exclam;
            else
               ev.key   = ev.KEY_1;
            break;

         case  '2':
            if( ev.mod & ev.SHIFT )
               ev.key   = ev.KEY_At;
            else
               ev.key   = ev.KEY_2;
            break;

         case  '3':
            if( ev.mod & ev.SHIFT )
               ev.key   = ev.KEY_NumSign;
            else
               ev.key   = ev.KEY_3;
            break;

         case  '4':
            if( ev.mod & ev.SHIFT )
               ev.key   = ev.KEY_Dollar;
            else
               ev.key   = ev.KEY_4;
            break;

         case  '5':
            if( ev.mod & ev.SHIFT )
               ev.key   = ev.KEY_Percent;
            else
               ev.key   = ev.KEY_5;
            break;

         case  '6':
            if( ev.mod & ev.SHIFT )
               ev.key   = ev.KEY_Caret;
            else
               ev.key   = ev.KEY_6;
            break;

         case  '7':
            if( ev.mod & ev.SHIFT )
               ev.key   = ev.KEY_Ampersand;
            else
               ev.key   = ev.KEY_7;
            break;

         case  '8':
            if( ev.mod & ev.SHIFT )
               ev.key   = ev.KEY_Asterisk;
            else
               ev.key   = ev.KEY_8;
            break;

         case  '9':
            if( ev.mod & ev.SHIFT )
               ev.key   = ev.KEY_Paren_L;
            else
               ev.key   = ev.KEY_9;
            break;

         case  '0':
            if( ev.mod & ev.SHIFT )
               ev.key   = ev.KEY_Paren_R;
            else
               ev.key   = ev.KEY_0;
            break;

         case  '-':
            if( ev.mod & ev.SHIFT )
               ev.key   = ev.KEY_UnderScore;
            else
               ev.key   = ev.KEY_Minus;
            break;

         case  '=':
            if( ev.mod & ev.SHIFT )
               ev.key   = ev.KEY_Plus;
            else
               ev.key   = ev.KEY_Equal;
            break;

         case  '[':
            if( ev.mod & ev.SHIFT )
               ev.key   = ev.KEY_Brace_L;
            else
               ev.key   = ev.KEY_Bracket_L;
            break;

         case  ']':
            if( ev.mod & ev.SHIFT )
               ev.key   = ev.KEY_Brace_R;
            else
               ev.key   = ev.KEY_Bracket_R;
            break;

         case  '\\':
            if( ev.mod & ev.SHIFT )
               ev.key   = ev.KEY_Bar;
            else
               ev.key   = ev.KEY_Backslash;
            break;

         case  ';':
            if( ev.mod & ev.SHIFT )
               ev.key   = ev.KEY_Colon;
            else
               ev.key   = ev.KEY_Semicolon;
            break;

         case  '\'':
            if( ev.mod & ev.SHIFT )
               ev.key   = ev.KEY_DblQuote;
            else
               ev.key   = ev.KEY_Apostrophe;
            break;

         case  ',':
            if( ev.mod & ev.SHIFT )
               ev.key   = ev.KEY_Less;
            else
               ev.key   = ev.KEY_Comma;
            break;

         case  '.':
            if( ev.mod & ev.SHIFT )
               ev.key   = ev.KEY_Greater;
            else
               ev.key   = ev.KEY_Period;
            break;

         case  '/':
            if( ev.mod & ev.SHIFT )
               ev.key   = ev.KEY_Question;
            else
               ev.key   = ev.KEY_Slash;
            break;

         case  'a':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_A;
            else
               ev.key   = ev.KEY_a;
            break;

         case  'b':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_B;
            else
               ev.key   = ev.KEY_b;
            break;

         case  'c':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_C;
            else
               ev.key   = ev.KEY_c;
            break;

         case  'd':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_D;
            else
               ev.key   = ev.KEY_d;
            break;

         case  'e':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_E;
            else
               ev.key   = ev.KEY_e;
            break;

         case  'f':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_F;
            else
               ev.key   = ev.KEY_f;
            break;

         case  'g':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_G;
            else
               ev.key   = ev.KEY_g;
            break;

         case  'h':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_H;
            else
               ev.key   = ev.KEY_h;
            break;

         case  'i':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_I;
            else
               ev.key   = ev.KEY_i;
            break;

         case  'j':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_J;
            else
               ev.key   = ev.KEY_j;
            break;

         case  'k':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_K;
            else
               ev.key   = ev.KEY_k;
            break;

         case  'l':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_L;
            else
               ev.key   = ev.KEY_l;
            break;

         case  'm':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_M;
            else
               ev.key   = ev.KEY_m;
            break;

         case  'n':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_N;
            else
               ev.key   = ev.KEY_n;
            break;

         case  'o':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_O;
            else
               ev.key   = ev.KEY_o;
            break;

         case  'p':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_P;
            else
               ev.key   = ev.KEY_p;
            break;

         case  'q':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_Q;
            else
               ev.key   = ev.KEY_q;
            break;

         case  'r':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_R;
            else
               ev.key   = ev.KEY_r;
            break;

         case  's':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_S;
            else
               ev.key   = ev.KEY_s;
            break;

         case  't':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_T;
            else
               ev.key   = ev.KEY_t;
            break;

         case  'u':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_U;
            else
               ev.key   = ev.KEY_u;
            break;

         case  'v':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_V;
            else
               ev.key   = ev.KEY_v;
            break;

         case  'w':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_W;
            else
               ev.key   = ev.KEY_w;
            break;

         case  'x':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_X;
            else
               ev.key   = ev.KEY_x;
            break;

         case  'y':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_Y;
            else
               ev.key   = ev.KEY_y;
            break;

         case  'z':
            if( ( ev.mod & shf ) && ( ( ev.mod & shf ) != shf ) )
               ev.key   = ev.KEY_Z;
            else
               ev.key   = ev.KEY_z;
            break;

         default:
            break;
      }
   }



   /**
    * Inline unified constructor.
    */
   inline   void
   ctor( void )
   {
      end();

      // this and Widget communicate through
      // their own sig-slot mechanism
      AddSender( this );
   }

   private:
      long  mEvent;
};


#endif // DELTA_FL_DT_WINDOW
