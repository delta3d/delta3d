/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2015, Caper Holdings, LLC
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef DTVOXEL_VOXELBLOCK_H_
#define DTVOXEL_VOXELBLOCK_H_

#include <dtVoxel/export.h>
#include <dtVoxel/voxelcell.h>
#include <dtCore/refptr.h>
#include <osgVolume/Volume>

namespace dtVoxel
{
    /***
    *  A VoxelBlock represents a 3d block of VoxelCells.
    */
   class DT_VOXEL_EXPORT VoxelBlock
   {
   public:
      VoxelBlock();
      virtual ~VoxelBlock();

      void Init(const osg::Vec3& ws_dimensions, const osg::Vec3& ws_offset, const osg::Vec3f& cellSize);
      bool IsAllocated() const;
      
      const osg::Vec3& GetOffset();
      
      void Allocate(VoxelActor& voxelActor, openvdb::GridBase::Ptr localGrid, const osg::Vec3i& textureResolution);
      
      //void AllocateCell(const osg::Vec3& pos, const osg::Vec3i& textureResolution);
      
      VoxelCell* GetCellFromIndex(int x, int y, int z);      
      VoxelCell* GetCellFromPos(const osg::Vec3& pos);
      
      osgVolume::Volume* GetOSGVolume();      
      const osgVolume::Volume* GetOSGVolume() const;
      
      const osg::Vec3& GetOffset() const;

   protected:
   
      openvdb::GridBase::Ptr ConvertToLocalResolutionGrid(openvdb::GridBase::Ptr);

   private:
      osg::Vec3 mWSDimensions;
      osg::Vec3 mWSCellDimensions;
      osg::Vec3 mOffset;

      dtCore::RefPtr<osgVolume::Volume> mVolume;

      VoxelCell* mCells;
   };

} /* namespace dtVoxel */

#endif /* DTVOXEL_VOXELBLOCK_H_ */
