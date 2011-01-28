// button.cpp: Implementation of the Button class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix.h>
#include <dtCore/button.h>
#include <dtCore/buttonlistener.h>
#include <dtCore/buttonobserver.h>
#include <dtCore/inputdevice.h>
#include <algorithm> //for std::find

namespace dtCore
{
   Button::Button(InputDevice* owner, const std::string& description)
      : InputDeviceFeature(owner, description)
      , mState(false)
      , mSymbol(-1)
  {}

   Button::Button(InputDevice* owner, int symbol, const std::string& description) :
      InputDeviceFeature(owner, description),
      mState(false),
      mSymbol(symbol)
   {}

   bool Button::SetState(bool state)
   {
      if (state != mState)
      {
         mState = state;

         // Notify all of our observers
         ButtonObserverList::iterator observerIter = mButtonObservers.begin();
         ButtonObserverList::iterator observerEnditer = mButtonObservers.end();
         while (observerIter != observerEnditer)
         {
            (*observerIter)->OnButtonStateChanged(this, !mState, mState);
            ++observerIter;
         }

         // Notify all owner's axis observers
         if (GetOwner() != NULL)
         {
            ButtonObserverList::iterator observerIter = GetOwner()->mButtonObservers.begin();
            ButtonObserverList::iterator observerEnditer = GetOwner()->mButtonObservers.end();
            while (observerIter != observerEnditer)
            {
               (*observerIter)->OnButtonStateChanged(this, !mState, mState);
               ++observerIter;
            }
         }

         return true;
      }

      return false;
   }

   bool Button::GetState() const
   {
      return mState;
   }

   bool Button::NotifyStateChange()
   {
      bool handled = false;

      // perform the chain of responsibility
      ButtonListenerList::iterator listenerIter = mButtonListeners.begin();
      ButtonListenerList::iterator listenerEnditer = mButtonListeners.end();
      while (!handled && listenerIter != listenerEnditer)
      {
         handled = (*listenerIter)->HandleButtonStateChanged(this, !mState, mState);
         ++listenerIter;
      }

      // Notify owner's button listeners if this hasn't been handled already
      if (GetOwner() != NULL)
      {
         ButtonListenerList::iterator listenerIter = GetOwner()->mButtonListeners.begin();
         ButtonListenerList::iterator listenerEnditer = GetOwner()->mButtonListeners.end();
         while (!handled && listenerIter != listenerEnditer)
         {
            handled = (*listenerIter)->HandleButtonStateChanged(this, !mState, mState);
            ++listenerIter;
         }
      }

      return handled;
   }

   int Button::GetSymbol() const
   {
      return mSymbol;
   }

   void Button::AddButtonListener(ButtonListener* buttonListener)
   {
      mButtonListeners.push_back(buttonListener);
   }

   void Button::InsertButtonListener(const ButtonListenerList::value_type& pos, ButtonListener* bl)
   {
      ButtonListenerList::iterator iter = std::find(mButtonListeners.begin(), mButtonListeners.end(), pos);
      mButtonListeners.insert(iter,bl);
   }

   void Button::RemoveButtonListener(ButtonListener* buttonListener)
   {
      mButtonListeners.remove(buttonListener);
   }

   void Button::AddButtonObserver(ButtonObserver* buttonObserver)
   {
      mButtonObservers.push_back(buttonObserver);
   }

   void Button::InsertButtonObserver(const ButtonObserverList::value_type& pos, ButtonObserver* bl)
   {
      ButtonObserverList::iterator iter = std::find(mButtonObservers.begin(), mButtonObservers.end(), pos);
      mButtonObservers.insert(iter, bl);
   }

   void Button::RemoveButtonObserver(ButtonObserver* buttonObserver)
   {
      mButtonObservers.remove(buttonObserver);
   }
}
