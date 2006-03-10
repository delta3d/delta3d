// inputmapper.cpp: Implementation of the InputMapper class.
//
//////////////////////////////////////////////////////////////////////

#include "dtCore/inputmapper.h"

namespace dtCore
{

   IMPLEMENT_MANAGEMENT_LAYER(InputMapper)
   
   /**
    * Constructor.
    *
    * @param name the instance name
    */
    InputMapper::InputMapper(const std::string& name)
      : Base(name),
        mAcquiringButtonMapping(false),
        mAcquiringAxisMapping(false)
   {
      RegisterInstance(this);
   }
   
   /**
    * Destructor.
    */
   InputMapper::~InputMapper()
   {
      DeregisterInstance(this);
   }
   
   /**
    * Adds an input device to this manager.
    *
    * @param device the device to add
    */
   void InputMapper::AddDevice(InputDevice* device)
   {
      device->AddButtonListener(this);
      device->AddAxisListener(this);
      
      mDevices.push_back(device);
   }
   
   /**
    * Removes an input device from this manager.
    *
    * @param device the device to remove
    */
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
   
   /**
    * Returns the number of devices watched by this mapper.
    *
    * @return the number of devices
    */
   int InputMapper::GetNumDevices()
   {
      return mDevices.size();
   }
   
   /**
    * Returns the device at the specified index
    *
    * @param index the index
    * @return the device at the index
    */
   InputDevice* InputMapper::GetDevice(int index)
   {
      return mDevices[index].get();
   }
   
   /**
    * Sets the cancel button.
    *
    * @param button the cancel button
    */
   void InputMapper::SetCancelButton(Button* button)
   {
      mCancelButton = button;
   }
   
   /**
    * Returns the cancel button.
    *
    * @return the cancel button
    */
   Button* InputMapper::GetCancelButton()
   {
      return mCancelButton.get();
   }
   
   /**
    * Acquires a button mapping.
    *
    * @param callback the callback interface, used to report
    * the result
    * @return true if the acquisition process was initiated, false
    * if an acquisition is already taking place
    */
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
   
   /**
    * Acquires an axis mapping.
    *
    * @param callback the callback interface, used to report
    * the result
    * @return true if the acquisition process was initiated, false
    * if an acquisition is already taking place
    */
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
   
   /**
    * Called when a button's state has changed.
    *
    * @param button the origin of the event
    * @param oldState the old state of the button
    * @param newState the new state of the button
    */
   void InputMapper::ButtonStateChanged(Button* button,
                                        bool oldState,
                                        bool newState)
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
            
            mCallback->ButtonMappingAcquired(
               new ButtonToButton(button)
            );
         }
      }
   }
   
   /**
    * Called when an axis' state has changed.
    *
    * @param axis the changed axis
    * @param oldState the old state of the axis
    * @param newState the new state of the axis
    * @param delta a delta value indicating stateless motion
    */
   void InputMapper::AxisStateChanged(Axis* axis,
                                      double oldState, 
                                      double newState, 
                                      double delta)
   {
      if(mAcquiringAxisMapping)
      {
         mAcquiringAxisMapping = false;
      
         mCallback->AxisMappingAcquired( new AxisToAxis(axis) );
      }
   }
}
