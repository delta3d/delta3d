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

   void VolumeUpdateMessage::AddDeactivatedIndex(const osg::Vec3& idx)
   {
      GetIndicesDeactivated()->AddParameter(*new dtCore::NamedVec3Parameter("x", idx));
   }

   const VolumeUpdateMessage::ArrayT* VolumeUpdateMessage::GetIndicesChanged() const
   {
      return static_cast<const VolumeUpdateMessage::ArrayT*>(GetParameter(VolumeUpdateMessage::PARAM_INDICES_CHANGED));
   }
   const VolumeUpdateMessage::ArrayT* VolumeUpdateMessage::GetValuesChanged() const
   {
      return static_cast<const VolumeUpdateMessage::ArrayT*>(GetParameter(VolumeUpdateMessage::PARAM_VALUES_CHANGED));
   }
   const VolumeUpdateMessage::ArrayT* VolumeUpdateMessage::GetIndicesDeactivated() const
   {
      return static_cast<const VolumeUpdateMessage::ArrayT*>(GetParameter(VolumeUpdateMessage::PARAM_INDICES_DEACTIVATED));
   }

   VolumeUpdateMessage::ArrayT* VolumeUpdateMessage::GetIndicesChanged()
   {
      return static_cast<VolumeUpdateMessage::ArrayT*>(GetParameter(VolumeUpdateMessage::PARAM_INDICES_CHANGED));
   }
   VolumeUpdateMessage::ArrayT* VolumeUpdateMessage::GetValuesChanged()
   {
      return static_cast<VolumeUpdateMessage::ArrayT*>(GetParameter(VolumeUpdateMessage::PARAM_VALUES_CHANGED));
   }
   VolumeUpdateMessage::ArrayT* VolumeUpdateMessage::GetIndicesDeactivated()
   {
      return static_cast<VolumeUpdateMessage::ArrayT*>(GetParameter(VolumeUpdateMessage::PARAM_INDICES_DEACTIVATED));
   }



} /* namespace dtVoxel */
