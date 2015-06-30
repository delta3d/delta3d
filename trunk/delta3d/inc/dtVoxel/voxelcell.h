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

#ifndef DTVOXEL_VOXELCELL_H_
#define DTVOXEL_VOXELCELL_H_

#include <dtVoxel/export.h>
#include <dtVoxel/voxelactor.h>
#include <dtUtil/getsetmacros.h>
#include <dtUtil/threadpool.h>

#include <osg/Vec3>
#include <osg/Matrix>
#include <osg/Geometry>

#include <map>

#include <openvdb/tools/Interpolation.h>

#include <openvdb/openvdb.h>

namespace dtVoxel
{
    
   class VoxelCellImpl;
   class VoxelCell;
   
   class DT_VOXEL_EXPORT CreateMeshTask : public dtUtil::ThreadPoolTask
   {
   public:
      CreateMeshTask(const osg::Vec3& offset, const osg::Vec3& texelSize, const osg::Vec3i& resolution, openvdb::FloatGrid::Ptr grid);

      osg::Geode* TakeGeometry();

      bool IsDone() const;

      virtual void operator()();

      double SampleCoord(double x, double y, double z, openvdb::tools::GridSampler<openvdb::FloatGrid, openvdb::tools::PointSampler>& fastSampler);


   private:
   
      bool mIsDone;
      osg::Vec3 mOffset;
      osg::Vec3 mTexelSize;
      osg::Vec3i mResolution;
      dtCore::RefPtr<osg::Geode> mMesh;
      std::map<osg::Vec3, int> mVectorMap;
      openvdb::FloatGrid::Ptr mGrid;
      openvdb::tools::GridSampler<openvdb::FloatGrid, openvdb::tools::PointSampler> mSampler;

   };

   /***
   *  A VoxelCell represents a 3d grid of individual voxels stored in a 3d texture.
   */
   class DT_VOXEL_EXPORT VoxelCell
   {
   public:
      VoxelCell();
      virtual ~VoxelCell();

      void CreateImage(VoxelActor& voxelActor, openvdb::GridBase::Ptr localGrid, osg::Matrix& transform, const osg::Vec3& cellSize, const osg::Vec3i& texture_resolution);
      
      void CreateMesh(VoxelActor& voxelActor, osg::Matrix& transform, const osg::Vec3& cellSize, const osg::Vec3i& resolution);
      
      void CreateMeshWithTask(VoxelActor& voxelActor, osg::Matrix& transform, const osg::Vec3& cellSize, const osg::Vec3i& resolution);
      
      bool CheckTaskStatus();

      void TakeGeometry();


      bool IsAllocated() const;
      void SetAllocated(bool b);
      void DeAllocate();

      bool IsDirty() const;
      void SetDirty(bool b);

      osg::Vec3 GetOffset() const;

      osg::Node* GetOSGNode();
      const osg::Node* GetOSGNode() const;

      double SampleCoord(double x, double y, double z, openvdb::tools::GridSampler<openvdb::FloatGrid::ConstAccessor, openvdb::tools::PointSampler>& fastSampler);
         
      void AddGeometry(VoxelActor& voxelActor, osg::Matrix& transform, const osg::Vec3& cellSize, const osg::Vec3i& resolution, osg::Vec3Array* vertArray, osg::DrawElementsUInt* drawElements);

   protected:
      
      void AllocateImage(VoxelActor& voxelActor, openvdb::GridBase::Ptr gridPtr, const osg::Vec3& cellSize, int width, int height, int slices);
      
   
   private:
      VoxelCellImpl* mImpl;
   };

} /* namespace dtVoxel */

#endif /* DTVOXEL_VOXELCELL_H_ */
