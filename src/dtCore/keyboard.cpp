// keyboard.cpp: Implementation of the Keyboard class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix.h>
#include <dtCore/keyboard.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/log.h>
#include <algorithm>

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(Keyboard)

Keyboard::Keyboard(const std::string& name) : InputDevice(name)
{
   RegisterInstance(this);

   AddFeature(new Button(this, -1, "Unknown"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Escape       ,"Esc"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_F1           ,"F1"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_F2           ,"F2"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_F3           ,"F3"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_F4           ,"F4"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_F5           ,"F5"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_F6           ,"F6"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_F7           ,"F7"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_F8           ,"F8"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_F9           ,"F9"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_F10          ,"F10"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_F11          ,"F11"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_F12          ,"F12"));
   AddFeature(new Button(this, '`'                                      ,"`"));

   AddFeature(new Button(this, '1'                                      ,"1"));
   AddFeature(new Button(this, '2'                                      ,"2"));
   AddFeature(new Button(this, '3'                                      ,"3"));
   AddFeature(new Button(this, '4'                                      ,"4"));
   AddFeature(new Button(this, '5'                                      ,"5"));
   AddFeature(new Button(this, '6'                                      ,"6"));
   AddFeature(new Button(this, '7'                                      ,"7"));
   AddFeature(new Button(this, '8'                                      ,"8"));
   AddFeature(new Button(this, '9'                                      ,"9"));
   AddFeature(new Button(this, '0'                                      ,"0"));
   AddFeature(new Button(this, '-'                                      ,"-"));
   AddFeature(new Button(this, '='                                      ,"="));

   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_BackSpace    ,"Backspace"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Tab          ,"Tab"));
   AddFeature(new Button(this, 'A'                                      ,"A"));
   AddFeature(new Button(this, 'B'                                      ,"B"));
   AddFeature(new Button(this, 'C'                                      ,"C"));
   AddFeature(new Button(this, 'D'                                      ,"D"));
   AddFeature(new Button(this, 'E'                                      ,"E"));
   AddFeature(new Button(this, 'F'                                      ,"F"));
   AddFeature(new Button(this, 'G'                                      ,"G"));
   AddFeature(new Button(this, 'H'                                      ,"H"));
   AddFeature(new Button(this, 'I'                                      ,"I"));
   AddFeature(new Button(this, 'J'                                      ,"J"));
   AddFeature(new Button(this, 'K'                                      ,"K"));
   AddFeature(new Button(this, 'L'                                      ,"L"));
   AddFeature(new Button(this, 'M'                                      ,"M"));
   AddFeature(new Button(this, 'N'                                      ,"N"));
   AddFeature(new Button(this, 'O'                                      ,"O"));
   AddFeature(new Button(this, 'P'                                      ,"P"));
   AddFeature(new Button(this, 'Q'                                      ,"Q"));
   AddFeature(new Button(this, 'R'                                      ,"R"));
   AddFeature(new Button(this, 'S'                                      ,"S"));
   AddFeature(new Button(this, 'T'                                      ,"T"));
   AddFeature(new Button(this, 'U'                                      ,"U"));
   AddFeature(new Button(this, 'V'                                      ,"V"));
   AddFeature(new Button(this, 'W'                                      ,"W"));
   AddFeature(new Button(this, 'X'                                      ,"X"));
   AddFeature(new Button(this, 'Y'                                      ,"Y"));
   AddFeature(new Button(this, 'Z'                                      ,"Z"));

   AddFeature(new Button(this, 'a'                                      ,"a"));
   AddFeature(new Button(this, 'b'                                      ,"b"));
   AddFeature(new Button(this, 'c'                                      ,"c"));
   AddFeature(new Button(this, 'd'                                      ,"d"));
   AddFeature(new Button(this, 'e'                                      ,"e"));
   AddFeature(new Button(this, 'f'                                      ,"f"));
   AddFeature(new Button(this, 'g'                                      ,"g"));
   AddFeature(new Button(this, 'h'                                      ,"h"));
   AddFeature(new Button(this, 'i'                                      ,"i"));
   AddFeature(new Button(this, 'j'                                      ,"j"));
   AddFeature(new Button(this, 'k'                                      ,"k"));
   AddFeature(new Button(this, 'l'                                      ,"l"));
   AddFeature(new Button(this, 'm'                                      ,"m"));
   AddFeature(new Button(this, 'n'                                      ,"n"));
   AddFeature(new Button(this, 'o'                                      ,"o"));
   AddFeature(new Button(this, 'p'                                      ,"p"));
   AddFeature(new Button(this, 'q'                                      ,"q"));
   AddFeature(new Button(this, 'r'                                      ,"r"));
   AddFeature(new Button(this, 's'                                      ,"s"));
   AddFeature(new Button(this, 't'                                      ,"t"));
   AddFeature(new Button(this, 'u'                                      ,"u"));
   AddFeature(new Button(this, 'v'                                      ,"v"));
   AddFeature(new Button(this, 'w'                                      ,"w"));
   AddFeature(new Button(this, 'x'                                      ,"x"));
   AddFeature(new Button(this, 'y'                                      ,"y"));
   AddFeature(new Button(this, 'z'                                      ,"z"));


   AddFeature(new Button(this, '['                                      ,"["));
   AddFeature(new Button(this, ']'                                      ,"]"));
   AddFeature(new Button(this, '\\'                                     ,"\\"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Caps_Lock    ,"Caps Lock"));
   AddFeature(new Button(this, ';'                                      ,";"));
   AddFeature(new Button(this, '\''                                     ,"\'"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Return       ,"Enter"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Shift_L      ,"Left Shift"));
   AddFeature(new Button(this, ','                                      ,","));
   AddFeature(new Button(this, '.'                                      ,"."));
   AddFeature(new Button(this, '/'                                      ,"/"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Shift_R      ,"Right Shift"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Control_L    ,"Left Ctrl"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Control_R    ,"Right Ctrl"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Space       ,"Space"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Alt_L        ,"Left Alt"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Alt_R        ,"Right Alt"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Super_L      ,"Left Super"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Super_R      ,"Right Super"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Meta_L       ,"Left Meta"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Meta_R       ,"Right Meta"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Menu         ,"Menu"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Print        ,"Print Screen"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Scroll_Lock  ,"Scroll Lock"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Pause        ,"Pause"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Home         ,"Home"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Page_Up      ,"Page Up"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_End          ,"End"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Page_Down    ,"Page Down"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Delete       ,"Delete"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Insert       ,"Insert"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Left         ,"Left"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Up           ,"Up"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Right        ,"Right"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Down         ,"Down"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_Num_Lock     ,"Num Lock"));

   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_KP_Divide    ,"Keypad /"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_KP_Multiply  ,"Keypad *"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_KP_Subtract  ,"Keypad -"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_KP_Add       ,"Keypad +"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_KP_Home      ,"Keypad 7"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_KP_Up        ,"Keypad 8"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_KP_Page_Up   ,"Keypad 9"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_KP_Left      ,"Keypad 4"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_KP_Begin     ,"Keypad 5"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_KP_Right     ,"Keypad 6"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_KP_End       ,"Keypad 1"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_KP_Down      ,"Keypad 2"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_KP_Page_Down ,"Keypad 3"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_KP_Insert    ,"Keypad 0"));
   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_KP_Delete    ,"Keypad ."));
//   AddFeature(new Button(this, osgGA::GUIEventAdapter::KEY_KP,          ,"Keypad Enter")); not defined in OSG
}

Keyboard::~Keyboard()
{
   DeregisterInstance(this);
}

bool Keyboard::GetKeyState(int key) const
{
   const Button* b = GetButton(key);
   if (b == NULL)
   {
      LOG_ERROR("Keyboard does not have a Button assigned to the key value of " + dtUtil::ToString(key) );
      return false;
   }

   return b->GetState();
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

bool Keyboard::KeyDown(int kc)
{
   bool handled(false);

   KeyboardListenerList::iterator iter = mKeyboardListeners.begin();
   KeyboardListenerList::iterator enditer = mKeyboardListeners.end();
   while (!handled && iter != enditer)
   {
      handled = (*iter)->HandleKeyPressed(this, kc);
      ++iter;
   }

   Button* b = GetButton(kc);
   if (b != NULL)
   {
      // Set the button's state
      if (b->SetState(true) && !handled)
      {
         handled = b->NotifyStateChange();
      }
   }

   return handled;
}

bool Keyboard::KeyUp(int kc)
{
   bool handled(false);
   KeyboardListenerList::iterator iter = mKeyboardListeners.begin();
   KeyboardListenerList::iterator enditer = mKeyboardListeners.end();
   while (!handled && iter != enditer)
   {
      handled = (*iter)->HandleKeyReleased(this, kc);
      ++iter;
   }

   Button* b = GetButton(kc);
   if (b != NULL)
   {
      // Set the button's state
      if (b->SetState(false) && !handled)
      {
         handled = b->NotifyStateChange();
      }
   }

   return handled;
}

