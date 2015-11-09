// inputdevice.cpp: Implementation of the InputDevice class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix.h>
#include <dtCore/inputdevice.h>

#include <dtCore/axis.h>
#include <dtCore/button.h>

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
      ButtonMap::iterator it = mButtons.find(index);
      if ( it == mButtons.end() )
      {
         return NULL;
      }
      else
      {
         return it->second.get();
      }
   }

   const Button* InputDevice::GetButton(int index) const
   {
      ButtonMap::const_iterator it = mButtons.find(index);
      if (it == mButtons.end() )
      {
         return NULL;
      }
      else
      {
         return it->second.get();
      }
   }

   int InputDevice::GetButtonIndex(const Button* button) const
   {
      if (button->GetOwner() != this)
      {
         return -1;
      }

      for (ButtonMap::const_iterator i = mButtons.begin(); i != mButtons.end(); ++i)
      {
         std::pair< int, dtCore::RefPtr<dtCore::Button> > idButton = *i;
         if (idButton.second.get() == button)
         {
            return idButton.first;
         }
      }

      return -1;
   }

   int InputDevice::GetAxisCount() const
   {
      return mAxes.size();
   }

   Axis* InputDevice::GetAxis(int index)
   {
      if ( mAxes.empty() || unsigned(index) > mAxes.size()-1U )
      {
         return 0;
      }
      else
      {
         return mAxes[index].get();
      }
   }

   const Axis* InputDevice::GetAxis(int index) const
   {
      if ( mAxes.empty() || unsigned(index) > mAxes.size()-1U )
      {
         return 0;
      }
      else
      {
         return mAxes[index].get();
      }
   }

   int InputDevice::GetAxisIndex(const Axis* axis) const
   {
      for (int i = 0; i < GetAxisCount(); ++i)
      {
         if (GetAxis(i) == axis)
         {
            return i;
         }
      }
      return -1;
   }

   void InputDevice::AddButtonHandler(ButtonHandler* buttonHandler)
   {
      mButtonHandlers.push_back(buttonHandler);
   }

   void InputDevice::RemoveButtonHandler(ButtonHandler* buttonHandler)
   {
      mButtonHandlers.remove(buttonHandler);
   }

   void InputDevice::AddAxisHandler(AxisHandler* axisHandler)
   {
      mAxisHandlers.push_back(axisHandler);
   }

   void InputDevice::RemoveAxisHandler(AxisHandler* axisHandler)
   {
      mAxisHandlers.remove(axisHandler);
   }

   void InputDevice::AddButtonListener(ButtonHandler* buttonHandler)
   {
      DEPRECATE("void InputDevice::AddButtonListener(ButtonHandler*)",
         "void InputDevice::AddButtonHandler(ButtonHandler*)");
      AddButtonHandler(buttonHandler);
   }

   void InputDevice::RemoveButtonListener(ButtonHandler* buttonHandler)
   {
      DEPRECATE("void InputDevice::RemoveButtonListener(ButtonHandler*)",
         "void InputDevice::RemoveButtonHandler(ButtonHandler*)");
      RemoveButtonHandler(buttonHandler);
   }

   void InputDevice::AddAxisListener(AxisHandler* axisHandler)
   {
      DEPRECATE("void InputDevice::AddAxisListener(AxisHandler*)",
         "void InputDevice::AddAxisHandler(AxisHandler*)");
      AddAxisHandler(axisHandler);
   }

   void InputDevice::RemoveAxisListener(AxisHandler* axisHandler)
   {
      DEPRECATE("void InputDevice::RemoveAxisListener(AxisHandler*)",
         "void InputDevice::RemoveAxisHandler(AxisHandler*)");
      RemoveAxisHandler(axisHandler);
   }

   void InputDevice::AddButtonObserver(ButtonObserver* buttonObserver)
   {
      mButtonObservers.push_back(buttonObserver);
   }

   void InputDevice::RemoveButtonObserver(ButtonObserver* buttonObserver)
   {
      mButtonObservers.remove(buttonObserver);
   }

   void InputDevice::AddAxisObserver(AxisObserver* axisObserver)
   {
      mAxisObservers.push_back(axisObserver);
   }

   void InputDevice::RemoveAxisObserver(AxisObserver* axisObserver)
   {
      mAxisObservers.remove(axisObserver);
   }

   /**
    * Adds a feature to this device.
    *
    * @param feature a pointer to the feature to add
    */
   bool InputDevice::AddFeature( InputDeviceFeature* feature )
   {

      if (Button* button = dynamic_cast<Button*>(feature))
      {
         //see if a button with the same symbol already exists
         if (GetButton(button->GetSymbol()) != NULL)
         {
            return false;
         }

         mButtons[button->GetSymbol()] = button;
      }

      if (Axis* axis = dynamic_cast<Axis*>(feature))
      {
         mAxes.push_back(axis);
      }

      mFeatures.push_back(feature);
      return true;
   }

   void InputDevice::RemoveFeature(InputDeviceFeature* feature)
   {
      for (FeatureVector::iterator it = mFeatures.begin();
          it != mFeatures.end();
          ++it)
      {
         if ((*it).get() == feature)
         {
            mFeatures.erase(it);

            if (IS_A(feature, Button*))
            {
               for (ButtonMap::iterator bit = mButtons.begin();
                   bit != mButtons.end();
                   ++bit)
               {
                  if ((*bit).second.get() == feature)
                  {
                     mButtons.erase(bit);
                     break;
                  }
               }
            }

            if (IS_A(feature, Axis*))
            {
               for (AxisVector::iterator ait = mAxes.begin();
                   ait != mAxes.end();
                   ++ait)
               {
                  if ((*ait).get() == feature)
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
}
