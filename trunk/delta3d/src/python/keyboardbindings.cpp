// keyboardbindings.cpp: Keyboard binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/keyboard.h"

using namespace boost::python;
using namespace dtCore;

class KeyboardListenerWrap : public KeyboardListener
{
   public:
   
      KeyboardListenerWrap(PyObject* self)
         : mSelf(self)
      {}
      
      virtual void KeyPressed(Keyboard* keyboard, 
                              Producer::KeyboardKey key,
                              Producer::KeyCharacter character)
      {
         call_method<void>(mSelf, "KeyPressed", keyboard, key, character);
      }
      
      void DefaultKeyPressed(Keyboard* keyboard, 
                             Producer::KeyboardKey key,
                             Producer::KeyCharacter character)
      {
         KeyboardListener::KeyPressed(keyboard, key, character);
      }
                              
      virtual void KeyReleased(Keyboard* keyboard, 
                               Producer::KeyboardKey key,
                               Producer::KeyCharacter character)
      {
         call_method<void>(mSelf, "KeyReleased", keyboard, key, character);
      }
      
      void DefaultKeyReleased(Keyboard* keyboard, 
                              Producer::KeyboardKey key,
                              Producer::KeyCharacter character)
      {
         KeyboardListener::KeyReleased(keyboard, key, character);
      }
      
      virtual void KeyTyped(Keyboard* keyboard, 
                            Producer::KeyboardKey key,
                            Producer::KeyCharacter character)
      {
         call_method<void>(mSelf, "KeyTyped", keyboard, key, character);
      }
      
      void DefaultKeyTyped(Keyboard* keyboard, 
                           Producer::KeyboardKey key,
                           Producer::KeyCharacter character)
      {
         KeyboardListener::KeyTyped(keyboard, key, character);
      }
      
   private:
      
      PyObject* mSelf;
};

void initKeyboardBindings()
{
   Keyboard* (*KeyboardGI1)(int) = &Keyboard::GetInstance;
   Keyboard* (*KeyboardGI2)(std::string) = &Keyboard::GetInstance;

   class_<Keyboard, bases<InputDevice>, dtCore::RefPtr<Keyboard> >("Keyboard", no_init)
      .def("GetInstanceCount", &Keyboard::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", KeyboardGI1, return_internal_reference<>())
      .def("GetInstance", KeyboardGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("GetKeyState", &Keyboard::GetKeyState)
      .def("AddKeyboardListener", &Keyboard::AddKeyboardListener)
      .def("RemoveKeyboardListener", &Keyboard::RemoveKeyboardListener);
      
   class_<KeyboardListener, KeyboardListenerWrap, boost::noncopyable>("KeyboardListener")
      .def("KeyPressed", &KeyboardListener::KeyPressed, &KeyboardListenerWrap::DefaultKeyPressed)
      .def("KeyReleased", &KeyboardListener::KeyReleased, &KeyboardListenerWrap::DefaultKeyReleased)
      .def("KeyTyped", &KeyboardListener::KeyTyped, &KeyboardListenerWrap::DefaultKeyTyped);
}
