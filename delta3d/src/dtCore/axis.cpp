// axis.cpp: Implementation of the Axis class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix.h>
#include <dtCore/axis.h>
#include <dtCore/axislistener.h>
#include <dtCore/inputdevice.h>
#include <algorithm>

namespace dtCore
{
   Axis::Axis(InputDevice* owner, const std::string& description) :
      InputDeviceFeature(owner, description),
      mState(0.0)
   {}

   bool Axis::SetState(double state, double delta, bool handled /*= false*/)
   {
      bool axisHandled(false);

      if (state != mState || delta != 0.0)
      {
         double oldState = mState;
         mState = state;

         // perform the chain of responsibility
         AxisListenerList::iterator iter = mAxisListeners.begin();
         AxisListenerList::iterator enditer = mAxisListeners.end();
         while (!axisHandled && iter != enditer)
         {
            axisHandled = (*iter)->HandleAxisStateChanged(this, oldState, mState, delta);
            ++iter;
         }

         // Notify owner's axis listeners if this hasn't been handled already
         if (GetOwner() != NULL)
         {
            AxisListenerList::iterator iter = GetOwner()->mAxisListeners.begin();
            AxisListenerList::iterator enditer = GetOwner()->mAxisListeners.end();
            while (/*!handled && */iter != enditer)
            {
               (*iter)->HandleAxisStateChanged(this, oldState, mState, delta);
               ++iter;
            }
         }
      }

      return handled || axisHandled;
   }

   double Axis::GetState() const
   {
      return mState;
   }

   void Axis::AddAxisListener(AxisListener* axisListener)
   {
      mAxisListeners.push_back(axisListener);
   }

   void Axis::InsertAxisListener(const AxisListenerList::value_type& pos, AxisListener* al)
   {
      AxisListenerList::iterator iter = std::find( mAxisListeners.begin() , mAxisListeners.end() , pos );
      mAxisListeners.insert(iter,al);
   }

   void Axis::RemoveAxisListener(AxisListener* axisListener)
   {
      mAxisListeners.remove(axisListener);
   }
}
