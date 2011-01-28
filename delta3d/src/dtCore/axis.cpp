// axis.cpp: Implementation of the Axis class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix.h>
#include <dtCore/axis.h>
#include <dtCore/axislistener.h>
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
      AxisListenerList::iterator listenerIter = mAxisListeners.begin();
      AxisListenerList::iterator listenerEnditer = mAxisListeners.end();
      while (!handled && listenerIter != listenerEnditer)
      {
         handled = (*listenerIter)->HandleAxisStateChanged(this, mPrevState, mState, delta);
         ++listenerIter;
      }

      // Notify owner's axis listeners if this hasn't been handled already
      if (GetOwner() != NULL)
      {
         AxisListenerList::iterator listenerIter = GetOwner()->mAxisListeners.begin();
         AxisListenerList::iterator listenerEnditer = GetOwner()->mAxisListeners.end();
         while (!handled && listenerIter != listenerEnditer)
         {
            handled = (*listenerIter)->HandleAxisStateChanged(this, mPrevState, mState, delta);
            ++listenerIter;
         }
      }

      return handled;
   }

   void Axis::AddAxisListener(AxisListener* axisListener)
   {
      mAxisListeners.push_back(axisListener);
   }

   void Axis::InsertAxisListener(const AxisListenerList::value_type& pos, AxisListener* al)
   {
      AxisListenerList::iterator iter = std::find(mAxisListeners.begin(), mAxisListeners.end(), pos);
      mAxisListeners.insert(iter, al);
   }

   void Axis::RemoveAxisListener(AxisListener* axisListener)
   {
      mAxisListeners.remove(axisListener);
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
