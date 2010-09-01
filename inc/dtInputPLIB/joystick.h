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

#ifndef DELTA_INPUT_PLIB
#define DELTA_INPUT_PLIB

// joystick.h: Declaration of the Joystick class.
//
//////////////////////////////////////////////////////////////////////

#include <string>
#include <dtInputPLIB/export.h>
#include <dtCore/inputdevice.h>

//forward declaration
class jsJoystick;


/** The dtInputPLIB namespace contains classes used to read input devices
  * using the PLIB library (http://plib.sourceforge.net/)
  */
namespace dtInputPLIB
{
   /**
    * A joystick device.
    */
   class DT_INPUT_PLIB_EXPORT Joystick : public dtCore::InputDevice
   {
      DECLARE_MANAGEMENT_LAYER(Joystick)

      public:

         /**
          * Creates instances of Joystick corresponding to each
          * connected joystick device.
          */
         static void CreateInstances();

         /**
          * Destroys all Joystick instances.
          */
         static void DestroyInstances();

         /**
          * Polls all Joystick instances.
          */
         static void PollInstances();

         /**
          * Returns the device name from PLIB.
          */
         const std::string GetDeviceName() const;

         /**
          * Manually polls the state of this joystick, updating
          * all of its features.
          */
         void Poll();


      private:

         /**
          * Constructor.
          *
          * @param name the instance name
          * @param joystick the underlying PLIB joystick object
          */
         Joystick(std::string name, jsJoystick* joystick);

         /**
          * Destructor.
          */
         virtual ~Joystick();

         /**
          * The underlying PLIB joystick object.
          */
         jsJoystick* mJoystick;
   };
}


#endif // DELTA_INPUT_PLIB
