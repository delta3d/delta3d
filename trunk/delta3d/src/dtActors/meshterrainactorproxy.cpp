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
#include <dtCore/datatype.h>
#include <dtCore/resourceactorproperty.h>
#include <dtCore/resourcedescriptor.h>
#include <dtCore/actorproxyicon.h>
#include <dtUtil/log.h>

using namespace dtCore;
using namespace dtCore;

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
      const std::string& GROUPNAME = "Terrain";
      DeltaObjectActorProxy::BuildPropertyMap();


      AddProperty(new dtCore::ResourceActorProperty(dtCore::DataType::TERRAIN,
            "terrain mesh", "Terrain Mesh",
            dtCore::ResourceActorProperty::SetFuncType(this, &MeshTerrainActorProxy::LoadFile),
            "The mesh that defines the geometry of the terrain.", GROUPNAME));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MeshTerrainActorProxy::CreateDrawable()
   {
      SetDrawable(*new MeshTerrainActor);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MeshTerrainActorProxy::LoadFile(const std::string& fileName)
   {
      dtCore::Object *obj = GetDrawable<dtCore::Object>();

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
   const dtCore::BaseActorObject::RenderMode& MeshTerrainActorProxy::GetRenderMode()
   {
      dtCore::ResourceDescriptor resource = dynamic_cast<dtCore::ResourceActorProperty*>(GetProperty("terrain mesh"))->GetValue();
      if (resource.IsEmpty() == false)
      {
         if (resource.GetResourceIdentifier().empty() || GetDrawable()->GetOSGNode() == NULL)
            return dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON;
         else
            return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR;
      }
      else
         return dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::ActorProxyIcon* MeshTerrainActorProxy::GetBillBoardIcon()
   {
      if (!mBillBoardIcon.valid())
      {
         mBillBoardIcon = new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_MESHTERRAIN);
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
