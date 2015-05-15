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
#include <dtCore/transformable.h>
#include <dtCore/propertymacros.h>
#include <openvdb/openvdb.h>
#include <dtCore/project.h>

namespace dtVoxel
{

   VoxelActor::VoxelActor()
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
      if (rd != GetDatabase() && !rd.IsEmpty())
      {
         try
         {
            openvdb::io::File file(dtCore::Project::GetInstance().GetResourcePath(rd));
            file.open();
            mGrids = file.getGrids();
            file.close();
         }
         catch (const openvdb::IoError& ioe)
         {
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
      typedef dtCore::PropertyRegHelper<VoxelActor> RegHelper;
      static dtUtil::RefString GROUP("VoxelActor");
      RegHelper regHelper(*this, this, GROUP);
      DT_REGISTER_RESOURCE_PROPERTY(dtCore::DataType::TERRAIN, Database, "Database", "Voxel database file", RegHelper, regHelper);
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
   void VoxelActor::CollideWithAABB(osg::BoundingBox& bb)
   {

   }

   /////////////////////////////////////////////////////
   void VoxelActor::CreateDrawable()
   {
      // This is temporary
      SetDrawable(*new dtCore::Transformable("VoxelDrawable"));
   }

} /* namespace dtVoxel */
