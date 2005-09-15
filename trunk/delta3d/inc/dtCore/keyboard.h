/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
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

#ifndef DELTA_KEYBOARD
#define DELTA_KEYBOARD

// keyboard.h: Declaration of the Keyboard class.
//
//////////////////////////////////////////////////////////////////////


#include <set>
#include <string>

#include "Producer/KeyboardMouse"
#include "dtCore/inputdevice.h"


namespace dtCore
{
   class KeyboardListener;


   /**
    * The interface to the keyboard
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
         ///Producer callback methods
         virtual void keyRelease( Producer::KeyCharacter );
         ///Producer callback methods
         virtual void specialKeyPress( Producer::KeyCharacter );
         ///Producer callback methods
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

         virtual ~KeyboardListener() {}

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
          * @param character the corresponding character
         */
         virtual void KeyReleased(Keyboard* keyboard, 
                                  Producer::KeyboardKey key,
                                  Producer::KeyCharacter character) {}

         /**
          * Called when a key is typed.
          *
          * @param keyboard the source of the event
          * @param key the key typed
          * @param character the corresponding character
          */
         virtual void KeyTyped(Keyboard* keyboard, 
                               Producer::KeyboardKey key,
                               Producer::KeyCharacter character) {}
   };
};


#endif // DELTA_KEYBOARD
