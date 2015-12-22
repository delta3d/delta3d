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

   const dtUtil::RefString VolumeUpdateMessage::PARAM_ARRAY_ITEM("x");

   VolumeUpdateMessage::VolumeUpdateMessage()
   : mIndicesChanged(new dtCore::NamedArrayParameter(PARAM_INDICES_CHANGED))
   , mValuesChanged(new dtCore::NamedArrayParameter(PARAM_VALUES_CHANGED))
   {
      AddParameter(mIndicesChanged);
      AddParameter(mValuesChanged);
   }

   VolumeUpdateMessage::~VolumeUpdateMessage() {}

   void VolumeUpdateMessage::AddChangedValue(const osg::Vec3& idx, dtCore::NamedParameter& np)
   {
      GetIndicesChanged()->AddParameter(*new dtCore::NamedVec3Parameter(PARAM_ARRAY_ITEM, idx));
      GetValuesChanged()->AddParameter(np);
   }

   void VolumeUpdateMessage::AddDeactivatedIndex(const osg::Vec3& idx)
   {
      GetIndicesChanged()->AddParameter(*new dtCore::NamedVec3Parameter(PARAM_ARRAY_ITEM, idx));
      GetValuesChanged()->AddEmptyIndex();
   }

   const VolumeUpdateMessage::ArrayT* VolumeUpdateMessage::GetIndicesChanged() const
   {
      return mIndicesChanged;
   }
   const VolumeUpdateMessage::ArrayT* VolumeUpdateMessage::GetValuesChanged() const
   {
      return mValuesChanged;
   }

   VolumeUpdateMessage::ArrayT* VolumeUpdateMessage::GetIndicesChanged()
   {
      return mIndicesChanged;
   }
   VolumeUpdateMessage::ArrayT* VolumeUpdateMessage::GetValuesChanged()
   {
      return mValuesChanged;
   }



} /* namespace dtVoxel */
