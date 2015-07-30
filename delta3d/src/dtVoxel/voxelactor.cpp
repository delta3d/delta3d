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

#include <dtVoxel/voxelactor.h>
#include <dtVoxel/voxelgriddebugdrawable.h>
#include <dtVoxel/aabbintersector.h>
#include <dtVoxel/voxelgeometry.h>
#include <dtVoxel/voxelmessagetype.h>
#include <dtVoxel/volumeupdatemessage.h>

#include <dtABC/application.h>

#include <dtCore/transformable.h>
#include <dtCore/propertymacros.h>
#include <dtCore/camera.h>
#include <dtCore/project.h>
#include <dtCore/transform.h>

#include <dtGame/basemessages.h>
#include <dtGame/gamemanager.h>
#include <dtGame/shaderactorcomponent.h>

#include <openvdb/openvdb.h>
#include <openvdb/tools/Interpolation.h>

#include <dtPhysics/physicsactcomp.h>

#include <dtUtil/functor.h>

namespace dtVoxel
{

   VoxelActor::VoxelActor()
   : mViewDistance(1000.0f)
   , mIsoLevel(0.12f)
   , mSimplify(false)
   , mSampleRatio(0.2f)
   , mCreateRemotePhysics(false)
   {
   }

   VoxelActor::~VoxelActor()
   {
   }

   /////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR_WITH_STATEMENT(VoxelActor, dtCore::ResourceDescriptor, Database, LoadGrid(value););

   /////////////////////////////////////////////////////
   void VoxelActor::LoadGrid(const dtCore::ResourceDescriptor& rd)
   {
      std::cout << "Loading Grid" << std::endl;

      if (rd != GetDatabase() && !rd.IsEmpty())
      {
         try
         {
            openvdb::io::File file(dtCore::Project::GetInstance().GetResourcePath(rd));
            file.open();
            mGrids = file.getGrids();
            file.close();

            std::cout << "Done Loading Grid" << std::endl;

         }
         catch (const openvdb::IoError& ioe)
         {
            std::cout << "Error Loading Grid" << std::endl;

            throw dtUtil::FileUtilIOException(ioe.what(), __FILE__, __LINE__);
         }
      }
      else if (rd.IsEmpty())
      {
         mGrids = NULL;
      }
   }

   /////////////////////////////////////////////////////
   void VoxelActor::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      typedef dtCore::PropertyRegHelper<VoxelActor> RegHelper;
      static dtUtil::RefString GROUP("VoxelActor");
      RegHelper regHelper(*this, this, GROUP);
      
      DT_REGISTER_PROPERTY_WITH_LABEL(ViewDistance, "View Distance", "The distance to at which voxels will be generated into groups of volumes and rendered.", RegHelper, regHelper);
      DT_REGISTER_PROPERTY_WITH_LABEL(IsoLevel, "IsoLevel", "The value of the database which marks the surface, use the same number as the thickness if you compiled a database with the OpenVDBCompiler.", RegHelper, regHelper);
      DT_REGISTER_PROPERTY_WITH_LABEL(Simplify, "Simplify", "Whether or not to simplify the result, this can take a long longer to generate.", RegHelper, regHelper);
      DT_REGISTER_PROPERTY_WITH_LABEL(SampleRatio, "SampleRatio", "The percentage at which to sample in each dimension when simplifying, make sure to turn on the Simplify option to use this property.", RegHelper, regHelper);
      DT_REGISTER_PROPERTY_WITH_LABEL(GridDimensions, "Grid Dimensions", "The size of the grid to allocate into blocks.", RegHelper, regHelper);
      DT_REGISTER_PROPERTY_WITH_LABEL(BlockDimensions,"Block Dimensions", "The size of the blocks within the grid.", RegHelper, regHelper);
      DT_REGISTER_PROPERTY_WITH_LABEL(CellDimensions, "Cell Dimensions", "The size of the cells within the blocks", RegHelper, regHelper);
      DT_REGISTER_PROPERTY_WITH_LABEL(StaticResolution, "Static Resolution", "The resolution to sample the VDB database for the static pregenerated data.", RegHelper, regHelper);
      DT_REGISTER_PROPERTY_WITH_LABEL(DynamicResolution, "Dynamic Resolution", "The resolution to sample the VDB database for the dynamic deformable data.", RegHelper, regHelper);
      DT_REGISTER_PROPERTY_WITH_LABEL(Offset, "Offset", "The offset of the database in world space.", RegHelper, regHelper);
      DT_REGISTER_PROPERTY_WITH_LABEL(CreateRemotePhysics, "Create Remote Physics", "Create the voxel geometry for the physics if this actor is remote.", RegHelper, regHelper);

      DT_REGISTER_RESOURCE_PROPERTY(dtCore::DataType::VOLUME, Database, "Database", "Voxel database file", RegHelper, regHelper);
   }

   /////////////////////////////////////////////////////
   openvdb::GridPtrVecPtr VoxelActor::GetGrids()
   {
      return mGrids;
   }

   /////////////////////////////////////////////////////
   openvdb::GridBase::Ptr VoxelActor::GetGrid(int i)
   {
      if (mGrids) return (*mGrids)[i];
      return NULL;
   }

   /////////////////////////////////////////////////////
   size_t VoxelActor::GetNumGrids() const
   {
      size_t result = 0;
      if (mGrids) result = mGrids->size();
      return result;
   }

   /////////////////////////////////////////////////////
   openvdb::GridBase::Ptr VoxelActor::CollideWithAABB(const osg::BoundingBox& bb, int gridIdx)
   {
      openvdb::GridBase::Ptr result(NULL);
      openvdb::BBoxd bbox(openvdb::Vec3d(bb.xMin(), bb.yMin(), bb.zMin()), openvdb::Vec3d(bb.xMax(), bb.yMax(), bb.zMax()));
      if (openvdb::BoolGrid::Ptr gridB = boost::dynamic_pointer_cast<openvdb::BoolGrid>(GetGrid(gridIdx)))
      {
         AABBIntersector<openvdb::BoolGrid> aabb(gridB);
         aabb.SetWorldBB(bbox);
         aabb.Intersect();
         result = aabb.GetHits();
       }
      else if (openvdb::FloatGrid::Ptr gridF = boost::dynamic_pointer_cast<openvdb::FloatGrid>(GetGrid(gridIdx)))
      {
         AABBIntersector<openvdb::FloatGrid> aabb(gridF);
         aabb.SetWorldBB(bbox);
         aabb.Intersect();
         result = aabb.GetHits();
      }
      return result;
   }

   ////////////////////////////////////////////////////
   bool VoxelActor::HasDataInAABB(const osg::BoundingBox& bb, int gridIdx)
   {
      bool result = false;
      openvdb::BBoxd bbox(openvdb::Vec3d(bb.xMin(), bb.yMin(), bb.zMin()), openvdb::Vec3d(bb.xMax(), bb.yMax(), bb.zMax()));
      if (openvdb::BoolGrid::Ptr gridB = boost::dynamic_pointer_cast<openvdb::BoolGrid>(GetGrid(gridIdx)))
      {
         AABBIntersector<openvdb::BoolGrid> aabb(gridB);
         aabb.SetWorldBB(bbox);
         result = aabb.HasDataInAABB();
       }
      else if (openvdb::FloatGrid::Ptr gridF = boost::dynamic_pointer_cast<openvdb::FloatGrid>(GetGrid(gridIdx)))
      {
         AABBIntersector<openvdb::FloatGrid> aabb(gridF);
         aabb.SetWorldBB(bbox);
         aabb.Intersect();
         result = aabb.HasDataInAABB();
      }
      return result;
   }

   /////////////////////////////////////////////////////
   void VoxelActor::MarkVisualDirty(const osg::BoundingBox& bb, int gridIdx)
   {
      mVisualGrid->MarkDirtyAABB(bb);
   }

   /////////////////////////////////////////////////////
   void VoxelActor::OnTickLocal(const dtGame::TickMessage& tickMessage)
   {
      if (mVisualGrid.valid())
      {
         dtGame::GameManager* gm = GetGameManager();

         if (gm != nullptr)
         {
            dtCore::Camera* cam = gm->GetApplication().GetCamera();

            osg::Vec3 pos;
            dtCore::Transform xform;
            cam->GetTransform(xform);
            xform.GetTranslation(pos);

            mVisualGrid->UpdateGrid(pos);

            ModifyGrids.emit_signal(tickMessage);

            mVisualGrid->BeginNewUpdates(pos);
            //std::cout << "Updating voxel grid with position (" << pos[0] << ", " << pos[1] << ", " << pos[2] << ")" << std::endl;
         }
      }
   }

   /////////////////////////////////////////////////////
   void VoxelActor::CreateDrawable()
   {
      //commented out, this is a simple debug rendering
      //dtCore::RefPtr<VoxelGridDebugDrawable> dd = new VoxelGridDebugDrawable();
      //dd->CreateDebugDrawable(*mGrid);
      //SetDrawable(*dd);

      mVisualGrid = new VoxelGrid();
      SetDrawable(*mVisualGrid);

   }

   /////////////////////////////////////////////////////
   template<typename GridTypePtr>
   void VoxelActor::UpdateVolumeInternal(GridTypePtr grid, const dtCore::NamedArrayParameter* indices, const dtCore::NamedArrayParameter* values,
         const dtCore::NamedArrayParameter* indicesDeactivated, bool updateVisualOnly)
   {
      typedef typename GridTypePtr::element_type GridType;
      typedef typename GridType::ValueType ValueType;
      typedef typename dtCore::TypeToActorProperty<ValueType>::named_parameter_type ParameterType;
      typedef typename GridType::Accessor AccessorType;

      AccessorType accessor = grid->getAccessor();

      osg::BoundingBox bb;
      for (unsigned i = 0; i < indices->GetSize(); ++i)
      {
         const dtCore::NamedParameter* indexP = indices->GetParameter(i);
         if (indexP != nullptr && indexP->GetDataType() == dtCore::DataType::VEC3)
         {
            auto indexVp = static_cast<const dtCore::NamedVec3Parameter*>(indexP);
            osg::Vec3 idxVec = indexVp->GetValue();
            auto valueParam = dynamic_cast<const ParameterType*>(values->GetParameter(i));
            if (valueParam != NULL)
            {
               ValueType val = valueParam->GetValue();
               openvdb::Vec3d idxOVDBVec(idxVec.x(), idxVec.y(), idxVec.z());
               openvdb::Vec3d worldVec = grid->transform().indexToWorld(idxOVDBVec);
               bb.expandBy(osg::Vec3(worldVec.x(), worldVec.y(), worldVec.z()));
               if (!updateVisualOnly)
               {
                  openvdb::Coord c(std::round(idxVec.x()), std::round(idxVec.y()), std::round(idxVec.z()));
                  accessor.setValue(c, val);
               }
            }
         }
         else
         {
            LOGN_ERROR("voxelactor.cpp", "Received a VolumeUpdateMessage, but the indices are not Vec3 parameters.");
         }
      }
      for (unsigned i = 0; i < indicesDeactivated->GetSize(); ++i)
      {
         const dtCore::NamedParameter* indexP = indicesDeactivated->GetParameter(i);
         if (indexP != nullptr && indexP->GetDataType() == dtCore::DataType::VEC3)
         {
            auto indexVp = static_cast<const dtCore::NamedVec3Parameter*>(indexP);
            osg::Vec3 idxVec = indexVp->GetValue();
            openvdb::Vec3d idxOVDBVec(idxVec.x(), idxVec.y(), idxVec.z());
            openvdb::Vec3d worldVec = grid->transform().indexToWorld(idxOVDBVec);
            bb.expandBy(osg::Vec3(worldVec.x(), worldVec.y(), worldVec.z()));
            if (!updateVisualOnly)
            {
               openvdb::Coord c(openvdb::Coord::round(idxOVDBVec));
               accessor.setValueOff(c, grid->background());
            }
         }
         else
         {
            LOGN_ERROR("voxelactor.cpp", "Received a VolumeUpdateMessage, but the indices deactivated are not Vec3 parameters.");
         }
      }
      //re-add when not busted
      MarkVisualDirty(bb, 0);
   }

   /////////////////////////////////////////////////////
   void VoxelActor::UpdateVolume(const VolumeUpdateMessage& msg, bool updateVisualOnly)
   {
      if (GetNumGrids() > 0)
      {
         const dtCore::NamedArrayParameter* indices = msg.GetIndicesChanged();
         const dtCore::NamedArrayParameter* values = msg.GetValuesChanged();
         const dtCore::NamedArrayParameter* indicesDeactivated = msg.GetIndicesDeactivated();

         openvdb::FloatGrid::Ptr gridF = boost::dynamic_pointer_cast<openvdb::FloatGrid>(GetGrid(0));
         if (gridF)
         {
            UpdateVolumeInternal(gridF, indices, values, indicesDeactivated, updateVisualOnly);
         }
         else
         {
            openvdb::BoolGrid::Ptr gridB = boost::dynamic_pointer_cast<openvdb::BoolGrid>(GetGrid(0));
            if (gridB)
               UpdateVolumeInternal(gridB, indices, values, indicesDeactivated, updateVisualOnly);
         }
      }
   }

   /////////////////////////////////////////////////////
   void VoxelActor::OnVolumeUpdateMsg(const VolumeUpdateMessage& msg)
   {
      if ((IsRemote() || GetLocalActorUpdatePolicy() != dtGame::GameActorProxy::LocalActorUpdatePolicy::IGNORE_ALL)
                  && msg.GetSource() != GetGameManager()->GetMachineInfo())
         UpdateVolume(msg, false);
   }

   /////////////////////////////////////////////////////
   void VoxelActor::OnEnteredWorld()
   {
      RegisterForMessages(dtGame::MessageType::TICK_LOCAL, dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);

      osg::Vec3i staticRes(int(mStaticResolution.x()), int(mStaticResolution.y()), int(mStaticResolution.z()));
      osg::Vec3i dynamicRes(int(mDynamicResolution.x()), int(mDynamicResolution.y()), int(mDynamicResolution.z()));


      mVisualGrid->Init(mOffset, mGridDimensions, mBlockDimensions, mCellDimensions, staticRes, dynamicRes);
      
      dtGame::GameManager* gm = GetGameManager();

      if (gm != nullptr)
      {
         dtCore::Camera* cam = gm->GetApplication().GetCamera();

         osg::Vec3 pos;
         dtCore::Transform xform;
         cam->GetTransform(xform);
         xform.GetTranslation(pos);

         mVisualGrid->SetViewDistance(mViewDistance);
         mVisualGrid->CreatePagedLODGrid(pos, *this);
      }
      else
      {
         LOG_ERROR("NULL GameManager!");
      }


      if (!IsRemote() || GetCreateRemotePhysics())
      {
         dtPhysics::PhysicsActCompPtr pac = GetComponent<dtPhysics::PhysicsActComp>();
         if (mGrids && pac.valid())
         {
            dtPhysics::PhysicsObject* po = pac->GetMainPhysicsObject();
            if (po != nullptr && po->GetPrimitiveType() == dtPhysics::PrimitiveType::CUSTOM_CONCAVE_MESH)
            {
               dtPhysics::TransformType xform;
               VoxelGeometryPtr geometry;
               openvdb::FloatGrid::Ptr gridF = boost::dynamic_pointer_cast<openvdb::FloatGrid>(GetGrid(0));
               if (gridF)
               {
                  geometry = VoxelGeometry::CreateVoxelGeometry(xform, po->GetMass(), gridF);
               }
               else
               {
                  openvdb::BoolGrid::Ptr gridB = boost::dynamic_pointer_cast<openvdb::BoolGrid>(GetGrid(0));
                  if (gridB)
                     geometry = VoxelGeometry::CreateVoxelGeometry(xform, po->GetMass(), gridB);
               }
               if (geometry.valid())
                  po->CreateFromGeometry(*geometry);
            }
         }
      }

      RegisterForMessagesAboutSelf(VoxelMessageType::INFO_VOLUME_CHANGED, dtUtil::MakeFunctor(&VoxelActor::OnVolumeUpdateMsg, this));

      dtGame::ShaderActorComponent* shaderComp = nullptr;
      GetComponent(shaderComp);
      if (shaderComp != nullptr)
      {
         mVisualGrid->SetShaderGroup(shaderComp->GetCurrentShader().GetResourceIdentifier());
      }
   }


} /* namespace dtVoxel */
