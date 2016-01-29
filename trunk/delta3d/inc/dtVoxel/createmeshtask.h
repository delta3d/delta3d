/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2016, Caper Holdings, LLC
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

#ifndef DTVOXEL_CREATEMESHTASK_H_
#define DTVOXEL_CREATEMESHTASK_H_

#include <dtVoxel/export.h>
#include <dtUtil/threadpool.h>

#include <osg/Vec3>
#include <osg/Matrix>
#include <osg/Geometry>

#include <openvdb/tools/Interpolation.h>

#include <dtVoxel/marchingcubes.h>

#include <openvdb/openvdb.h>
#include <OpenThreads/Atomic>

namespace dtVoxel
{
    
   class VoxelCellImpl;
   class VoxelCell;
   
   class DT_VOXEL_EXPORT CreateMeshTask : public dtUtil::ThreadPoolTask
   {
   public:
      enum GenerateMode{ Default, UseMaxThreads, UseSetNumThreads, RunInSingleThread };

   public:
      CreateMeshTask(const osg::Vec3& offset, const osg::Vec3& texelSize, const osg::Vec3i& resolution, double isoLevel, openvdb::FloatGrid::Ptr grid);
      ~CreateMeshTask();

      osg::Geode* TakeGeometry();

      void SetMode();
      GenerateMode GetMode() const;
      
      bool IsDone() const;

      virtual void operator()();
      
      double GetTime() const;

      DT_DECLARE_ACCESSOR_INLINE(bool, SkipBackFaces);
      DT_DECLARE_ACCESSOR_INLINE(int, NumThreads);

   private:
            
      void RunMultiThreads();
      void RunSingleThreaded();

      int SampleSingleCell(int i, int j, int k, openvdb::tools::GridSampler<openvdb::FloatGrid::ConstAccessor, openvdb::tools::PointSampler>& sampler, TRIANGLE* triangles);
      double SampleCoord(double x, double y, double z, openvdb::tools::GridSampler<openvdb::FloatGrid::ConstAccessor, openvdb::tools::PointSampler>& fastSampler);

      volatile bool mIsDone;
           
      GenerateMode mMode;
      
      double mTime;
      double mIsoLevel;
      
      osg::Vec3 mOffset;
      osg::Vec3 mTexelSize;
      osg::Vec3i mResolution;
      dtCore::RefPtr<osg::Geode> mMesh;
      openvdb::FloatGrid::Ptr mGrid;

      osg::Vec3::value_type* mCacheData;
   };
   
} /* namespace dtVoxel */

#endif /* DTVOXEL_CREATEMESHTASK_H_ */
