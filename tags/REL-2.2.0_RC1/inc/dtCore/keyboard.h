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

#ifndef DELTA_KEYBOARD
#define DELTA_KEYBOARD

// keyboard.h: Declaration of the Keyboard class.
//
//////////////////////////////////////////////////////////////////////


#include <string>
#include <list>

#include <osg/Referenced>               // for listener's base class
#include <osgGA/GUIEventAdapter>

#include <dtCore/inputdevice.h>
#include <dtCore/refptr.h>             // for typedef, list member



namespace dtCore
{
   class KeyboardListener;

   /// The model of the keyboard used throughout Delta3D.
   class DT_CORE_EXPORT Keyboard : public InputDevice
   {
      DECLARE_MANAGEMENT_LAYER(Keyboard)

   public:
      typedef std::list<dtCore::RefPtr<KeyboardListener> > KeyboardListenerList;

      /// Registers many Buttons as DeviceFeatures with the base class, InputDevice.
      /// It also registers itself with the instance management layer.
      /// @param name the instance name
      Keyboard(const std::string& name = "keyboard");

   protected:

      /// Deregisters itself with the instance management layer.
      virtual ~Keyboard();

   public:

      /// Checks the state of the specified key.
      /// @param key the key to check
      /// @return true if the key is being held down, false otherwise
      bool GetKeyState(int key) const;

      /// Pushes a listener for keyboard events to the back of the container.
      /// @param keyboardListener the listener to add
      void AddKeyboardListener(KeyboardListener* keyboardListener);

      /// Inserts the listener into the list at a position BEFORE pos.
      void InsertKeyboardListener(const KeyboardListenerList::value_type& pos, KeyboardListener* kbl);

      /// Removes a keyboard listener.
      /// @param keyboardListener the listener to remove from the container of listeners.
      void RemoveKeyboardListener(KeyboardListener* keyboardListener);

      // Producer callback methods. These are KeyDown and KeyUp instead of
      // KeyPress and KeyRelease to avoid a define clash with X11's X.h.
      virtual bool KeyDown(int key);
      virtual bool KeyUp(int key);

      /// @return The container of listeners.
      const KeyboardListenerList& GetListeners() const { return mKeyboardListeners; }

   protected:

      /// The container of keyboard listeners.
      KeyboardListenerList mKeyboardListeners;

   private:
      // the following are not implemented by design,
      // to cause compile errors for users that need to use the new interface.
      bool KeyPressed(int);
      void keyPressed(int);

      bool KeyReleased(int);
      void keyReleased(int);

      // Disallowed to prevent compile errors on VS2003. It apparently
      // creates this functions even if they are not used, and if
      // this class is forward declared, these implicit functions will
      // cause compiler errors for missing calls to "ref".
      Keyboard& operator=( const Keyboard& ); 
      Keyboard( const Keyboard& );
   };

   
   /// An interface for objects interested in receiving keyboard events.
   class DT_CORE_EXPORT KeyboardListener : public osg::Referenced
   {
      protected:
         virtual ~KeyboardListener() {}

      public:

         /// Called when a key is pressed. 
         /// @param keyboard the source of the event
         /// @param key the key pressed
         /// @param character the corresponding character
         /// @return true if this KeyboardListener handled the event. The
         /// Keyboard calling this function is responsbile for using this
         /// return value or not.
         virtual bool HandleKeyPressed(const Keyboard* keyboard, int kc )=0;
         
         
         /// Called when a key is released.
         /// @param keyboard the source of the event
         /// @param key the key released
         /// @return true if this KeyboardListener handled the event. The
         /// Keyboard calling this function is responsbile for using this
         /// return value or not.
         virtual bool HandleKeyReleased(const Keyboard* keyboard, int kc )=0;

         /// Called when a key is typed.
         /// @param keyboard the source of the event
         /// @param key the key typed
         /// @return true if this KeyboardListener handled the event. The
         /// Keyboard calling this function is responsbile for using this
         /// return value or not.
         virtual bool HandleKeyTyped(const Keyboard* keyboard, int kc )=0;
   };
}

#endif // DELTA_KEYBOARD
