/*
 * voxelmessagetype.cpp
 *
 *  Created on: Jun 9, 2015
 *      Author: david
 */

#include <dtVoxel/voxelmessagetype.h>
#include <dtVoxel/volumeupdatemessage.h>

namespace dtVoxel
{
   IMPLEMENT_ENUM(VoxelMessageType)

   const VoxelMessageType VoxelMessageType::INFO_VOLUME_CHANGED("Volume Change", MessageType::CATEGORY_INFO, "Sent when the state or contents of a volume changes.", dtGame::MessageType::USER_DEFINED_MESSAGE_TYPE + 20000, (VolumeUpdateMessage*)(NULL));

   VoxelMessageType::~VoxelMessageType()
   {
   }

} /* namespace dtVoxel */
