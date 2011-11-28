/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2005 MOVES Institute
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
*/

#ifndef DELTA_INPUTMAPPER
#define DELTA_INPUTMAPPER

// inputmapper.h: Declaration of the InputMapper class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/base.h>
#include <dtCore/axisobserver.h>
#include <dtCore/buttonobserver.h>

#include <dtCore/logicalinputdevice.h>
#include <dtCore/refptr.h>

#include <OpenThreads/Thread>

namespace dtCore
{
   class InputMapperCallback;

   /**
    * Acquires mappings for logical input device features by listening to
    * a set of devices.
    */
   class DT_CORE_EXPORT InputMapper :  public Base,
                                       public AxisObserver,
                                       public ButtonObserver
   {
      DECLARE_MANAGEMENT_LAYER(InputMapper)

      public:

         /**
          * Constructor.
          *
          * @param name the instance name
          */
         InputMapper(const std::string& name = "InputMapper");

      protected:

         /**
          * Destructor.
          */
         virtual ~InputMapper();

      public:

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
         virtual void OnButtonStateChanged(const Button* button, bool oldState, bool newState);

         /**
          * Called when an axis' state has changed.
          *
          * @param axis the changed axis
          * @param oldState the old state of the axis
          * @param newState the new state of the axis
          * @param delta a delta value indicating stateless motion
          * \todo fix this compile error!
          */
         virtual void OnAxisStateChanged(const Axis* axis,
                                       double oldState,
                                       double newState,
                                       double delta);


      private:

         /**
          * The set of devices to watch.
          */
         typedef std::vector< RefPtr<InputDevice> > DeviceVector;
         DeviceVector mDevices;

         /**
          * The cancel button.
          */
         RefPtr<Button> mCancelButton;

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
   class DT_CORE_EXPORT InputMapperCallback
   {
      public:

         virtual ~InputMapperCallback() {}

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
}


#endif // DELTA_INPUTMAPPER
