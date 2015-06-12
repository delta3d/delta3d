/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * Curtiss Murphy
 */

#ifndef DELTA_BASEINPUTCOMPONENT
#define DELTA_BASEINPUTCOMPONENT

#include <dtGame/gmcomponent.h>
#include <dtGame/export.h>
#include <dtCore/keyboard.h>
#include <dtCore/mouse.h>
#include <dtCore/observerptr.h>

namespace dtGame
{
   class InputComponentMouseListener;
   class InputComponentKeyboardListener;

   /**
    * This class is a base component that is setup to support keyboard and mouse events.
    * To handle your own keyboard or mouse events in your GM application, simply subclass 
    * this and override whichever event method you want. Typically, you will want to 
    * override the HandleKeyPressed() method. 
    * 
    * If you use this component, be sure to call SetListeners() at some point after 
    * your application is constructed (BaseEntryPoint.OnStartup() for instance)
    */
   class DT_GAME_EXPORT BaseInputComponent : public dtGame::GMComponent
   {
   public:

      static const dtCore::RefPtr<dtCore::SystemComponentType> DEFAULT_TYPE;
      static const std::string DEFAULT_NAME;

      /// Constructor
      BaseInputComponent(dtCore::SystemComponentType& type = *DEFAULT_TYPE);

      /**
       * When this component is added to the GM, it
       * needs to setup the mouse listener and keyboard listener on this input component.
       * Do not call this yourself.
       * If overwriting this method, ensure BaseInputComponent::OnAddedToGM()
       * gets called or else no input will take place.
       */
      virtual void OnAddedToGM();

      /**
       * Called when this component is removed from the GM
       */
      virtual void OnRemovedFromGM();

      /**
       * Gets the mouse listener of this input component. Not typically useful externally.
       * @return mMouseListener
       */
      InputComponentMouseListener* GetMouseListener() { return mMouseListener.get(); }

      /**
       * Gets the keyboard listener of this input component.  Not typically useful externally.
       * @return mKeyboardListener
       */
      InputComponentKeyboardListener* GetKeyboardListener() { return mKeyboardListener.get(); }

      /**
       * KeyboardListener call back- Called when a key is pressed.
       * Override this if you want to handle this listener event.
       * Default handles the Escape key to quit.
       *
       * @param keyboard the source of the event
       * @param key the key pressed
       * @param character the corresponding character
       */
      virtual bool HandleKeyPressed(const dtCore::Keyboard* keyboard, int key);

      /**
       * KeyboardListener call back - Called when a key is released.
       * Override this if you want to handle this listener event.
       * Default does nothing.
       *
       * @param keyboard the source of the event
       * @param key the key released
       * @param character the corresponding character
       * @return true if this KeyboardListener handled the event. The
       * Keyboard calling this function is responsbile for using this
       * return value or not.
       */
      virtual bool HandleKeyReleased(const dtCore::Keyboard* /*keyboard*/, int /*key*/)
      {
         return false;
      }

      /**
       * KeyboardListener call back - Called when a key is typed.
       * Override this if you want to handle this listener event.
       * Default does nothing.
       *
       * @param keyboard the source of the event
       * @param key the key typed
       * @param character the corresponding character
       * @return true if this KeyboardListener handled the event. The
       * Keyboard calling this function is responsbile for using this
       * return value or not.
       */
      virtual bool HandleKeyTyped(const dtCore::Keyboard* /*keyboard*/, int /*key*/)
      {
         return false;
      }

      /**
       * MouseListener call back - Called when a button is pressed.
       * Override this if you want to handle this listener event.
       * Default does nothing.
       *
       * @param mouse the source of the event
       * @param button the button pressed
       * @return true if this MouseListener handled the event. The
       * Mouse calling this function is responsbile for using this
       * return value or not.
       */
      virtual bool HandleButtonPressed(const dtCore::Mouse* /*mouse*/, dtCore::Mouse::MouseButton /*button*/)
      {
         return false;
      }

      /**
       * MouseListener call back - Called when a button is released.
       * Override this if you want to handle this listener event.
       * Default does nothing.
       *
       * @param mouse the source of the event
       * @param button the button released
       * @return true if this MouseListener handled the event. The
       * Mouse calling this function is responsbile for using this
       * return value or not.
       */
      virtual bool HandleButtonReleased(const dtCore::Mouse* /*mouse*/, dtCore::Mouse::MouseButton /*button*/)
      {
         return false;
      }

      /**
       * MouseListener call back - Called when a button is clicked.
       * Override this if you want to handle this listener event.
       * Default does nothing.
       *
       * @param mouse the source of the event
       * @param button the button clicked
       * @param clickCount the click count
       * @return true if this MouseListener handled the event. The
       * Mouse calling this function is responsbile for using this
       * return value or not.
       */
      virtual bool HandleButtonClicked(const dtCore::Mouse* /*mouse*/, dtCore::Mouse::MouseButton /*button*/, int /*clickCount*/)
      {
         return false;
      }

      /**
       * MouseListener call back - Called when the mouse pointer is moved.
       * Override this if you want to handle this listener event.
       * Default does nothing.
       *
       * @param mouse the source of the event
       * @param x the x coordinate
       * @param y the y coordinate
       * @return true if this MouseListener handled the event. The
       * Mouse calling this function is responsbile for using this
       * return value or not.
       */
      virtual bool HandleMouseMoved(const dtCore::Mouse* /*mouse*/, float /*x*/, float /*y*/)
      {
         return false;
      }

      /**
       * MouseListener call back - Called when the mouse pointer is dragged.
       * Override this if you want to handle this listener event.
       * Default does nothing.
       *
       * @param mouse the source of the event
       * @param x the x coordinate
       * @param y the y coordinate
       * @return true if this MouseListener handled the event. The
       * Mouse calling this function is responsbile for using this
       * return value or not.
       */
      virtual bool HandleMouseDragged(const dtCore::Mouse* /*mouse*/, float /*x*/, float /*y*/)
      {
         return false;
      }

      /**
       * MouseListener call back - Called when the mouse pointer is scrolled.
       * Override this if you want to handle this listener event.
       * Default does nothing.
       *
       * @param mouse the source of the event
       * @param delta the scroll delta (+1 for up one, -1 for down one)
       * @return true if this MouseListener handled the event. The
       * Mouse calling this function is responsbile for using this
       * return value or not.
       */
      virtual bool HandleMouseScrolled(const dtCore::Mouse* /*mouse*/, int /*delta*/)
      {
         return false;
      }

      /**
       * Converts a key to the keypad equivalent. This is handy for keyboards that don't have keypads (Apple) and need to
       * simulate it.  It also will convert \ to insert if you need that key.
       * One suggestion on OS X is to use the command key to switch numbers and arrows to the keypad ones, which is the super mod
       * key in the OSG keys struct.
       */
      static void ConvertToKeypadEquivalent(int& key);

   protected:

      /// Destructor
      virtual ~BaseInputComponent() { }

   private:
      dtCore::RefPtr<InputComponentMouseListener> mMouseListener;
      dtCore::RefPtr<InputComponentKeyboardListener> mKeyboardListener;
   };

   /**
    * Implementation of MouseListener.  This is used internally by the BaseInputComponent 
    * for handling mouse events in a Game Manager app using the GameEntryPoint
    *
    * @see BaseInputComponent
    * @see GameEntryPoint
    */
   class DT_GAME_EXPORT InputComponentMouseListener : public dtCore::MouseListener
   {
   public:

      /// Constructor
      InputComponentMouseListener(dtGame::BaseInputComponent& inputComp);

      /// mouse event - passes to the BaseInputComponent
      virtual bool HandleButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button);

      /// mouse event - passes to the BaseInputComponent
      virtual bool HandleButtonReleased(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button);
      /// mouse event - passes to the BaseInputComponent
      virtual bool HandleButtonClicked(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button, int clickCount);

      /// mouse event - passes to the BaseInputComponent
      virtual bool HandleMouseMoved(const dtCore::Mouse* mouse, float x, float y);

      /// mouse event - passes to the BaseInputComponent
      virtual bool HandleMouseDragged(const dtCore::Mouse* mouse, float x, float y);

      /// mouse event - passes to the BaseInputComponent
      virtual bool HandleMouseScrolled(const dtCore::Mouse* mouse, int delta);

   protected:

      /// Destructor
      virtual ~InputComponentMouseListener() { }

   private:

      // This is not a RefPtr to prevent a circular reference with BaseInputComponent.
      dtCore::ObserverPtr<dtGame::BaseInputComponent> mInputComp;
   };


   /**
    * Implementation of KeyboardListener.  This class is used internally by the BaseInputComponent 
    * for handling keyboard events in a Game Manager app using the GameEntryPoint
    *
    * @see BaseInputComponent
    * @see GameEntryPoint
    */
   class DT_GAME_EXPORT InputComponentKeyboardListener : public dtCore::KeyboardListener
   {
   public:

      /// Constructor
      InputComponentKeyboardListener(dtGame::BaseInputComponent &inputComp);

      /// keyboard event - passes to the BaseInputComponent
      virtual bool HandleKeyPressed(const dtCore::Keyboard* keyboard, int key);

      /// keyboard event - passes to the BaseInputComponent
      virtual bool HandleKeyReleased(const dtCore::Keyboard* keyboard, int key);

      /// keyboard event - passes to the BaseInputComponent
      virtual bool HandleKeyTyped(const dtCore::Keyboard* keyboard, int key );

   protected:

      /// Destructor
      virtual ~InputComponentKeyboardListener() { }

   private:

      // This is not a RefPtr to prevent a circular reference with BaseInputComponent.
      dtCore::ObserverPtr<dtGame::BaseInputComponent> mInputComp;
   };

}

#endif

