// inputdevice.cpp: Implementation of the InputDevice class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/inputdevice.h>
#include <algorithm>

namespace dtCore
{

   IMPLEMENT_MANAGEMENT_LAYER(InputDevice)
   
    InputDevice::InputDevice(const std::string& name) : Base(name)
   {
      RegisterInstance(this);
   }
   
   InputDevice::~InputDevice()
   {
      DeregisterInstance(this);
   }
   
   int InputDevice::GetFeatureCount() const
   {
      return mFeatures.size();
   }
   
   InputDeviceFeature* InputDevice::GetFeature(int index)
   {
      return mFeatures[index].get();
   }
   
   int InputDevice::GetButtonCount() const
   {
      return mButtons.size();
   }
   
   Button* InputDevice::GetButton(int index)
   {
      return mButtons[index].get();
   }
   
   const Button* InputDevice::GetButton(int index) const
   {
      return mButtons[index].get();
   }

   int InputDevice::GetAxisCount() const
   {
      return mAxes.size();
   }
   
   Axis* InputDevice::GetAxis(int index)
   {
      return mAxes[index].get();
   }
   
   const Axis* InputDevice::GetAxis(int index) const
   {
      return mAxes[index].get();
   }

   void InputDevice::AddButtonListener(ButtonListener* buttonListener)
   {
      mButtonListeners.push_back(buttonListener);
   }
   
   void InputDevice::RemoveButtonListener(ButtonListener* buttonListener)
   {
      mButtonListeners.remove(buttonListener);
   }

   void InputDevice::AddAxisListener(AxisListener* axisListener)
   {
      mAxisListeners.push_back(axisListener);
   }
   
   void InputDevice::RemoveAxisListener(AxisListener* axisListener)
   {
      mAxisListeners.remove(axisListener);
   }
   
   /**
    * Adds a feature to this device.
    *
    * @param feature a pointer to the feature to add
    */
   void InputDevice::AddFeature(InputDeviceFeature* feature)
   {
      mFeatures.push_back(feature);
   
      if(Button* button = dynamic_cast<Button*>(feature))
      {
         mButtons.push_back(button);
      }
      
      if(Axis* axis = dynamic_cast<Axis*>(feature))
      {
         mAxes.push_back(axis);
      }
   }
   
   void InputDevice::RemoveFeature(InputDeviceFeature* feature)
   {
      for(std::vector< RefPtr<InputDeviceFeature> >::iterator it = mFeatures.begin();
          it != mFeatures.end();
          it++)
      {
         if((*it).get() == feature)
         {
            mFeatures.erase(it);
   
            if(IS_A(feature, Button*))
            {
               for(std::vector< RefPtr<Button> >::iterator bit = mButtons.begin();
                   bit != mButtons.end();
                   bit++)
               {
                  if((*bit).get() == feature)
                  {
                     mButtons.erase(bit);
                     break;
                  }
               }
            }
   
            if(IS_A(feature, Axis*))
            {
               for(std::vector< RefPtr<Axis> >::iterator ait = mAxes.begin();
                   ait != mAxes.end();
                   ait++)
               {
                  if((*ait).get() == feature)
                  {
                     mAxes.erase(ait);
                     break;
                  }
               }
            }
   
            return;
         }
      }
   }

   InputDeviceFeature::InputDeviceFeature(InputDevice* owner, const std::string& description) :
      mOwner(owner),
      mDescription(description)
   {}
   
   InputDeviceFeature::~InputDeviceFeature()
   {}
   
   InputDevice* InputDeviceFeature::GetOwner() const
   {
      return mOwner;
   }
   
   void InputDeviceFeature::SetDescription(const std::string& description)
   {
      mDescription = description;
   }
   
   std::string InputDeviceFeature::GetDescription() const
   {
      return mDescription;
   }
   
   Button::Button(InputDevice* owner, const std::string& description) :
      InputDeviceFeature(owner, description),
      mState(false)
   {}
   
   bool Button::SetState(bool state)
   {
      bool handled(false);
      if(state != mState)
      {
         mState = state;
   
         ButtonListenerList::iterator it;

         // perform the chain of responsibility
         ButtonListenerList::iterator iter = mButtonListeners.begin();
         ButtonListenerList::iterator enditer = mButtonListeners.end();
         while( !handled && iter!=enditer )
         {
            handled = (*iter)->ButtonStateChanged( this, !mState, mState);
            ++iter;
         }

         ///\todo Grasp why this is here
         for(it = GetOwner()->mButtonListeners.begin();
             it != GetOwner()->mButtonListeners.end();
             it++)
         {
            (*it)->ButtonStateChanged(this, !mState, mState);
         }
      }

      return handled;
   }

   bool Button::GetState() const
   {
      return mState;
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
   
   Axis::Axis(InputDevice* owner, const std::string& description) : 
      InputDeviceFeature(owner, description),
      mState(0.0)
   {}

   bool Axis::SetState(double state, double delta)
   {
      bool handled(false);

      if(state != mState || delta != 0.0)
      {
         double oldState = mState;
         mState = state;

         // perform the chain of responsibility
         AxisListenerList::iterator iter = mAxisListeners.begin();
         AxisListenerList::iterator enditer = mAxisListeners.end();
         while( !handled && iter!=enditer )
         {
            handled = (*iter)->AxisStateChanged(this, oldState, mState, delta);
            ++iter;
         }

         ///\todo Grasp why this is here.
         // update all of the device's listeners?????????????????????
         AxisListenerList::iterator it;
         for(it = GetOwner()->mAxisListeners.begin();
             it != GetOwner()->mAxisListeners.end();
             it++)
         {
            (*it)->AxisStateChanged(this, oldState, mState, delta);
         }
      }

      return handled;
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
