/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 *
 * William E. Johnson II
 */

#include <dtActors/meshterrainactorproxy.h>
#include <dtDAL/datatype.h>
#include <dtDAL/resourceactorproperty.h>
#include <dtDAL/resourcedescriptor.h>
#include <dtDAL/actorproxyicon.h>
#include <dtUtil/log.h>

using namespace dtCore;
using namespace dtDAL;

namespace dtActors
{


   ///////////////////////////////////////////////////////////////////////////////
   MeshTerrainActorProxy::MeshTerrainActorProxy()
   {
      SetClassName("dtCore::Object");
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MeshTerrainActorProxy::BuildPropertyMap()
   {
      const std::string &GROUPNAME = "Terrain";
      DeltaObjectActorProxy::BuildPropertyMap();


      AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::TERRAIN,
            "terrain mesh", "Terrain Mesh", dtDAL::ResourceActorProperty::SetFuncType(this, &MeshTerrainActorProxy::LoadFile),
            "The mesh that defines the geometry of the terrain.", GROUPNAME));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MeshTerrainActorProxy::CreateActor()
   {
      SetActor(*new MeshTerrainActor);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MeshTerrainActorProxy::LoadFile(const std::string &fileName)
   {
      dtCore::Object *obj = static_cast<dtCore::Object*>(GetActor());

      if (obj->LoadFile(fileName, false) == NULL)
      {
         if (!fileName.empty())
            LOG_ERROR("Error loading terrain mesh file: " + fileName);

         return;
      }


      //We need this little hack to ensure that when a mesh is loaded, the collision
      //properties get updated properly.
      SetCollisionType(GetCollisionType());
   }

   ///////////////////////////////////////////////////////////////////////////////
   const dtDAL::BaseActorObject::RenderMode& MeshTerrainActorProxy::GetRenderMode()
   {
      dtDAL::ResourceDescriptor resource = GetResource("terrain mesh");
      if (resource.IsEmpty() == false)
      {
         if (resource.GetResourceIdentifier().empty() || GetActor()->GetOSGNode() == NULL)
            return dtDAL::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON;
         else
            return dtDAL::BaseActorObject::RenderMode::DRAW_ACTOR;
      }
      else
         return dtDAL::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON;
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProxyIcon* MeshTerrainActorProxy::GetBillBoardIcon()
   {
      if (!mBillBoardIcon.valid())
      {
         mBillBoardIcon = new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IMAGE_BILLBOARD_MESHTERRAIN);
      }

      return mBillBoardIcon.get();
   }

   //////////////////////////////////////////////////////////////////////////
   MeshTerrainActor::MeshTerrainActor()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   MeshTerrainActor::~MeshTerrainActor()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void MeshTerrainActor::AddedToScene(dtCore::Scene* scene)
   {
      dtCore::Object::AddedToScene(scene);

      // Don't load the file if we're not
      // really being added to the scene
      if (scene != NULL)
      {
         if (!GetFilename().empty())
         {
            LoadFile(GetFilename());
         }
      }
   }
}
