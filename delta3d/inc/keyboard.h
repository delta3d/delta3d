// keyboard.h: Declaration of the Keyboard class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DELTA_KEYBOARD
#define DELTA_KEYBOARD

#include <set>
#include <string>

#include "Producer/KeyboardMouse"

#include "inputdevice.h"


namespace dtCore
{
   class KeyboardListener;


   /**
    * A keyboard device.
    */
   class DT_EXPORT Keyboard : public InputDevice
   {
      DECLARE_MANAGEMENT_LAYER(Keyboard)


      public:

         /**
          * Constructor.
          *
          * @param name the instance name
          */
         Keyboard(std::string name = "keyboard");

         /**
          * Destructor.
          */
         virtual ~Keyboard();

         /**
          * Checks the state of the specified key.
          *
          * @param key the key to check
          * @return true if the key is being held down, false
          * otherwise
          */
         bool GetKeyState(Producer::KeyboardKey key);

         /**
          * Adds a listener for keyboard events.
          *
          * @param keyboardListener the listener to add
          */
         void AddKeyboardListener(KeyboardListener* keyboardListener);

         /**
          * Removes a keyboard listener.
          *
          * @param keyboardListener the listener to remove
          */
         void RemoveKeyboardListener(KeyboardListener* keyboardListener);


         ///Producer callback methods
         virtual void keyPress( Producer::KeyCharacter );
         virtual void keyRelease( Producer::KeyCharacter );
         virtual void specialKeyPress( Producer::KeyCharacter );
         virtual void specialKeyRelease( Producer::KeyCharacter );


      private:

         /**
          * The set of keyboard listeners.
          */
         std::set<KeyboardListener*> keyboardListeners;
   };

   
   /**
    * An interface for objects interested in receiving keyboard events.
    */
   class DT_EXPORT KeyboardListener
   {
      public:

         /**
          * Called when a key is pressed.
          *
          * @param keyboard the source of the event
          * @param key the key pressed
          * @param character the corresponding character
          */
         virtual void KeyPressed(Keyboard* keyboard, 
                                 Producer::KeyboardKey key,
                                 Producer::KeyCharacter character) {}

         /**
          * Called when a key is released.
          *
          * @param keyboard the source of the event
          * @param key the key released
          */
         virtual void KeyReleased(Keyboard* keyboard, 
                                  Producer::KeyboardKey key,
                                  Producer::KeyCharacter character) {}

         /**
          * Called when a key is typed.
          *
          * @param keyboard the source of the event
          * @param key the key typed
          */
         virtual void KeyTyped(Keyboard* keyboard, 
                               Producer::KeyboardKey key,
                               Producer::KeyCharacter character) {}
   };
};

#endif // DELTA_KEYBOARD
