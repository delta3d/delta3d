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
      VolumeUpdateMessage();

      typedef dtCore::NamedArrayParameter ArrayT;

      template<typename ValueType>
      void AddChangedValue(const osg::Vec3& idx, typename dtUtil::TypeTraits<ValueType>::param_type value)
      {
         GetIndicesChanged()->AddParameter(*new dtCore::NamedVec3Parameter("x", idx));
         GetValuesChanged()->AddParameter(*new typename dtCore::TypeToActorProperty<ValueType>::named_parameter_type("x", value));
      }

      void AddDeactivatedIndex(const osg::Vec3& idx);

      const ArrayT* GetIndicesChanged() const;
      const ArrayT* GetValuesChanged() const;
      const ArrayT* GetIndicesDeactivated() const;

      ArrayT* GetIndicesChanged();
      ArrayT* GetValuesChanged();
      ArrayT* GetIndicesDeactivated();

   protected:
      ~VolumeUpdateMessage() override;
   private:
   };


} /* namespace dtVoxel */

#endif /* DTVOXEL_VOLUMEUPDATEMESSAGE_H_ */
