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

#ifndef DTVOXEL_VOXELGRID_H_
#define DTVOXEL_VOXELGRID_H_

#include <dtVoxel/export.h>
#include <dtVoxel/voxelblock.h>
#include <dtUtil/getsetmacros.h>
#include <dtCore/deltadrawable.h>
#include <dtCore/sigslot.h>

#include <osg/Group>

namespace dtVoxel
{   

   /***
    *  A VoxelGrid represents a 3d grid of VoxelBlocks.  
    */
   class DT_VOXEL_EXPORT VoxelGrid : public dtCore::DeltaDrawable
   {
   public:

      VoxelGrid();

      ///Call Init() before CreateGridFromActor()
      void Init(const osg::Vec3& grid_offset, const osg::Vec3& dimensions, const osg::Vec3& block_dimensions, const osg::Vec3& cellDimensions, const osg::Vec3i& staticResolution, const osg::Vec3i& dynamicResolution);

      void CreateGridFromActor(const osg::Vec3& pos, VoxelActor& voxelActor);

      void CreatePagedLODGrid(const osg::Vec3& pos, VoxelActor& voxelActor);

      void GenerateVisibility(VoxelActor& voxelActor);

      /***
       * Updates the renderable area defined by view distance
       */
      void UpdateGrid(const osg::Vec3& newCameraPos);

      void BeginNewUpdates(const osg::Vec3& newCameraPos, unsigned maxCellsToUpdate, bool allowBackgroundThreading);

      /***
       * Marks a region dirty which will force regenerate next time they are visible
       */
      void MarkDirtyAABB(const osg::BoundingBox& bb);

      /***
      * Deletes all data and recreates it, this is useful if the voxel database has been
      *  deformed but needs to be reset and revert back to the original loaded database.
      */
      void ResetGrid();

      VoxelBlock* GetBlockFromIndex(int index);
      VoxelBlock* GetBlockFromIndex(int x, int y, int z);
      VoxelBlock* GetBlockFromPos(const osg::Vec3& pos);
      osg::Vec3i GetIndexFromPos(const osg::Vec3& pos);

      osg::Node* GetOSGNode() override;
      const osg::Node* GetOSGNode() const override;


      DT_DECLARE_ACCESSOR_INLINE(int, BlocksX)
      DT_DECLARE_ACCESSOR_INLINE(int, BlocksY)
      DT_DECLARE_ACCESSOR_INLINE(int, BlocksZ)
      DT_DECLARE_ACCESSOR_INLINE(int, NumBlocks)
      DT_DECLARE_ACCESSOR_INLINE(float, ViewDistance)
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, GridOffset)
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, WSDimensions)
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, BlockDimensions)
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, CellDimensions)

      const osg::Vec3i& GetStaticResolution() const;
      const osg::Vec3i& GetDynamicResolution() const;

   protected:
      DT_DECLARE_ACCESSOR_INLINE(bool, Initialized)

      openvdb::GridBase::Ptr ConvertToLocalResolutionGrid(openvdb::GridBase::Ptr);

      osg::BoundingBox ComputeWorldBounds(const osg::Vec3& pos);

      osg::Vec3 GetCenterOfBlock(int x, int y, int z);
      void GenerateCacheString();
      bool WriteVoxelDatabase();
      bool ReadVoxelDatabase();


      bool ReadBlockVisibility();
      bool WriteBlockVisibility();

      virtual ~VoxelGrid();

      osg::Vec3i mRes0;
      osg::Vec3i mRes1;
      osg::Vec3i mRes2;
      osg::Vec3i mRes3;

      DT_DECLARE_ACCESSOR_INLINE(float, Dist0)
      DT_DECLARE_ACCESSOR_INLINE(float, Dist1)
      DT_DECLARE_ACCESSOR_INLINE(float, Dist2)

      
   private:
      osg::Vec3 mOffset;
      osg::BoundingBox mAllocatedBounds;
      osg::Vec3i mStaticResolution, mDynamicResolution;
      dtCore::RefPtr<osg::Group> mRootNode;
      dtCore::ObserverPtr<VoxelActor> mVoxelActor;
      std::string mFullPathToFileCache;
      std::string mCacheFolder;
      std::vector<bool> mBlockVisibility;
      std::list<VoxelCellUpdateInfo> mDirtyCells;
      VoxelBlock* mBlocks;

   };

   typedef dtCore::RefPtr<VoxelGrid> VoxelGridPtr;
} /* namespace dtVoxel */

#endif /* DTVOXEL_VOXELGRID_H_ */
