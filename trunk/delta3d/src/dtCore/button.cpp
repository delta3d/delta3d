// button.cpp: Implementation of the Button class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix.h>
#include <dtCore/button.h>
#include <dtCore/buttonlistener.h>
#include <dtCore/inputdevice.h>

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

   bool Button::SetState(bool state, bool handled /*= false*/)
   {
      bool buttonHandled(false);

      if (state != mState)
      {
         mState = state;

         ButtonListenerList::iterator it;

         // perform the chain of responsibility
         ButtonListenerList::iterator iter = mButtonListeners.begin();
         ButtonListenerList::iterator enditer = mButtonListeners.end();
         while (!buttonHandled && iter != enditer)
         {
            buttonHandled = (*iter)->HandleButtonStateChanged(this, !mState, mState);
            ++iter;
         }

         // Notify owner's button listeners if this hasn't been handled already
         if (GetOwner() != NULL)
         {
            ButtonListenerList::iterator iter = GetOwner()->mButtonListeners.begin();
            ButtonListenerList::iterator enditer = GetOwner()->mButtonListeners.end();
            while (/*!handled && */iter != enditer)
            {
               (*iter)->HandleButtonStateChanged(this, !mState, mState);
               ++iter;
            }
         }
      }

      return handled || buttonHandled;
   }

   bool Button::GetState() const
   {
      return mState;
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
      ButtonListenerList::iterator iter = std::find( mButtonListeners.begin() , mButtonListeners.end() , pos );
      mButtonListeners.insert(iter,bl);
   }

   void Button::RemoveButtonListener(ButtonListener* buttonListener)
   {
      mButtonListeners.remove(buttonListener);
   }
}
