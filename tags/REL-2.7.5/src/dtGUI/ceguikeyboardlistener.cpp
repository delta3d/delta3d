#include <dtGUI/ceguikeyboardlistener.h>
#if defined(CEGUI_VERSION_MAJOR) && CEGUI_VERSION_MAJOR >= 0 && defined(CEGUI_VERSION_MINOR) && CEGUI_VERSION_MINOR == 6
#include <dtGUI/hud.h>
#endif
#include <CEGUI/CEGUISystem.h>

#include <osgGA/GUIEventAdapter>

////////////////////////////////////////////////////////////////////////////////
using namespace dtGUI;

////////////////////////////////////////////////////////////////////////////////
#if CEGUI_VERSION_MAJOR >= 0 && CEGUI_VERSION_MINOR >= 7
   CEGUIKeyboardListener::CEGUIKeyboardListener()
   {
   }
#else
   CEGUIKeyboardListener::CEGUIKeyboardListener(HUD *pGUI):
   m_pGUI(pGUI)
   {
   }
#endif


////////////////////////////////////////////////////////////////////////////////
CEGUIKeyboardListener::~CEGUIKeyboardListener()
{
}

////////////////////////////////////////////////////////////////////////////////
bool CEGUIKeyboardListener::HandleKeyPressed(const dtCore::Keyboard* keyboard, int key)
{
   bool handled(false);
   if( CEGUI::Key::Scan scanKey = KeyboardKeyToKeyScan(key) )
   {
      MakeCurrent();
      handled = CEGUI::System::getSingleton().injectKeyDown(scanKey);
   }

   if (!handled)
   {
      MakeCurrent();
      handled = CEGUI::System::getSingleton().injectChar( static_cast<CEGUI::utf32>(key) );
   }

   return handled;
}

////////////////////////////////////////////////////////////////////////////////
bool CEGUIKeyboardListener::HandleKeyReleased(const dtCore::Keyboard* keyboard, int key)
{
   bool handled(false);
   if( CEGUI::Key::Scan scanKey = KeyboardKeyToKeyScan(key) )
   {
      MakeCurrent();

      handled = CEGUI::System::getSingleton().injectKeyUp(scanKey);
   }

   return handled;
}

////////////////////////////////////////////////////////////////////////////////
bool CEGUIKeyboardListener::HandleKeyTyped(const dtCore::Keyboard* keyboard, int key)
{
   return false;
}

////////////////////////////////////////////////////////////////////////////////
CEGUI::Key::Scan CEGUIKeyboardListener::KeyboardKeyToKeyScan( int key )
{
   switch(key)
   {
//      case Producer::Key_Unknown:
//      {
//         return CEGUI::Key::Scan(0);//?
//      }
      case osgGA::GUIEventAdapter::KEY_Escape:
      {
         return CEGUI::Key::Escape;
      }
      case osgGA::GUIEventAdapter::KEY_F1:
      {
         return CEGUI::Key::F1;
      }
      case osgGA::GUIEventAdapter::KEY_F2:
      {
         return CEGUI::Key::F2;
      }
      case osgGA::GUIEventAdapter::KEY_F3:
      {
         return CEGUI::Key::F3;
      }
      case osgGA::GUIEventAdapter::KEY_F4:
      {
         return CEGUI::Key::F4;
      }
      case osgGA::GUIEventAdapter::KEY_F5:
      {
         return CEGUI::Key::F5;
      }
      case osgGA::GUIEventAdapter::KEY_F6:
      {
         return CEGUI::Key::F6;
      }
      case osgGA::GUIEventAdapter::KEY_F7:
      {
         return CEGUI::Key::F7;
      }
      case osgGA::GUIEventAdapter::KEY_F8:
      {
         return CEGUI::Key::F8;
      }
      case osgGA::GUIEventAdapter::KEY_F9:
      {
         return CEGUI::Key::F9;
      }
      case osgGA::GUIEventAdapter::KEY_F10:
      {
         return CEGUI::Key::F10;
      }
      case osgGA::GUIEventAdapter::KEY_F11:
      {
         return CEGUI::Key::F11;
      }
      case osgGA::GUIEventAdapter::KEY_F12:
      {
         return CEGUI::Key::F12;
      }
      case '`':
      {
         return CEGUI::Key::Apostrophe;
      }
      case '1':
      {
         return CEGUI::Key::One;
      }
      case '2':
      {
         return CEGUI::Key::Two;
      }
      case '3':
      {
         return CEGUI::Key::Three;
      }
      case '4':
      {
         return CEGUI::Key::Four;
      }
      case '5':
      {
         return CEGUI::Key::Five;
      }
      case '6':
      {
         return CEGUI::Key::Six;
      }
      case '7':
      {
         return CEGUI::Key::Seven;
      }
      case '8':
      {
         return CEGUI::Key::Eight;
      }
      case '9':
      {
         return CEGUI::Key::Nine;
      }
      case '0':
      {
         return CEGUI::Key::Zero;
      }
      case '-':
      {
         return CEGUI::Key::Minus;
      }
      case '=':
      {
         return CEGUI::Key::Equals;
      }
      case osgGA::GUIEventAdapter::KEY_BackSpace:
      {
         return CEGUI::Key::Backspace;
      }
      case osgGA::GUIEventAdapter::KEY_Tab:
      {
         return CEGUI::Key::Tab;
      }
      case 'a':
      case 'A':
      {
         return CEGUI::Key::A;
      }
      case 'b':
      case 'B':
      {
         return CEGUI::Key::B;
      }
      case 'c':
      case 'C':
      {
         return CEGUI::Key::C;
      }
      case 'd':
      case 'D':
      {
         return CEGUI::Key::D;
      }
      case 'e':
      case 'E':
      {
         return CEGUI::Key::E;
      }
      case 'f':
      case 'F':
      {
         return CEGUI::Key::F;
      }
      case 'g':
      case 'G':
      {
         return CEGUI::Key::G;
      }
      case 'h':
      case 'H':
      {
         return CEGUI::Key::H;
      }
      case 'i':
      case 'I':
      {
         return CEGUI::Key::I;
      }
      case 'j':
      case 'J':
      {
         return CEGUI::Key::J;
      }
      case 'k':
      case 'K':
      {
         return CEGUI::Key::K;
      }
      case 'l':
      case 'L':
      {
         return CEGUI::Key::L;
      }
      case 'm':
      case 'M':
      {
         return CEGUI::Key::M;
      }
      case 'n':
      case 'N':
      {
         return CEGUI::Key::N;
      }
      case 'o':
      case 'O':
      {
         return CEGUI::Key::O;
      }
      case 'p':
      case 'P':
      {
         return CEGUI::Key::P;
      }
      case 'q':
      case 'Q':
      {
         return CEGUI::Key::Q;
      }
      case 'r':
      case 'R':
      {
         return CEGUI::Key::R;
      }
      case 's':
      case 'S':
      {
         return CEGUI::Key::S;
      }
      case 't':
      case 'T':
      {
         return CEGUI::Key::T;
      }
      case 'u':
      case 'U':
      {
         return CEGUI::Key::U;
      }
      case 'v':
      case 'V':
      {
         return CEGUI::Key::V;
      }
      case 'w':
      case 'W':
      {
         return CEGUI::Key::W;
      }
      case 'x':
      case 'X':
      {
         return CEGUI::Key::X;
      }
      case 'y':
      case 'Y':
      {
         return CEGUI::Key::Y;
      }
      case 'z':
      case 'Z':
      {
         return CEGUI::Key::Z;
      }
      case '(':
      {
         return CEGUI::Key::LeftBracket;
      }
      case ')':
      {
         return CEGUI::Key::RightBracket;
      }
      case '\\':
      {
         return CEGUI::Key::Backslash;
      }
      case osgGA::GUIEventAdapter::KEY_Shift_Lock:
      {
         return CEGUI::Key::Capital;
      }
      case ';':
      {
         return CEGUI::Key::Semicolon;
      }
      case '\'':
      {
         return CEGUI::Key::Apostrophe;
      }
      case osgGA::GUIEventAdapter::KEY_Return:
      {
         return CEGUI::Key::Return;
      }
      case osgGA::GUIEventAdapter::KEY_Shift_L:
      {
         return CEGUI::Key::LeftShift;
      }
      case ',':
      {
         return CEGUI::Key::Comma;
      }
      case '.':
      {
         return CEGUI::Key::Period;
      }
      case '/':
      {
         return CEGUI::Key::Slash;
      }
      case osgGA::GUIEventAdapter::KEY_Shift_R:
      {
         return CEGUI::Key::RightShift;
      }
      case osgGA::GUIEventAdapter::KEY_Control_L:
      {
         return CEGUI::Key::LeftControl;
      }
      case osgGA::GUIEventAdapter::KEY_Super_L:
      {
         return CEGUI::Key::Scan(0);//?
      }
      case ' ':
      {
         return CEGUI::Key::Space;
      }
      case osgGA::GUIEventAdapter::KEY_Alt_L:
      {
         return CEGUI::Key::LeftAlt;
      }
      case osgGA::GUIEventAdapter::KEY_Alt_R:
      {
         return CEGUI::Key::RightAlt;
      }
      case osgGA::GUIEventAdapter::KEY_Super_R:
      {
         return CEGUI::Key::Scan(0);//?
      }
      case osgGA::GUIEventAdapter::KEY_Menu:
      {
         return CEGUI::Key::Scan(0);//?
      }
      case osgGA::GUIEventAdapter::KEY_Control_R:
      {
         return CEGUI::Key::RightControl;
      }
      case osgGA::GUIEventAdapter::KEY_Print:
      {
         return CEGUI::Key::SysRq;
      }
      case osgGA::GUIEventAdapter::KEY_Scroll_Lock:
      {
         return CEGUI::Key::ScrollLock;
      }
      case osgGA::GUIEventAdapter::KEY_Pause:
      {
         return CEGUI::Key::Pause;
      }
      case osgGA::GUIEventAdapter::KEY_Home:
      {
         return CEGUI::Key::Home;
      }
      case osgGA::GUIEventAdapter::KEY_Page_Up:
      {
         return CEGUI::Key::PageUp;
      }
      case osgGA::GUIEventAdapter::KEY_End:
      {
         return CEGUI::Key::End;
      }
      case osgGA::GUIEventAdapter::KEY_Page_Down:
      {
         return CEGUI::Key::PageDown;
      }
      case osgGA::GUIEventAdapter::KEY_Delete:
      {
         return CEGUI::Key::Delete;
      }
      case osgGA::GUIEventAdapter::KEY_Insert:
      {
         return CEGUI::Key::Insert;
      }
      case osgGA::GUIEventAdapter::KEY_Left:
      {
         return CEGUI::Key::ArrowLeft;
      }
      case osgGA::GUIEventAdapter::KEY_Up:
      {
         return CEGUI::Key::ArrowUp;
      }
      case osgGA::GUIEventAdapter::KEY_Right:
      {
         return CEGUI::Key::ArrowRight;
      }
      case osgGA::GUIEventAdapter::KEY_Down:
      {
         return CEGUI::Key::ArrowDown;
      }
      case osgGA::GUIEventAdapter::KEY_Num_Lock:
      {
         return CEGUI::Key::NumLock;
      }
      case osgGA::GUIEventAdapter::KEY_KP_0:
      {
         return CEGUI::Key::Numpad0;
      }
      case osgGA::GUIEventAdapter::KEY_KP_1:
      {
         return CEGUI::Key::Numpad1;
      }
      case osgGA::GUIEventAdapter::KEY_KP_2:
      {
         return CEGUI::Key::Numpad2;
      }
      case osgGA::GUIEventAdapter::KEY_KP_3:
      {
         return CEGUI::Key::Numpad3;
      }
      case osgGA::GUIEventAdapter::KEY_KP_4:
      {
         return CEGUI::Key::Numpad4;
      }
      case osgGA::GUIEventAdapter::KEY_KP_5:
      {
         return CEGUI::Key::Numpad5;
      }
      case osgGA::GUIEventAdapter::KEY_KP_6:
      {
         return CEGUI::Key::Numpad6;
      }
      case osgGA::GUIEventAdapter::KEY_KP_7:
      {
         return CEGUI::Key::Numpad7;
      }
      case osgGA::GUIEventAdapter::KEY_KP_8:
      {
         return CEGUI::Key::Numpad8;
      }
      case osgGA::GUIEventAdapter::KEY_KP_9:
      {
         return CEGUI::Key::Numpad9;
      }
      case osgGA::GUIEventAdapter::KEY_KP_Divide:
      {
         return CEGUI::Key::Divide;
      }
      case osgGA::GUIEventAdapter::KEY_KP_Multiply:
      {
         return CEGUI::Key::Multiply;
      }
      case osgGA::GUIEventAdapter::KEY_KP_Subtract:
      {
         return CEGUI::Key::Subtract;
      }
      case osgGA::GUIEventAdapter::KEY_KP_Add:
      {
         return CEGUI::Key::Add;
      }
      case osgGA::GUIEventAdapter::KEY_KP_Home:
      {
         return CEGUI::Key::Numpad7;
      }
      case osgGA::GUIEventAdapter::KEY_KP_Up:
      {
         return CEGUI::Key::Numpad8;
      }
      case osgGA::GUIEventAdapter::KEY_KP_Page_Up:
      {
         return CEGUI::Key::Numpad9;
      }
      case osgGA::GUIEventAdapter::KEY_KP_Left:
      {
         return CEGUI::Key::Numpad4;
      }
      case osgGA::GUIEventAdapter::KEY_KP_Begin:
      {
         return CEGUI::Key::Scan(0); //?
      }
      case osgGA::GUIEventAdapter::KEY_KP_Right:
      {
         return CEGUI::Key::Numpad6;
      }
      case osgGA::GUIEventAdapter::KEY_KP_End:
      {
         return CEGUI::Key::Numpad1;
      }
      case osgGA::GUIEventAdapter::KEY_KP_Down:
      {
         return CEGUI::Key::Numpad2;
      }
      case osgGA::GUIEventAdapter::KEY_KP_Page_Down:
      {
         return CEGUI::Key::Numpad3;
      }
      case osgGA::GUIEventAdapter::KEY_KP_Insert:
      {
         return CEGUI::Key::Numpad0;
      }
      case osgGA::GUIEventAdapter::KEY_KP_Delete:
      {
         return CEGUI::Key::Decimal;
      }
      case osgGA::GUIEventAdapter::KEY_KP_Enter:
      {
         return CEGUI::Key::NumpadEnter;
      }
      default:
      {
         return CEGUI::Key::Scan(0);
      }
   }
}


////////////////////////////////////////////////////////////////////////////////
void CEGUIKeyboardListener::MakeCurrent()
{
#if CEGUI_VERSION_MAJOR >= 0 && CEGUI_VERSION_MINOR == 6
  if (m_pGUI != NULL)
   {
      m_pGUI->MakeCurrent();
   }
#endif
}

////////////////////////////////////////////////////////////////////////////////

