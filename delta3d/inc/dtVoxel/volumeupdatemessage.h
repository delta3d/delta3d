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
#include <dtUtil/getsetmacros.h>
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

      const ArrayT* GetIndicesChanged() const
      {
         return static_cast<const ArrayT*>(GetParameter(VolumeUpdateMessage::PARAM_INDICES_CHANGED));
      }
      const ArrayT* GetValuesChanged() const
      {
         return static_cast<const ArrayT*>(GetParameter(VolumeUpdateMessage::PARAM_VALUES_CHANGED));
      }
      const ArrayT* GetIndicesDeactivated() const
      {
         return static_cast<const ArrayT*>(GetParameter(VolumeUpdateMessage::PARAM_INDICES_DEACTIVATED));
      }

      ArrayT* GetIndicesChanged()
      {
         return static_cast<ArrayT*>(GetParameter(VolumeUpdateMessage::PARAM_INDICES_CHANGED));
      }
      ArrayT* GetValuesChanged()
      {
         return static_cast<ArrayT*>(GetParameter(VolumeUpdateMessage::PARAM_VALUES_CHANGED));
      }
      ArrayT* GetIndicesDeactivated()
      {
         return static_cast<ArrayT*>(GetParameter(VolumeUpdateMessage::PARAM_INDICES_DEACTIVATED));
      }

   protected:
      ~VolumeUpdateMessage() override;
   private:
   };


} /* namespace dtVoxel */

#endif /* DTVOXEL_VOLUMEUPDATEMESSAGE_H_ */
