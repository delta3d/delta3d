// inputmapper.cpp: Implementation of the InputMapper class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix.h>
#include "dtCore/inputmapper.h"

namespace dtCore
{

   IMPLEMENT_MANAGEMENT_LAYER(InputMapper)

    InputMapper::InputMapper(const std::string& name)
      : Base(name),
        mAcquiringButtonMapping(false),
        mAcquiringAxisMapping(false)
   {
      RegisterInstance(this);
   }

   InputMapper::~InputMapper()
   {
      DeregisterInstance(this);
   }

   void InputMapper::AddDevice(InputDevice* device)
   {
      device->AddButtonObserver(this);
      device->AddAxisObserver(this);

      mDevices.push_back(device);
   }

   void InputMapper::RemoveDevice(InputDevice* device)
   {
      for(std::vector< RefPtr<InputDevice> >::iterator it = mDevices.begin();
          it != mDevices.end();
          it++)
      {
         if(*it == device)
         {
            device->RemoveButtonObserver(this);
            device->RemoveAxisObserver(this);

            mDevices.erase(it);

            return;
         }
      }
   }

   int InputMapper::GetNumDevices()
   {
      return mDevices.size();
   }

   InputDevice* InputMapper::GetDevice(int index)
   {
      return mDevices[index].get();
   }

   void InputMapper::SetCancelButton(Button* button)
   {
      mCancelButton = button;
   }

   Button* InputMapper::GetCancelButton()
   {
      return mCancelButton.get();
   }

   bool InputMapper::AcquireButtonMapping(InputMapperCallback* callback)
   {
      if(!mAcquiringButtonMapping && !mAcquiringAxisMapping)
      {
         mCallback = callback;
         mAcquiringButtonMapping = true;

         return true;
      }
      else
      {
         return false;
      }
   }

   bool InputMapper::AcquireAxisMapping(InputMapperCallback* callback)
   {
      if(!mAcquiringButtonMapping && !mAcquiringAxisMapping)
      {
         mCallback = callback;
         mAcquiringAxisMapping = true;

         return true;
      }
      else
      {
         return false;
      }
   }

   void InputMapper::OnButtonStateChanged(const Button* button, bool oldState, bool newState)
   {
      if(newState)
      {
         if(button == mCancelButton.get())
         {
            if(mAcquiringButtonMapping)
            {
               mAcquiringButtonMapping = false;

               mCallback->ButtonMappingAcquired(NULL);
            }
            else if(mAcquiringAxisMapping)
            {
               mAcquiringAxisMapping = false;

               mCallback->AxisMappingAcquired(NULL);
            }
         }
         else if(mAcquiringButtonMapping)
         {
            mAcquiringButtonMapping = false;

            // find the first device containing the non const button that is equivalent to this button
            bool button_found(false);
            DeviceVector::iterator deviter = mDevices.begin();
            DeviceVector::iterator devend = mDevices.end();
            while( deviter != devend && !button_found )
            {
               InputDevice::ButtonMap& idbv = (*deviter)->GetButtons();
               InputDevice::ButtonMap::iterator butiter = idbv.begin();
               InputDevice::ButtonMap::iterator butend = idbv.end();
               while( butiter != butend && !button_found )
               {
                  if(button == (*butiter).second.get() )
                  {
                     button_found = true;
                     mCallback->ButtonMappingAcquired(new ButtonToButton( butiter->second.get() ));
                  }

                  butiter++;
               }

               deviter++;
            }
         }
      }
   }

   void InputMapper::OnAxisStateChanged(const Axis* axis, double oldState, double newState, double delta)
   {
      if(mAcquiringAxisMapping)
      {
         mAcquiringAxisMapping = false;

         // find the first device containing the non const axis that is equivalent to this axis
         bool axis_found(false);
         DeviceVector::iterator deviter = mDevices.begin();
         DeviceVector::iterator devend = mDevices.end();
         while( deviter != devend && !axis_found )
         {
            InputDevice::AxisVector& idav = (*deviter)->GetAxes();
            InputDevice::AxisVector::iterator axsiter = idav.begin();
            InputDevice::AxisVector::iterator axsend = idav.end();
            while( axsiter != axsend && !axis_found )
            {
               if(axis == (*axsiter).get() )
               {
                  axis_found = true;
                  mCallback->AxisMappingAcquired(new AxisToAxis( axsiter->get() ));
               }

               axsiter++;
            }

            deviter++;
         }
      }
   }
}
