// button.cpp: Implementation of the Button class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix.h>
#include <dtCore/button.h>
#include <dtCore/buttonhandler.h>
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
      ButtonHandlerList::iterator handlerIter = mButtonHandlers.begin();
      ButtonHandlerList::iterator handlerEnditer = mButtonHandlers.end();
      while (!handled && handlerIter != handlerEnditer)
      {
         handled = (*handlerIter)->HandleButtonStateChanged(this, !mState, mState);
         ++handlerIter;
      }

      // Notify owner's button handlers if this hasn't been handled already
      if (GetOwner() != NULL)
      {
         ButtonHandlerList::iterator handlerIter = GetOwner()->mButtonHandlers.begin();
         ButtonHandlerList::iterator handlerEnditer = GetOwner()->mButtonHandlers.end();
         while (!handled && handlerIter != handlerEnditer)
         {
            handled = (*handlerIter)->HandleButtonStateChanged(this, !mState, mState);
            ++handlerIter;
         }
      }

      return handled;
   }

   int Button::GetSymbol() const
   {
      return mSymbol;
   }

   void Button::AddButtonHandler(ButtonHandler* buttonHandler)
   {
      mButtonHandlers.push_back(buttonHandler);
   }

   void Button::InsertButtonHandler(const ButtonHandlerList::value_type& pos, ButtonHandler* bl)
   {
      ButtonHandlerList::iterator iter = std::find(mButtonHandlers.begin(), mButtonHandlers.end(), pos);
      mButtonHandlers.insert(iter,bl);
   }

   void Button::RemoveButtonHandler(ButtonHandler* buttonHandler)
   {
      mButtonHandlers.remove(buttonHandler);
   }

   void Button::AddButtonListener(ButtonHandler* buttonHandler)
   {
      DEPRECATE("void Button::AddButtonListener(ButtonHandler*)",
         "void Button::AddButtonHandler(ButtonHandler*)");
      AddButtonHandler(buttonHandler);
   }

   void Button::InsertButtonListener(const ButtonHandlerList::value_type& pos, ButtonHandler* bl)
   {
      DEPRECATE("void Button::InsertButtonListener(const ButtonHandlerList::value_type&, ButtonHandler*)",
         "void Button::InsertButtonHandler(const ButtonHandlerList::value_type&, ButtonHandler*)");
      InsertButtonHandler(pos, bl);
   }

   void Button::RemoveButtonListener(ButtonHandler* buttonHandler)
   {
      DEPRECATE("void Button::RemoveButtonListener(ButtonHandler*)",
         "void Button::RemoveButtonHandler(ButtonHandler*)");
      RemoveButtonHandler(buttonHandler);
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
