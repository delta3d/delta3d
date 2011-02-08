/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2011 MOVES Institute
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

#ifndef button_h__
#define button_h__

// button.h: Declaration of the Button class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/export.h>
#include <dtCore/inputdevicefeature.h>
#include <list>

namespace dtCore
{
   class ButtonListener;
   class ButtonObserver;

   /// Buttons are features with binary state.
   class DT_CORE_EXPORT Button : public InputDeviceFeature
   {
   public:
      typedef std::list<ButtonListener*> ButtonListenerList;  ///< A container of ButtonListeners
      typedef std::list<ButtonObserver*> ButtonObserverList;  ///< A container of ButtonObservers

      /**
       * Constructor.
       *
       * @param owner the owner of this button
       * @param description a description of this button
       */
      Button(InputDevice* owner, const std::string& description);

      /**
       * Constructor.
       *
       * @param owner the owner of this button
       * @param symbole the symbole of this button
       * @param description a description of this button
       */
      Button(InputDevice* owner, int symbole, const std::string& description);

   protected:

      virtual ~Button() {}

   public:
      /// Sets the state of this button.
      /// @param state the new state
      /// @return The result of the listeners
      bool SetState(bool state);

      /**
       * Returns the current state of this button.
       *
       * @return true if this button is pressed, false
       * otherwise
       */
      bool GetState() const;

      /**
       * Notifies all the button listeners of a state change
       * This needs to be called anytime a button's state initially
       * gets changed (i.e. from the Mouse, Keyboard, Joystick, etc.)
       * as opposed to when it changes from a mapping (AxisToAxis,
       * ButtonToButton, etc)
       *
       * @return whether the state change was handled or not
       */
      bool NotifyStateChange();

      /**
       * Returns the symbole of this button.
       */
      int GetSymbol() const;

      /**
       * Adds a button listener.
       *
       * @param buttonListener a pointer to the listener to add
       */
      void AddButtonListener(ButtonListener* buttonListener);

      /// Inserts the listener into the list at a position BEFORE pos.
      void InsertButtonListener(const ButtonListenerList::value_type& pos, ButtonListener* bl);

      /**
       * Removes a button listener.
       *
       * @param buttonListener a pointer to the listener to remove
       */
      void RemoveButtonListener(ButtonListener* buttonListener);

      const ButtonListenerList& GetListeners() const { return mButtonListeners; }

      /**
       * Adds a button observer.
       *
       * @param buttonObserver a pointer to the observer to add
       */
      void AddButtonObserver(ButtonObserver* buttonObserver);

      /// Inserts the observer into the list at a position BEFORE pos.
      void InsertButtonObserver(const ButtonObserverList::value_type& pos, ButtonObserver* bl);

      /**
       * Removes a button observer.
       *
       * @param buttonObserver a pointer to the observer to remove
       */
      void RemoveButtonObserver(ButtonObserver* buttonObserver);

      const ButtonObserverList& GetObservers() const { return mButtonObservers; }

   private:
      bool mState;  ///< The state of this button.
      int mSymbol;  ///< The symbol of this button.
      ButtonListenerList mButtonListeners;  ///< Listeners to this button.
      ButtonObserverList mButtonObservers;  ///< Observers to this button.
   };
}

#endif // button_h__
