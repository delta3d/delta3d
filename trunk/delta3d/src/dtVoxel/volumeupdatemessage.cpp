/*
 * volumeupdatemessage.cpp
 *
 *  Created on: Jun 9, 2015
 *      Author: david
 */

#include <dtVoxel/volumeupdatemessage.h>
#include <dtCore/namedarrayparameter.h>

namespace dtVoxel
{
   const dtUtil::RefString VolumeUpdateMessage::PARAM_INDICES_CHANGED("ParamIndicesChanged");

   const dtUtil::RefString VolumeUpdateMessage::PARAM_VALUES_CHANGED("ParamValuesChanged");

   const dtUtil::RefString VolumeUpdateMessage::PARAM_INDICES_DEACTIVATED("ParamIndicesDeactivated");

   VolumeUpdateMessage::VolumeUpdateMessage()
   {
      AddParameter(new dtCore::NamedArrayParameter(PARAM_INDICES_CHANGED));
      AddParameter(new dtCore::NamedArrayParameter(PARAM_VALUES_CHANGED));
      AddParameter(new dtCore::NamedArrayParameter(PARAM_INDICES_DEACTIVATED));
   }

   VolumeUpdateMessage::~VolumeUpdateMessage()
   {

   }


} /* namespace dtVoxel */
