// inputmapper.cpp: Implementation of the InputMapper class.
//
//////////////////////////////////////////////////////////////////////

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
      device->AddButtonListener(this);
      device->AddAxisListener(this);
      
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
            device->RemoveButtonListener(this);
            device->RemoveAxisListener(this);
            
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

   bool InputMapper::ButtonStateChanged(const Button* button, bool oldState, bool newState)
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
               InputDevice::ButtonVector& idbv = (*deviter)->GetButtons();
               InputDevice::ButtonVector::iterator butiter = idbv.begin();
               InputDevice::ButtonVector::iterator butend = idbv.end();
               while( butiter != butend && !button_found )
               {
                  if(button == (*butiter).get() )
                  {
                     button_found = true;
                     mCallback->ButtonMappingAcquired(new ButtonToButton( butiter->get() ));
                  }

                  butiter++;
               }

               deviter++;
            }
         }
      }

      return false;
   }

   bool InputMapper::AxisStateChanged(const Axis* axis, double oldState, double newState, double delta)
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

      return false;
   }
}
