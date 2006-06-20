// keyboard.cpp: Implementation of the Keyboard class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/keyboard.h>
#include <algorithm>

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(Keyboard)

Keyboard::Keyboard(const std::string& name) : InputDevice(name)
{
   RegisterInstance(this);

   AddFeature(new Button(this, "Unknown"));
   AddFeature(new Button(this, "Esc"));
   AddFeature(new Button(this, "F1"));
   AddFeature(new Button(this, "F2"));
   AddFeature(new Button(this, "F3"));
   AddFeature(new Button(this, "F4"));
   AddFeature(new Button(this, "F5"));
   AddFeature(new Button(this, "F6"));
   AddFeature(new Button(this, "F7"));
   AddFeature(new Button(this, "F8"));
   AddFeature(new Button(this, "F9"));
   AddFeature(new Button(this, "F10"));
   AddFeature(new Button(this, "F11"));
   AddFeature(new Button(this, "F12"));
   AddFeature(new Button(this, "`"));

   AddFeature(new Button(this, "1"));
   AddFeature(new Button(this, "2"));
   AddFeature(new Button(this, "3"));
   AddFeature(new Button(this, "4"));
   AddFeature(new Button(this, "5"));
   AddFeature(new Button(this, "6"));
   AddFeature(new Button(this, "7"));
   AddFeature(new Button(this, "8"));
   AddFeature(new Button(this, "9"));
   AddFeature(new Button(this, "0"));
   AddFeature(new Button(this, "-"));
   AddFeature(new Button(this, "="));

   AddFeature(new Button(this, "Backspace"));
   AddFeature(new Button(this, "Tab"));
   AddFeature(new Button(this, "A"));
   AddFeature(new Button(this, "B"));
   AddFeature(new Button(this, "C"));
   AddFeature(new Button(this, "D"));
   AddFeature(new Button(this, "E"));
   AddFeature(new Button(this, "F"));
   AddFeature(new Button(this, "G"));
   AddFeature(new Button(this, "H"));
   AddFeature(new Button(this, "I"));
   AddFeature(new Button(this, "J"));
   AddFeature(new Button(this, "K"));
   AddFeature(new Button(this, "L"));
   AddFeature(new Button(this, "M"));
   AddFeature(new Button(this, "N"));
   AddFeature(new Button(this, "O"));
   AddFeature(new Button(this, "P"));
   AddFeature(new Button(this, "Q"));
   AddFeature(new Button(this, "R"));
   AddFeature(new Button(this, "S"));
   AddFeature(new Button(this, "T"));
   AddFeature(new Button(this, "U"));
   AddFeature(new Button(this, "V"));
   AddFeature(new Button(this, "W"));
   AddFeature(new Button(this, "X"));
   AddFeature(new Button(this, "Y"));
   AddFeature(new Button(this, "Z"));

   AddFeature(new Button(this, "["));
   AddFeature(new Button(this, "]"));
   AddFeature(new Button(this, "\\"));
   AddFeature(new Button(this, "Caps Lock"));
   AddFeature(new Button(this, ";"));
   AddFeature(new Button(this, "'"));
   AddFeature(new Button(this, "Enter"));
   AddFeature(new Button(this, "Left Shift"));
   AddFeature(new Button(this, ","));
   AddFeature(new Button(this, "."));
   AddFeature(new Button(this, "/"));
   AddFeature(new Button(this, "Right Shift"));
   AddFeature(new Button(this, "Left Ctrl"));
   AddFeature(new Button(this, "Left Super"));
   AddFeature(new Button(this, "Space"));
   AddFeature(new Button(this, "Left Alt"));
   AddFeature(new Button(this, "Right Alt"));
   AddFeature(new Button(this, "Right Super"));
   AddFeature(new Button(this, "Menu"));
   AddFeature(new Button(this, "Right Ctrl"));
   AddFeature(new Button(this, "Print Screen"));
   AddFeature(new Button(this, "Scroll Lock"));
   AddFeature(new Button(this, "Pause"));
   AddFeature(new Button(this, "Home"));
   AddFeature(new Button(this, "Page Up"));
   AddFeature(new Button(this, "End"));
   AddFeature(new Button(this, "Page Down"));
   AddFeature(new Button(this, "Delete"));
   AddFeature(new Button(this, "Insert"));
   AddFeature(new Button(this, "Left"));
   AddFeature(new Button(this, "Up"));
   AddFeature(new Button(this, "Right"));
   AddFeature(new Button(this, "Down"));
   AddFeature(new Button(this, "Num Lock"));

   AddFeature(new Button(this, "Keypad /"));
   AddFeature(new Button(this, "Keypad *"));
   AddFeature(new Button(this, "Keypad -"));
   AddFeature(new Button(this, "Keypad +"));
   AddFeature(new Button(this, "Keypad 7"));
   AddFeature(new Button(this, "Keypad 8"));
   AddFeature(new Button(this, "Keypad 9"));
   AddFeature(new Button(this, "Keypad 4"));
   AddFeature(new Button(this, "Keypad 5"));
   AddFeature(new Button(this, "Keypad 6"));
   AddFeature(new Button(this, "Keypad 1"));
   AddFeature(new Button(this, "Keypad 2"));
   AddFeature(new Button(this, "Keypad 3"));
   AddFeature(new Button(this, "Keypad 0"));
   AddFeature(new Button(this, "Keypad ."));
   AddFeature(new Button(this, "Keypad Enter"));
}

Keyboard::~Keyboard()
{
   DeregisterInstance(this);
}

bool Keyboard::GetKeyState(Producer::KeyboardKey key)
{
   return GetButton(key)->GetState();
}

void Keyboard::AddKeyboardListener(KeyboardListener* keyboardListener)
{
   mKeyboardListeners.push_back( keyboardListener );
}

void Keyboard::InsertKeyboardListener(const KeyboardListenerList::value_type& pos, KeyboardListener* kbl)
{
   KeyboardListenerList::iterator iter = std::find( mKeyboardListeners.begin() , mKeyboardListeners.end() , pos );
   mKeyboardListeners.insert(iter,kbl);
}

void Keyboard::RemoveKeyboardListener(KeyboardListener* keyboardListener)
{
   mKeyboardListeners.remove(keyboardListener);
}

Producer::KeyboardKey Keyboard::KeyCharacterToKeyboardKey(Producer::KeyCharacter kc)
{
   switch(kc)
   {
      case Producer::KeyChar_Escape:
         return Producer::Key_Escape;

      case Producer::KeyChar_F1:
         return Producer::Key_F1;

      case Producer::KeyChar_F2:
         return Producer::Key_F2;

      case Producer::KeyChar_F3:
         return Producer::Key_F3;

      case Producer::KeyChar_F4:
         return Producer::Key_F4;

      case Producer::KeyChar_F5:
         return Producer::Key_F5;

      case Producer::KeyChar_F6:
         return Producer::Key_F6;

      case Producer::KeyChar_F7:
         return Producer::Key_F7;
      
      case Producer::KeyChar_F8:
         return Producer::Key_F8;

      case Producer::KeyChar_F9:
         return Producer::Key_F9;

      case Producer::KeyChar_F10:
         return Producer::Key_F10;

      case Producer::KeyChar_F11:
         return Producer::Key_F11;

      case Producer::KeyChar_F12:
         return Producer::Key_F12;

      case Producer::KeyChar_Print:
      case Producer::KeyChar_Sys_Req:
         return Producer::Key_Print;

      case Producer::KeyChar_Scroll_Lock:
         return Producer::Key_Scroll_Lock;

      case Producer::KeyChar_Pause:
      case Producer::KeyChar_Break:
         return Producer::Key_Pause;

      case Producer::KeyChar_asciitilde:
      case Producer::KeyChar_quoteleft:
         return Producer::Key_quoteleft;

      case Producer::KeyChar_exclam:
      case Producer::KeyChar_1:
         return Producer::Key_1;

      case Producer::KeyChar_at:
      case Producer::KeyChar_2:
         return Producer::Key_2;

      case Producer::KeyChar_numbersign:
      case Producer::KeyChar_3:
         return Producer::Key_3;

      case Producer::KeyChar_dollar:
      case Producer::KeyChar_4:
         return Producer::Key_4;
      
      case Producer::KeyChar_percent:
      case Producer::KeyChar_5:
         return Producer::Key_5;

      case Producer::KeyChar_asciicircum:
      case Producer::KeyChar_6:
         return Producer::Key_6;

      case Producer::KeyChar_ampersand:
      case Producer::KeyChar_7:
         return Producer::Key_7;

      case Producer::KeyChar_asterisk:
      case Producer::KeyChar_8:
         return Producer::Key_8;

      case Producer::KeyChar_parenleft:
      case Producer::KeyChar_9:
         return Producer::Key_9;

      case Producer::KeyChar_parenright:
      case Producer::KeyChar_0:
         return Producer::Key_0;

      case Producer::KeyChar_underscore:
      case Producer::KeyChar_minus:
         return Producer::Key_minus;

      case Producer::KeyChar_plus:
      case Producer::KeyChar_equal:
         return Producer::Key_equal;

      case Producer::KeyChar_BackSpace:
         return Producer::Key_BackSpace;

      case Producer::KeyChar_Tab:
         return Producer::Key_Tab;

      case Producer::KeyChar_bracketleft:
      case Producer::KeyChar_braceleft:
         return Producer::Key_bracketleft;

      case Producer::KeyChar_bracketright:
      case Producer::KeyChar_braceright:
         return Producer::Key_bracketright;

      case Producer::KeyChar_backslash:
      case Producer::KeyChar_bar:
         return Producer::Key_backslash;

      case Producer::KeyChar_Caps_Lock:
         return Producer::Key_Caps_Lock;

      case Producer::KeyChar_colon:
      case Producer::KeyChar_semicolon:
         return Producer::Key_semicolon;

      case Producer::KeyChar_apostrophe:
      case Producer::KeyChar_quotedbl:
         return Producer::Key_apostrophe;

      case Producer::KeyChar_Return:
         return Producer::Key_Return;

      case Producer::KeyChar_Shift_L:
         return Producer::Key_Shift_L;

      case Producer::KeyChar_comma:
      case Producer::KeyChar_greater:
         return Producer::Key_comma;

      case Producer::KeyChar_period:
      case Producer::KeyChar_less:
         return Producer::Key_period;

      case Producer::KeyChar_question:
      case Producer::KeyChar_slash:
         return Producer::Key_slash;

      case Producer::KeyChar_Shift_R:
         return Producer::Key_Shift_R;

      case Producer::KeyChar_Control_L:
         return Producer::Key_Control_L;

      case Producer::KeyChar_Super_L:
         return Producer::Key_Super_L;

      case Producer::KeyChar_Alt_L:
         return Producer::Key_Alt_L;

      case Producer::KeyChar_space:
         return Producer::Key_space;

      case Producer::KeyChar_Alt_R:
         return Producer::Key_Alt_R;

      case Producer::KeyChar_Super_R:
         return Producer::Key_Super_R;

      case Producer::KeyChar_Menu:
         return Producer::Key_Menu;

      case Producer::KeyChar_Control_R:
         return Producer::Key_Control_R;

      case Producer::KeyChar_Insert:
         return Producer::Key_Insert;

      case Producer::KeyChar_Home:
         return Producer::Key_Home;

      case Producer::KeyChar_Page_Up:
         return Producer::Key_Page_Up;

      case Producer::KeyChar_Delete:
         return Producer::Key_Delete;

      case Producer::KeyChar_End:
         return Producer::Key_End;

      case Producer::KeyChar_Page_Down:
         return Producer::Key_Page_Down;

      case Producer::KeyChar_Up:
         return Producer::Key_Up;

      case Producer::KeyChar_Down:
         return Producer::Key_Down;

      case Producer::KeyChar_Left:
         return Producer::Key_Left;

      case Producer::KeyChar_Right:
         return Producer::Key_Right;

      case Producer::KeyChar_Num_Lock:
         return Producer::Key_Num_Lock;

      case Producer::KeyChar_KP_Divide:
         return Producer::Key_KP_Divide;

      case Producer::KeyChar_KP_Multiply:
         return Producer::Key_KP_Multiply;

      case Producer::KeyChar_KP_Subtract:
         return Producer::Key_KP_Subtract;

      case Producer::KeyChar_KP_Add:
         return Producer::Key_KP_Add;

      case Producer::KeyChar_KP_Enter:
         return Producer::Key_KP_Enter;

      case Producer::KeyChar_KP_7:
      case Producer::KeyChar_KP_Home:
         return Producer::Key_KP_Home;

      case Producer::KeyChar_KP_8:
      case Producer::KeyChar_KP_Up:
         return Producer::Key_KP_Up;

      case Producer::KeyChar_KP_9:
      case Producer::KeyChar_KP_Page_Up:
         return Producer::Key_KP_Page_Up;

      case Producer::KeyChar_KP_4:
      case Producer::KeyChar_KP_Left:
         return Producer::Key_KP_Left;

      case Producer::KeyChar_KP_5:
      case Producer::KeyChar_KP_Begin:
         return Producer::Key_KP_Begin;

      case Producer::KeyChar_KP_6:
      case Producer::KeyChar_KP_Right:
         return Producer::Key_KP_Right;

      case Producer::KeyChar_KP_1:
      case Producer::KeyChar_KP_End:
         return Producer::Key_KP_End;

      case Producer::KeyChar_KP_2:
      case Producer::KeyChar_KP_Down:
         return Producer::Key_KP_Down;

      case Producer::KeyChar_KP_3:
      case Producer::KeyChar_KP_Page_Down:
         return Producer::Key_KP_Page_Down;

      case Producer::KeyChar_KP_0:
      case Producer::KeyChar_KP_Insert:
         return Producer::Key_KP_Insert;

      case Producer::KeyChar_KP_Decimal:
      case Producer::KeyChar_KP_Delete:
         return Producer::Key_KP_Delete;

      case Producer::KeyChar_A:
      case Producer::KeyChar_a:
         return Producer::Key_A;

      case Producer::KeyChar_B:
      case Producer::KeyChar_b:
         return Producer::Key_B;

      case Producer::KeyChar_C:
      case Producer::KeyChar_c:
         return Producer::Key_C;

      case Producer::KeyChar_D:
      case Producer::KeyChar_d:
         return Producer::Key_D;
      
      case Producer::KeyChar_E:
      case Producer::KeyChar_e:
         return Producer::Key_E;

      case Producer::KeyChar_F:
      case Producer::KeyChar_f:
         return Producer::Key_F;

      case Producer::KeyChar_G:
      case Producer::KeyChar_g:
         return Producer::Key_G;

      case Producer::KeyChar_H:
      case Producer::KeyChar_h:
         return Producer::Key_H;

      case Producer::KeyChar_I:
      case Producer::KeyChar_i:
         return Producer::Key_I;

      case Producer::KeyChar_J:
      case Producer::KeyChar_j:
         return Producer::Key_J;

      case Producer::KeyChar_K:
      case Producer::KeyChar_k:
         return Producer::Key_K;

      case Producer::KeyChar_L:
      case Producer::KeyChar_l:
         return Producer::Key_L;

      case Producer::KeyChar_M:
      case Producer::KeyChar_m:
         return Producer::Key_M;

      case Producer::KeyChar_N:
      case Producer::KeyChar_n:
         return Producer::Key_N;
      
      case Producer::KeyChar_O:
      case Producer::KeyChar_o:
         return Producer::Key_O;

      case Producer::KeyChar_P:
      case Producer::KeyChar_p:
         return Producer::Key_P;

      case Producer::KeyChar_Q:
      case Producer::KeyChar_q:
         return Producer::Key_Q;

      case Producer::KeyChar_R:
      case Producer::KeyChar_r:
         return Producer::Key_R;

      case Producer::KeyChar_S:
      case Producer::KeyChar_s:
         return Producer::Key_S;

      case Producer::KeyChar_T:
      case Producer::KeyChar_t:
         return Producer::Key_T;

      case Producer::KeyChar_U:
      case Producer::KeyChar_u:
         return Producer::Key_U;

      case Producer::KeyChar_V:
      case Producer::KeyChar_v:
         return Producer::Key_V;

      case Producer::KeyChar_W:
      case Producer::KeyChar_w:
         return Producer::Key_W;

      case Producer::KeyChar_X:
      case Producer::KeyChar_x:
         return Producer::Key_X;
      
      case Producer::KeyChar_Y:
      case Producer::KeyChar_y:
         return Producer::Key_Y;

      case Producer::KeyChar_Z:
      case Producer::KeyChar_z:
         return Producer::Key_Z;

      case Producer::KeyChar_Unknown:
      default:
         return Producer::Key_Unknown;
   }
}

bool Keyboard::KeyDown(Producer::KeyCharacter kc)
{
   Producer::KeyboardKey kbkey = KeyCharacterToKeyboardKey(kc);

   bool handled(false);
   KeyboardListenerList::iterator iter = mKeyboardListeners.begin();
   KeyboardListenerList::iterator enditer = mKeyboardListeners.end();
   while( !handled && iter!=enditer )
   {
      handled = (*iter)->HandleKeyPressed(this, kbkey, kc);
      ++iter;
   }

   if( !handled )  // affect the return value
   {
      handled = GetButton(kbkey)->SetState(true);
   }
   else  // don't affect the return value, but change the state for "pollers of the state"
   {
      GetButton(kbkey)->SetState(true);
   }

   return handled;
}

bool Keyboard::KeyUp(Producer::KeyCharacter kc)
{
   Producer::KeyboardKey kbkey = KeyCharacterToKeyboardKey(kc);

   bool handled(false);
   KeyboardListenerList::iterator iter = mKeyboardListeners.begin();
   KeyboardListenerList::iterator enditer = mKeyboardListeners.end();
   while( !handled && iter!=enditer )
   {
      handled = (*iter)->HandleKeyReleased(this, kbkey, kc);
      ++iter;
   }

   if( !handled )   // affect the return value
   {
      handled = GetButton(kbkey)->SetState(false);
   }
   else  // don't affect the return value, but change the state for "pollers of the state"
   {
      GetButton(kbkey)->SetState(false);
   }

   return handled;
}

