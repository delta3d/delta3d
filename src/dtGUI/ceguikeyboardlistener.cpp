#include <dtGUI/ceguikeyboardlistener.h>
#include <CEGUI/CEGUISystem.h>

using namespace dtGUI;

CEGUIKeyboardListener::CEGUIKeyboardListener()
{
}

CEGUIKeyboardListener::~CEGUIKeyboardListener()
{
}

bool CEGUIKeyboardListener::HandleKeyPressed(const dtCore::Keyboard* keyboard, Producer::KeyboardKey key, Producer::KeyCharacter kchar)
{
   if( CEGUI::Key::Scan scanKey = KeyboardKeyToKeyScan(key) )
   {
      CEGUI::System::getSingleton().injectKeyDown(scanKey);
   }

   return CEGUI::System::getSingleton().injectChar( static_cast<CEGUI::utf32>(kchar) );   
}

bool CEGUIKeyboardListener::HandleKeyReleased(const dtCore::Keyboard* keyboard, Producer::KeyboardKey key, Producer::KeyCharacter kchar)
{
   bool handled(false);
   if( CEGUI::Key::Scan scanKey = KeyboardKeyToKeyScan(key) )
   {
      handled = CEGUI::System::getSingleton().injectKeyUp(scanKey);
   }

   return handled;
}

bool CEGUIKeyboardListener::HandleKeyTyped(const dtCore::Keyboard* keyboard, Producer::KeyboardKey key, Producer::KeyCharacter kchar)
{
   return false;
}

CEGUI::Key::Scan CEGUIKeyboardListener::KeyboardKeyToKeyScan( Producer::KeyboardKey key )
{
   switch(key)
   {
      case Producer::Key_Unknown:
      {
         return CEGUI::Key::Scan(0);//?
      }
      case Producer::Key_Escape:
      {
         return CEGUI::Key::Escape;
      }
      case Producer::Key_F1:
      {
         return CEGUI::Key::F1;
      }
      case Producer::Key_F2:
      {
         return CEGUI::Key::F2;
      }
      case Producer::Key_F3:
      {
         return CEGUI::Key::F3;
      }
      case Producer::Key_F4:
      {
         return CEGUI::Key::F4;
      }
      case Producer::Key_F5:
      {
         return CEGUI::Key::F5;
      }
      case Producer::Key_F6:
      {
         return CEGUI::Key::F6;
      }
      case Producer::Key_F7:
      {
         return CEGUI::Key::F7;
      }
      case Producer::Key_F8:
      {
         return CEGUI::Key::F8;
      }
      case Producer::Key_F9:
      {
         return CEGUI::Key::F9;
      }
      case Producer::Key_F10:
      {
         return CEGUI::Key::F10;
      }
      case Producer::Key_F11:
      {
         return CEGUI::Key::F11;
      }
      case Producer::Key_F12:
      {
         return CEGUI::Key::F12;
      }
      case Producer::Key_quoteleft:
      {
         return CEGUI::Key::Apostrophe;
      }
      case Producer::Key_1:
      {
         return CEGUI::Key::One;
      }
      case Producer::Key_2:
      {
         return CEGUI::Key::Two;
      }
      case Producer::Key_3:
      {
         return CEGUI::Key::Three;
      }
      case Producer::Key_4:
      {
         return CEGUI::Key::Four;
      }
      case Producer::Key_5:
      {
         return CEGUI::Key::Five;
      }
      case Producer::Key_6:
      {
         return CEGUI::Key::Six;
      }
      case Producer::Key_7:
      {
         return CEGUI::Key::Seven;
      }
      case Producer::Key_8:
      {
         return CEGUI::Key::Eight;
      }
      case Producer::Key_9:
      {
         return CEGUI::Key::Nine;
      }
      case Producer::Key_0:
      {
         return CEGUI::Key::Zero;
      }
      case Producer::Key_minus:
      {
         return CEGUI::Key::Minus;
      }
      case Producer::Key_equal:
      {
         return CEGUI::Key::Equals;
      }
      case Producer::Key_BackSpace:
      {
         return CEGUI::Key::Backspace;
      }
      case Producer::Key_Tab:
      {
         return CEGUI::Key::Tab;
      }
      case Producer::Key_A:
      {
         return CEGUI::Key::A;
      }
      case Producer::Key_B:
      {
         return CEGUI::Key::B;
      }
      case Producer::Key_C:
      {
         return CEGUI::Key::C;
      }
      case Producer::Key_D:
      {
         return CEGUI::Key::D;
      }
      case Producer::Key_E:
      {
         return CEGUI::Key::E;
      }
      case Producer::Key_F:
      {
         return CEGUI::Key::F;
      }
      case Producer::Key_G:
      {
         return CEGUI::Key::G;
      }
      case Producer::Key_H:
      {
         return CEGUI::Key::H;
      }
      case Producer::Key_I:
      {
         return CEGUI::Key::I;
      }
      case Producer::Key_J:
      {
         return CEGUI::Key::J;
      }
      case Producer::Key_K:
      {
         return CEGUI::Key::K;
      }
      case Producer::Key_L:
      {
         return CEGUI::Key::L;
      }
      case Producer::Key_M:
      {
         return CEGUI::Key::M;
      }
      case Producer::Key_N:
      {
         return CEGUI::Key::N;
      }
      case Producer::Key_O:
      {
         return CEGUI::Key::O;
      }
      case Producer::Key_P:
      {
         return CEGUI::Key::P;
      }
      case Producer::Key_Q:
      {
         return CEGUI::Key::Q;
      }
      case Producer::Key_R:
      {
         return CEGUI::Key::R;
      }
      case Producer::Key_S:
      {
         return CEGUI::Key::S;
      }
      case Producer::Key_T:
      {
         return CEGUI::Key::T;
      }
      case Producer::Key_U:
      {
         return CEGUI::Key::U;
      }
      case Producer::Key_V:
      {
         return CEGUI::Key::V;
      }
      case Producer::Key_W:
      {
         return CEGUI::Key::W;
      }
      case Producer::Key_X:
      {
         return CEGUI::Key::X;
      }
      case Producer::Key_Y:
      {
         return CEGUI::Key::Y;
      }
      case Producer::Key_Z:
      {
         return CEGUI::Key::Z;
      }
      case Producer::Key_bracketleft:
      {
         return CEGUI::Key::LeftBracket;
      }
      case Producer::Key_bracketright:
      {
         return CEGUI::Key::RightBracket;
      }
      case Producer::Key_backslash:
      {
         return CEGUI::Key::Backslash;
      }
      case Producer::Key_Caps_Lock:
      {
         return CEGUI::Key::Capital;
      }
      case Producer::Key_semicolon:
      {
         return CEGUI::Key::Semicolon;
      }
      case Producer::Key_apostrophe:
      {
         return CEGUI::Key::Apostrophe;
      }
      case Producer::Key_Return:
      {
         return CEGUI::Key::Return;
      }
      case Producer::Key_Shift_L:
      {
         return CEGUI::Key::LeftShift;
      }
      case Producer::Key_comma:
      {
         return CEGUI::Key::Comma;
      }
      case Producer::Key_period:
      {
         return CEGUI::Key::Period;
      }
      case Producer::Key_slash:
      {
         return CEGUI::Key::Slash;
      }
      case Producer::Key_Shift_R:
      {
         return CEGUI::Key::RightShift;
      }
      case Producer::Key_Control_L:
      {
         return CEGUI::Key::LeftControl;
      }
      case Producer::Key_Super_L:
      {
         return CEGUI::Key::Scan(0);//?
      }
      case Producer::Key_space:
      {
         return CEGUI::Key::Space;
      }
      case Producer::Key_Alt_L:
      {
         return CEGUI::Key::LeftAlt;
      }
      case Producer::Key_Alt_R:
      {
         return CEGUI::Key::RightAlt;
      }
      case Producer::Key_Super_R:
      {
         return CEGUI::Key::Scan(0);//?
      }
      case Producer::Key_Menu:
      {
         return CEGUI::Key::Scan(0);//?
      }
      case Producer::Key_Control_R:
      {
         return CEGUI::Key::RightControl;
      }
      case Producer::Key_Print:
      {
         return CEGUI::Key::SysRq;
      }
      case Producer::Key_Scroll_Lock:
      {
         return CEGUI::Key::ScrollLock;
      }
      case Producer::Key_Pause:
      {
         return CEGUI::Key::Pause;
      }
      case Producer::Key_Home:
      {
         return CEGUI::Key::Home;
      }
      case Producer::Key_Page_Up:
      {
         return CEGUI::Key::PageUp;
      }
      case Producer::Key_End:
      {
         return CEGUI::Key::End;
      }
      case Producer::Key_Page_Down:
      {
         return CEGUI::Key::PageDown;
      }
      case Producer::Key_Delete:
      {
         return CEGUI::Key::Delete;
      }
      case Producer::Key_Insert:
      {
         return CEGUI::Key::Insert;
      }
      case Producer::Key_Left:
      {
         return CEGUI::Key::ArrowLeft;
      }
      case Producer::Key_Up:
      {
         return CEGUI::Key::ArrowUp;
      }
      case Producer::Key_Right:
      {
         return CEGUI::Key::ArrowRight;
      }
      case Producer::Key_Down:
      {
         return CEGUI::Key::ArrowDown;
      }
      case Producer::Key_Num_Lock:
      {
         return CEGUI::Key::NumLock;
      }
      case Producer::Key_KP_Divide:
      {
         return CEGUI::Key::Divide;
      }
      case Producer::Key_KP_Multiply:
      {
         return CEGUI::Key::Multiply;
      }
      case Producer::Key_KP_Subtract:
      {
         return CEGUI::Key::Subtract;
      }
      case Producer::Key_KP_Add:
      {
         return CEGUI::Key::Add;
      }
      case Producer::Key_KP_Home:
      {
         return CEGUI::Key::Numpad7;
      }
      case Producer::Key_KP_Up:
      {
         return CEGUI::Key::Numpad8;
      }
      case Producer::Key_KP_Page_Up:
      {
         return CEGUI::Key::Numpad9;
      }
      case Producer::Key_KP_Left:
      {
         return CEGUI::Key::Numpad4;
      }
      case Producer::Key_KP_Begin:
      {
         return CEGUI::Key::Scan(0); //?
      }
      case Producer::Key_KP_Right:
      {
         return CEGUI::Key::Numpad6;
      }
      case Producer::Key_KP_End:
      {
         return CEGUI::Key::Numpad1;
      }
      case Producer::Key_KP_Down:
      {
         return CEGUI::Key::Numpad2;
      }
      case Producer::Key_KP_Page_Down:
      {
         return CEGUI::Key::Numpad3;
      }
      case Producer::Key_KP_Insert:
      {
         return CEGUI::Key::Numpad0;
      }
      case Producer::Key_KP_Delete:
      {
         return CEGUI::Key::Decimal;
      }
      case Producer::Key_KP_Enter:
      {
         return CEGUI::Key::NumpadEnter;
      }
      default:
      {
         return CEGUI::Key::Scan(0);
      }
   }
}

