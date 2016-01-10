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
#include <dtVoxel/volumeupdatemessage.h>
#include <dtVoxel/readovdbthreadpooltask.h>
#include <dtGame/gameactorproxy.h>
#include <dtUtil/getsetmacros.h>
//Really need to fine grain this.
#include <openvdb/openvdb.h>
#include <osg/BoundingBox>

namespace dtVoxel
{
   class VoxelGrid;
   class VolumeUpdateMessage;

   class DT_VOXEL_EXPORT VoxelActor: public dtGame::GameActorProxy
   {
   public:
      typedef dtGame::GameActorProxy BaseClass;

      // This signal is fired when it's safe to modify the grids.
      sigslot::signal1<const dtGame::TickMessage&> ModifyGrids;

      VoxelActor();
       
      void BuildPropertyMap() override;

      DT_DECLARE_ACCESSOR(dtCore::ResourceDescriptor, Database);

      /**
       * This is a counter property so that remote versions will reset the database when ResetGrid is called.
       */
      DT_DECLARE_ACCESSOR(int, ResetCount);

      openvdb::GridPtrVecPtr GetGrids();
      openvdb::GridBase::Ptr GetGrid(int i);
      size_t GetNumGrids() const;

      void OnEnteredWorld() override;
      void OnTickLocal(const dtGame::TickMessage& tickMessage) override;
      void OnTickRemote(const dtGame::TickMessage& tickMessage) override;

      /**
       * Returns a new grid that contains the collision set of the given bounding box.
       */
      openvdb::GridBase::Ptr CollideWithAABB(const osg::BoundingBox& bb, int gridIdx = 0);
      bool HasDataInAABB(const osg::BoundingBox& bb, int gridIdx = 0);

      void MarkVisualDirty(const osg::BoundingBox& bb, int gridIdx = 0);

      void ResetGrid();

      DT_DECLARE_ACCESSOR_INLINE(float, ViewDistance)
      DT_DECLARE_ACCESSOR_INLINE(float, IsoLevel)
      DT_DECLARE_ACCESSOR_INLINE(bool, Simplify)
      DT_DECLARE_ACCESSOR_INLINE(float, SampleRatio)
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, GridDimensions)
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, BlockDimensions)
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, CellDimensions)
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, StaticResolution)
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, DynamicResolution)
      DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, Offset)
      DT_DECLARE_ACCESSOR_INLINE(unsigned, MaxCellsToUpdatePerFrame)
      DT_DECLARE_ACCESSOR_INLINE(unsigned, MinCellsToUpdatePerFrame)
      DT_DECLARE_ACCESSOR_INLINE(bool, UpdateCellsOnBackgroundThread)
      DT_DECLARE_ACCESSOR_INLINE(unsigned, NumLODs)

      // if the physics geometry should be created when the actor is remote.
      DT_DECLARE_ACCESSOR_INLINE(bool, CreateRemotePhysics)

      DT_DECLARE_ACCESSOR_INLINE(bool, PauseUpdate)

      // This exists external objects can deform the grid, created a change message, and then tell the visual to update with the message.
      void UpdateVolume(const VolumeUpdateMessage& msg, bool updateVisualOnly);

      /// Forces the background grid loading to block and complete.  It's also called when the loading actually completes
      void CompleteLoad();
   protected:
      virtual void SharedTick(const dtGame::TickMessage& tickMessage, bool local);
      /**
       * Loads the voxel mesh.
       * @throw dtUtil::FileNotFoundException if the resource does not exist.
       */
      virtual void LoadGrid(const dtCore::ResourceDescriptor& rd);
      ~VoxelActor() /*override*/;
      void CreateDrawable() override;

      /// This is used by the message handler
      void OnVolumeUpdateMsg(const VolumeUpdateMessage& msg);

      /** Loads or resets the physics based on the current state and configuration.
       *  It creates the physics for each grid in the file, but the grids can only be bool or float, and
       *  there must be a physics object with the same index as each grid that is configured to be CUSTOM_CONCAVE_MESH
       */
      void InitializePhysics();
      ///  Deletes just the physics data associated with the voxel grids.
      void CleanupPhysics();
   private:

      template<typename GridTypePtr>
      void UpdateVolumeInternal(GridTypePtr grid, const dtCore::NamedArrayParameter* indices, const dtCore::NamedArrayParameter* values, bool updateVisualOnly);

      dtCore::RefPtr<ReadOVDBThreadPoolTask> mLoader;

      dtCore::RefPtr<VoxelGrid> mVisualGrid;
      openvdb::GridPtrVecPtr mGrids;
      std::vector<VolumeUpdateMessagePtr> mUpdateMessages;
      int mTicksSinceVisualUpdate;
   };

   typedef dtCore::RefPtr<VoxelActor> VoxelActorPtr;

} /* namespace dtVoxel */

#endif /* DTVOXEL_VOXELACTOR_H_ */
