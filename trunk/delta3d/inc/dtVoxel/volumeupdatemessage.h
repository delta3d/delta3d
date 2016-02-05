/*
 * volumeupdatemessage.h
 *
 *  Created on: Jun 9, 2015
 *      Author: david
 */

#ifndef DTVOXEL_VOLUMEUPDATEMESSAGE_H_
#define DTVOXEL_VOLUMEUPDATEMESSAGE_H_

#include <dtVoxel/export.h>
#include <dtGame/message.h>
#include <dtGame/messagemacros.h>
#include <dtUtil/typetraits.h>
#include <dtCore/typetoactorproperty.h>
#include <dtCore/namedarrayparameter.h>
#include <osg/Vec3>
#include <openvdb/openvdb.h>
#include <OpenEXR/half.h>

namespace dtVoxel
{
   class DT_VOXEL_EXPORT VolumeUpdateMessage : public dtGame::Message
   {
   public:
      typedef dtGame::Message BaseClass;

      // call GetParameter() for these, all are array parameters.
      // whole number vec3's in this parameter for the indices
      static const dtUtil::RefString PARAM_INDICES_CHANGED;

      // this array should contain the value of the indices that have changes.
      static const dtUtil::RefString PARAM_VALUES_CHANGED;

      // whole number vec3's in this parameter for the indices
      static const dtUtil::RefString PARAM_INDICES_DEACTIVATED;
      // Name to use for array items, for simplicity
      static const dtUtil::RefString PARAM_ARRAY_ITEM;


      DT_DECLARE_ACCESSOR(bool, StoreHalfFloats);

      VolumeUpdateMessage();

      typedef dtCore::NamedArrayParameter ArrayT;

      template<typename ValueType>
      void AddChangedValue(const osg::Vec3& idx, typename dtUtil::TypeTraits<ValueType>::param_type value)
      {
         AddChangedValue(idx, *new typename dtCore::TypeToActorProperty<ValueType>::named_parameter_type(PARAM_ARRAY_ITEM, value));
      }

      /**
       * This adds a value to the changed volume set.  It will search back in the list of values set, and if
       * there is a duplicated idx, it will just set that one to the new value because a voxel can have but one value.
       */
      void AddChangedValue(const osg::Vec3& idx, dtCore::NamedParameter& np);

      /**
       * This adds an empty value to the changed volume set.  It will search back in the list of values set, and if
       * there is a duplicated idx, it will just clear that one because a voxel can have but one value, even if that value is deactivated.
       */
      void AddDeactivatedIndex(const osg::Vec3& idx);

      const ArrayT* GetIndicesChanged() const;
      const ArrayT* GetValuesChanged() const;

      ArrayT* GetIndicesChanged();
      ArrayT* GetValuesChanged();

   protected:
      ~VolumeUpdateMessage() override;
   private:
      unsigned AddOrReuseIndex(const osg::Vec3& idx);

      // Using direct datamembers because in a complex voxel system with a lot of changes, just accessing the values can be slow.
      dtCore::RefPtr<ArrayT> mIndicesChanged;
      dtCore::RefPtr<ArrayT> mValuesChanged;
   };

   typedef dtCore::RefPtr<const VolumeUpdateMessage> VolumeUpdateMessagePtr;

   template<>
   inline void VolumeUpdateMessage::AddChangedValue<float>(const osg::Vec3& idx, float value)
   {
      if (mStoreHalfFloats)
      {
         half h(value);
         AddChangedValue(idx, *new typename dtCore::TypeToActorProperty<unsigned short>::named_parameter_type(PARAM_ARRAY_ITEM, h.bits()));
      }
      else
      {
         AddChangedValue(idx, *new typename dtCore::TypeToActorProperty<float>::named_parameter_type(PARAM_ARRAY_ITEM, value));
      }

   }

} /* namespace dtVoxel */

#endif /* DTVOXEL_VOLUMEUPDATEMESSAGE_H_ */
