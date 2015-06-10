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

   const VoxelMessageType VoxelMessageType::INFO_VOLUME_CHANGED("Volume Change", MessageType::CATEGORY_INFO, "Sent when a game manager changes the simulation time.", 20, (VolumeUpdateMessage*)(NULL));

   VoxelMessageType::~VoxelMessageType()
   {
   }

} /* namespace dtVoxel */
