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

#ifndef DTVOXEL_VOXELACTOR_H_
#define DTVOXEL_VOXELACTOR_H_

#include <dtVoxel/export.h>
#include <dtGame/gameactorproxy.h>
#include <dtUtil/getsetmacros.h>
//Really need to fine grain this.
#include <openvdb/openvdb.h>
#include <osg/BoundingBox>

namespace dtVoxel
{
   class VoxelGrid;

   class DT_VOXEL_EXPORT VoxelActor: public dtGame::GameActorProxy
   {
   public:
      typedef dtGame::GameActorProxy BaseClass;

      VoxelActor();

      void BuildPropertyMap() override;

      DT_DECLARE_ACCESSOR(dtCore::ResourceDescriptor, Database);

      openvdb::GridPtrVecPtr GetGrids();
      openvdb::GridBase::Ptr GetGrid(int i);
      size_t GetNumGrids() const;

      virtual void OnEnteredWorld();
      virtual void OnTickLocal(const dtGame::TickMessage& tickMessage);


      /**
       * Returns a new grid that contains the collision set of the given bounding box.
       */
      openvdb::GridBase::Ptr CollideWithAABB(osg::BoundingBox& bb, int gridIdx = 0);

      void RemoveFromGridAABB(osg::BoundingBox& bb, int gridIdx = 0);

      DT_DECLARE_ACCESSOR_INLINE(float, ViewDistance)
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, GridDimensions)
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, BlockDimensions)
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, CellDimensions)
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, TextureResolution)
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, Offset)

   protected:
      /**
       * Loads the voxel mesh.
       * @throw dtUtil::FileNotFoundException if the resource does not exist.
       */
      virtual void LoadGrid(const dtCore::ResourceDescriptor& rd);
      ~VoxelActor() override;
      void CreateDrawable() override;

   private:
      dtCore::RefPtr<VoxelGrid> mGrid;
      openvdb::GridPtrVecPtr mGrids;
   };

   typedef dtCore::RefPtr<VoxelActor> VoxelActorPtr;

} /* namespace dtVoxel */

#endif /* DTVOXEL_VOXELACTOR_H_ */
