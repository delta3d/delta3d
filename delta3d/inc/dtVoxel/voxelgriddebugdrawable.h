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

#ifndef DTVOXEL_VoxelGridDebugDrawable_H_
#define DTVOXEL_VoxelGridDebugDrawable_H_

#include <dtVoxel/export.h>
#include <dtCore/deltadrawable.h>

#include <dtVoxel/voxelgrid.h>

namespace dtVoxel
{
    /***
    *  VoxelGridDebugDrawable is a DeltaDrawable that performs debug rendering of a VoxelGrid
    */
   class DT_VOXEL_EXPORT VoxelGridDebugDrawable : public dtCore::DeltaDrawable
   {
   public:
      VoxelGridDebugDrawable();
      virtual ~VoxelGridDebugDrawable();

      void CreateDebugDrawable(dtVoxel::VoxelGrid& grid);

      /*virtual*/ osg::Node* GetOSGNode();
      /*virtual*/ const osg::Node* GetOSGNode() const;


      DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, GridSize)
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, BlockSize)
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, CellSize)

      //DT_DECLARE_ACCESSOR_INLINE(float, DrawDistance)


   private:

      dtCore::RefPtr<osg::Group> mSceneRoot;

   };

} /* namespace dtVoxel */

#endif /* DTVOXEL_VoxelGridDebugDrawable_H_ */
