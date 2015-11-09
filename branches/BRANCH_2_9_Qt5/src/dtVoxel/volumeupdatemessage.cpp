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

   const dtUtil::RefString VolumeUpdateMessage::PARAM_ARRAY_ITEM("x");

   VolumeUpdateMessage::VolumeUpdateMessage()
   : mIndicesChanged(new dtCore::NamedArrayParameter(PARAM_INDICES_CHANGED))
   , mValuesChanged(new dtCore::NamedArrayParameter(PARAM_VALUES_CHANGED))
   , mIndicesDeactivated(new dtCore::NamedArrayParameter(PARAM_INDICES_DEACTIVATED))
   {
      AddParameter(mIndicesChanged);
      AddParameter(mValuesChanged);
      AddParameter(mIndicesDeactivated);
   }

   VolumeUpdateMessage::~VolumeUpdateMessage()
   {

   }

   void VolumeUpdateMessage::AddDeactivatedIndex(const osg::Vec3& idx)
   {
      GetIndicesDeactivated()->AddParameter(*new dtCore::NamedVec3Parameter(PARAM_ARRAY_ITEM, idx));
   }

   const VolumeUpdateMessage::ArrayT* VolumeUpdateMessage::GetIndicesChanged() const
   {
      return mIndicesChanged;
   }
   const VolumeUpdateMessage::ArrayT* VolumeUpdateMessage::GetValuesChanged() const
   {
      return mValuesChanged;
   }
   const VolumeUpdateMessage::ArrayT* VolumeUpdateMessage::GetIndicesDeactivated() const
   {
      return mIndicesDeactivated;
   }

   VolumeUpdateMessage::ArrayT* VolumeUpdateMessage::GetIndicesChanged()
   {
      return mIndicesChanged;
   }
   VolumeUpdateMessage::ArrayT* VolumeUpdateMessage::GetValuesChanged()
   {
      return mValuesChanged;
   }
   VolumeUpdateMessage::ArrayT* VolumeUpdateMessage::GetIndicesDeactivated()
   {
      return mIndicesDeactivated;
   }



} /* namespace dtVoxel */
