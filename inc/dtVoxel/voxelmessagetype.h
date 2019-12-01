/*
 * voxelmessagetype.h
 *
 *  Created on: Jun 9, 2015
 *      Author: david
 */

#ifndef INC_DTVOXEL_VOXELMESSAGETYPE_H_
#define INC_DTVOXEL_VOXELMESSAGETYPE_H_

#include <dtVoxel/export.h>
#include <dtGame/messagetype.h>

namespace dtVoxel
{

   class DT_VOXEL_EXPORT VoxelMessageType: public dtGame::MessageType
   {
      DECLARE_ENUM(VoxelMessageType);

      // Change set for a volume.
      static const VoxelMessageType INFO_VOLUME_CHANGED;

   protected:
      template<typename MessageClass>
      VoxelMessageType(const std::string& name, const std::string& category,
                  const std::string& description, const unsigned short id, const MessageClass* val)
         : dtGame::MessageType(name, category, description, id, val)
      {
         AddInstance(this);
      }

      ~VoxelMessageType() override;
   };

} /* namespace dtVoxel */

#endif /* INC_DTVOXEL_VOXELMESSAGETYPE_H_ */
