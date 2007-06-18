/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 John K. Grant
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
 * John K. Grant, April 2007.
 */

#ifndef __DELTA_DTDIS_VALUE_MAPS_H__
#define __DELTA_DTDIS_VALUE_MAPS_H__

#include <dtDIS/dtdisexport.h>      // for export symbols
#include <string>                   // for parameter type

///@cond DOXYGEN_SHOULD_SKIP_THIS
namespace DIS
{
   class EntityType;
}
///@endcond

namespace dtDIS
{
   /// a name scope for all sorts of mappings
   struct DT_DIS_EXPORT ValueMap
   {
      /// find the supported node name for the part type identifier.
      /// @param parttype the part identifier in the DIS::ArticulationParameter
      /// @param nodename the string value of the scenegraph node to be searched when performing articulation.
      /// @return true if parttype is supported, false if it is not suppported.
      static bool GetArticulationNodeName(unsigned int parttype, std::string& nodename);

      /// find the actor property name value for the supported motiontype associated with articulation.
      /// @param motiontype the DIS standard type of motion.
      /// @param propertyname the corresponding name of the property for the motiontype.
      /// @return true if the motiontype is supported
      static bool GetArticulationMotionPropertyName(unsigned int motiontype, std::string& propertyname);

      /// maps the provided dead reckoning model to the supported Delta3D property value.
      /// @param drm the dead reckoning model.
      /// @param property the value to be assigned which is mapped to the drm.
      /// @return true if the value of drm is supported.
      static bool GetDeadReckoningModelPropertyValue(unsigned char drm, std::string& property);

      /// informs clients if the EntityType should support ground clamping.
      /// @param requires modified to true when the EntityType should be ground clamped.
      /// @param etype the EntityType interested to know if ground clamping is desired.
      /// @return true if there exists a known interpretation of the EntityType
      static bool GetRequiresGroundClamping(const DIS::EntityType& etype, bool& requires);
   };
}

#endif // __DELTA_DTDIS_VALUE_MAPS_H__
