// axis.cpp: Implementation of the Axis class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix.h>
#include <dtCore/axis.h>
#include <dtCore/axishandler.h>
#include <dtCore/axisobserver.h>
#include <dtCore/inputdevice.h>
#include <algorithm>

namespace dtCore
{
   Axis::Axis(InputDevice* owner, const std::string& description) :
      InputDeviceFeature(owner, description),
      mState(0.0),
      mPrevState(0.0)
   {}

   bool Axis::SetState(double state, double delta)
   {
      if (state != mState || delta != 0.0)
      {
         mPrevState = mState;
         mState = state;

         // Notify all of our observers
         AxisObserverList::iterator observerIter = mAxisObservers.begin();
         AxisObserverList::iterator observerEnditer = mAxisObservers.end();
         while (observerIter != observerEnditer)
         {
            (*observerIter)->OnAxisStateChanged(this, mPrevState, mState, delta);
            ++observerIter;
         }

         // Notify all owner's axis observers
         if (GetOwner() != NULL)
         {
            AxisObserverList::iterator observerIter = GetOwner()->mAxisObservers.begin();
            AxisObserverList::iterator observerEnditer = GetOwner()->mAxisObservers.end();
            while (observerIter != observerEnditer)
            {
               (*observerIter)->OnAxisStateChanged(this, mPrevState, mState, delta);
               ++observerIter;
            }
         }

         return true;
      }

      return false;
   }

   double Axis::GetState() const
   {
      return mState;
   }

   bool Axis::NotifyStateChange(double delta /*= 0.0*/)
   {
      bool handled = false;

      // perform the chain of responsibility
      AxisHandlerList::iterator handlerIter = mAxisHandlers.begin();
      AxisHandlerList::iterator handlerEnditer = mAxisHandlers.end();
      while (!handled && handlerIter != handlerEnditer)
      {
         handled = (*handlerIter)->HandleAxisStateChanged(this, mPrevState, mState, delta);
         ++handlerIter;
      }

      // Notify owner's axis handlers if this hasn't been handled already
      if (GetOwner() != NULL)
      {
         AxisHandlerList::iterator handlerIter = GetOwner()->mAxisHandlers.begin();
         AxisHandlerList::iterator handlerEnditer = GetOwner()->mAxisHandlers.end();
         while (!handled && handlerIter != handlerEnditer)
         {
            handled = (*handlerIter)->HandleAxisStateChanged(this, mPrevState, mState, delta);
            ++handlerIter;
         }
      }

      return handled;
   }

   void Axis::AddAxisHandler(AxisHandler* axisHandler)
   {
      mAxisHandlers.push_back(axisHandler);
   }

   void Axis::InsertAxisHandler(const AxisHandlerList::value_type& pos, AxisHandler* al)
   {
      AxisHandlerList::iterator iter = std::find(mAxisHandlers.begin(), mAxisHandlers.end(), pos);
      mAxisHandlers.insert(iter, al);
   }

   void Axis::RemoveAxisHandler(AxisHandler* axisHandler)
   {
      mAxisHandlers.remove(axisHandler);
   }

   void Axis::AddAxisListener(AxisHandler* axisHandler)
   {
      DEPRECATE("void Axis::AddAxisListener(AxisHandler*)",
                "void Axis::AddAxisHandler(AxisHandler*)");
      AddAxisHandler(axisHandler);
   }

   void Axis::InsertAxisListener(const AxisHandlerList::value_type& pos, AxisHandler* al)
   {
      DEPRECATE("void Axis::InsertAxisListener(const AxisHandlerList::value_type&, AxisHandler*)",
                "void Axis::InsertAxisHandler(const AxisHandlerList::value_type&, AxisHandler*)");
      InsertAxisHandler(pos, al);
   }

   void Axis::RemoveAxisListener(AxisHandler* axisHandler)
   {
      DEPRECATE("void Axis::RemoveAxisListener(AxisHandler*)",
                "void Axis::RemoveAxisHandler(AxisHandler*)");
      RemoveAxisHandler(axisHandler);
   }

   void Axis::AddAxisObserver(AxisObserver* axisObserver)
   {
      mAxisObservers.push_back(axisObserver);
   }

   void Axis::InsertAxisObserver(const AxisObserverList::value_type& pos, AxisObserver* al)
   {
      AxisObserverList::iterator iter = std::find(mAxisObservers.begin(), mAxisObservers.end(), pos);
      mAxisObservers.insert(iter, al);
   }

   void Axis::RemoveAxisObserver(AxisObserver* axisObserver)
   {
      mAxisObservers.remove(axisObserver);
   }
}
