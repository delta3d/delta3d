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

#ifndef DELTA_INPUT_ISENSE_TRACKER
#define DELTA_INPUT_ISENSE_TRACKER

// tracker.h: Declaration of the Tracker class.
//
//////////////////////////////////////////////////////////////////////


#include <string>

// Hack to compensate for typedef collision on gcc.
#ifdef Bool
#undef Bool
#endif

#include "isense.h"
#include <dtCore/inputdevice.h>
#include <dtInputISense/export.h>

/** The dtInputIsense namespace contains classes that allow for reading of 
  * Intersense (http://www.isense.com/) tracker devices.
  */
namespace dtInputISense
{
   /**
    * A tracker device.
    */
   class DT_INPUT_ISENSE_EXPORT Tracker : public dtCore::InputDevice
   {
      DECLARE_MANAGEMENT_LAYER(Tracker)

      public:

         /**
          * Creates instances of Tracker corresponding to each
          * connected tracker device.
          */
         static void CreateInstances();

         /**
          * Destroys all Tracker instances.
          */
         static void DestroyInstances();

         /**
          * Polls all Tracker instances.
          */
         static void PollInstances();

         /**
          * Manually polls the state of this tracker, updating
          * all of its features.
          */
         void Poll();


      private:

         /**
          * Constructor.
          *
          * @param name the instance name
          * @param trackerHandle the handle of the tracker device
          */
         Tracker(const std::string& name, ISD_TRACKER_HANDLE trackerHandle);

         /**
          * Destructor.
          */
         virtual ~Tracker();

         /**
          * The InterSense tracker handle.
          */
         ISD_TRACKER_HANDLE mTrackerHandle;
   };
}


#endif // DELTA_INPUT_ISENSE_TRACKER
