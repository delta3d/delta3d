#ifndef DELTA_INPUTMAPPER
#define DELTA_INPUTMAPPER

// inputmapper.h: Declaration of the InputMapper class.
//
//////////////////////////////////////////////////////////////////////


#include <osg/ref_ptr>

#include <OpenThreads/Thread>

#include "dtCore/base.h"
#include "dtCore/inputdevice.h"
#include "dtCore/logicalinputdevice.h"

namespace dtCore
{
   class InputMapperCallback;
   
   
   /**
    * Acquires mappings for logical input device features by listening to
    * a set of devices.
    */
   class DT_EXPORT InputMapper : public Base,
                                 public ButtonListener,
                                 public AxisListener
   {
      DECLARE_MANAGEMENT_LAYER(InputMapper)
      
      
      public:
      
         /**
          * Constructor.
          *
          * @param name the instance name
          */
         InputMapper(std::string name = "InputMapper");
         
         /**
          * Destructor.
          */
         virtual ~InputMapper();
         
         /**
          * Adds an input device to this mapper.
          *
          * @param device the device to add
          */
         void AddDevice(InputDevice* device);
         
         /**
          * Removes an input device from this mapper.
          *
          * @param device the device to remove
          */
         void RemoveDevice(InputDevice* device);
      
         /**
          * Returns the number of devices watched by this mapper.
          *
          * @return the number of devices
          */
         int GetNumDevices();
         
         /**
          * Returns the device at the specified index
          *
          * @param index the index
          * @return the device at the index
          */
         InputDevice* GetDevice(int index);
         
         /**
          * Sets the cancel button.
          *
          * @param button the cancel button
          */
         void SetCancelButton(Button* button);
         
         /**
          * Returns the cancel button.
          *
          * @return the cancel button
          */
         Button* GetCancelButton();
         
         /**
          * Acquires a button mapping.
          *
          * @param callback the callback interface, used to report
          * the result
          * @return true if the acquisition process was initiated, false
          * if an acquisition is already taking place
          */
         bool AcquireButtonMapping(InputMapperCallback* callback);
         
         /**
          * Acquires an axis mapping.
          *
          * @param callback the callback interface, used to report
          * the result
          * @return true if the acquisition process was initiated, false
          * if an acquisition is already taking place
          */
         bool AcquireAxisMapping(InputMapperCallback* callback);
      
      
      protected:
         
         /**
          * Called when a button's state has changed.
          *
          * @param button the origin of the event
          * @param oldState the old state of the button
          * @param newState the new state of the button
          */
         virtual void ButtonStateChanged(Button* button,
                                         bool oldState,
                                         bool newState);
                                         
         /**
          * Called when an axis' state has changed.
          *
          * @param axis the changed axis
          * @param oldState the old state of the axis
          * @param newState the new state of the axis
          * @param delta a delta value indicating stateless motion
          */
         virtual void AxisStateChanged(Axis* axis,
                                       double oldState, 
                                       double newState, 
                                       double delta);
                                       
                                       
      private:
      
         /**
          * The set of devices to watch.
          */
         std::vector< osg::ref_ptr<InputDevice> > mDevices;
         
         /**
          * The cancel button.
          */
         osg::ref_ptr<Button> mCancelButton;
         
         /**
          * The callback interface.
          */
         InputMapperCallback* mCallback;
         
         /**
          * True if acquiring a button mapping.
          */
         bool mAcquiringButtonMapping;
         
         /**
          * True if acquiring an axis mapping.
          */
         bool mAcquiringAxisMapping;
   };
   
   
   /**
    * The input mapper callback interface.
    */
   class DT_EXPORT InputMapperCallback
   {
      public:

         /**
          * Notifies the listener that the button mapping acquisition has
          * completed.
          *
          * @param mapping the newly acquired button mapping, or NULL if
          * the user canceled the acquisition
          */
         virtual void ButtonMappingAcquired(ButtonMapping* mapping) {}
         
         /**
          * Notifies the listener that the axis mapping acquisition has
          * completed.
          *
          * @param mapping the newly acquired axis mapping, or NULL if
          * the user canceled the acquisition
          */
         virtual void AxisMappingAcquired(AxisMapping* mapping) {}
   };
};


#endif // DELTA_INPUTMAPPER
