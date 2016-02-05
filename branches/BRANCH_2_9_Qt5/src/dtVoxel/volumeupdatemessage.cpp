/*
 * volumeupdatemessage.cpp
 *
 *  Created on: Jun 9, 2015
 *      Author: david
 */

#include <dtVoxel/volumeupdatemessage.h>
#include <dtCore/namedarrayparameter.h>
#include <iostream>

namespace dtVoxel
{
   const dtUtil::RefString VolumeUpdateMessage::PARAM_INDICES_CHANGED("ParamIndicesChanged");

   const dtUtil::RefString VolumeUpdateMessage::PARAM_VALUES_CHANGED("ParamValuesChanged");

   const dtUtil::RefString VolumeUpdateMessage::PARAM_ARRAY_ITEM("x");

   VolumeUpdateMessage::VolumeUpdateMessage()
   : mStoreHalfFloats(true)
   , mIndicesChanged(new dtCore::NamedArrayParameter(PARAM_INDICES_CHANGED))
   , mValuesChanged(new dtCore::NamedArrayParameter(PARAM_VALUES_CHANGED))
   {
      AddParameter(mIndicesChanged);
      AddParameter(mValuesChanged);
   }

   VolumeUpdateMessage::~VolumeUpdateMessage() {}

   unsigned VolumeUpdateMessage::AddOrReuseIndex(const osg::Vec3& idx)
   {
      unsigned size = GetIndicesChanged()->GetSize();
      for (unsigned i = size; i > 0; --i)
      {
         unsigned curIdx = i - 1;
         dtCore::NamedVec3Parameter* nvp = static_cast<dtCore::NamedVec3Parameter*>(GetIndicesChanged()->GetParameter(curIdx));
         if (nvp != nullptr && nvp->GetValue() == idx)
         {
            return i;
         }
      }
      GetIndicesChanged()->AddParameter(*new dtCore::NamedVec3Parameter(PARAM_ARRAY_ITEM, idx));
      return size;
   }

   void VolumeUpdateMessage::AddChangedValue(const osg::Vec3& idx, dtCore::NamedParameter& np)
   {
      unsigned position = AddOrReuseIndex(idx);
      if (position == GetValuesChanged()->GetSize())
         GetValuesChanged()->AddParameter(np);
      else
         GetValuesChanged()->SetParameter(position, np);
   }

   void VolumeUpdateMessage::AddDeactivatedIndex(const osg::Vec3& idx)
   {
      unsigned position = AddOrReuseIndex(idx);
      if (position == GetValuesChanged()->GetSize())
         GetValuesChanged()->AddEmptyIndex();
      else
         GetValuesChanged()->SetEmptyIndex(position);
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
