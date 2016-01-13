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

#include <dtVoxel/voxelgeometry.h>
#include <dtPhysics/palutil.h>
#include <dtPhysics/palphysicsworld.h>

namespace dtVoxel
{

   dtCore::RefPtr<VoxelGeometry> VoxelGeometry::CreateVoxelGeometryWithCallback(const dtCore::Transform& worldxform, float mass, palCustomGeometryCallback* callBack)
   {
      dtCore::RefPtr<VoxelGeometry> result = new VoxelGeometry();

      palCustomConcaveGeometry* convGeom = dtPhysics::PhysicsWorld::GetInstance().GetPalFactory()->CreateCustomConcaveGeometry();
      if (convGeom == NULL)
      {
         LOG_ERROR("Failed to create voxel geometry.");
         return NULL;
      }

      result->SetPalGeometry(*convGeom);

      dtPhysics::MatrixType osgMat;
      worldxform.Get(osgMat);
      palMatrix4x4 palMat;
      dtPhysics::TransformToPalMatrix(palMat, osgMat);

      //init the geom
      convGeom->Init(palMat, mass, *callBack);
      return result;
   }


   VoxelGeometry::VoxelGeometry()
   : dtPhysics::Geometry(dtPhysics::PrimitiveType::CUSTOM_CONCAVE_MESH)
   {
   }

   VoxelGeometry::~VoxelGeometry()
   {
   }

} /* namespace dtVoxel */
