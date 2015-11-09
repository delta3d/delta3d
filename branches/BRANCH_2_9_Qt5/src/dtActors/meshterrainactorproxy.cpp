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
#include <dtCore/object.h>

using namespace dtCore;

namespace dtActors
{


   ///////////////////////////////////////////////////////////////////////////////
   MeshTerrainActor::MeshTerrainActor()
   {
      SetClassName("dtCore::Object");
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MeshTerrainActor::BuildPropertyMap()
   {
      const std::string& GROUPNAME = "Terrain";
      BaseClass::BuildPropertyMap();

      RemoveProperty("static mesh");
      dtCore::Object* drawable = GetDrawable<dtCore::Object>();

      AddProperty(new dtCore::ResourceActorProperty(dtCore::DataType::TERRAIN,
         "terrain mesh", "Terrain Mesh",
         dtCore::ResourceActorProperty::SetDescFuncType(drawable, &dtCore::Object::SetMeshResource),
         dtCore::ResourceActorProperty::GetDescFuncType(drawable, &dtCore::Object::GetMeshResource),
         "The static mesh resource that defines the geometry", GROUPNAME));
   }

   ///////////////////////////////////////////////////////////////////////////////
   const dtCore::BaseActorObject::RenderMode& MeshTerrainActor::GetRenderMode()
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
   dtCore::ActorProxyIcon* MeshTerrainActor::GetBillBoardIcon()
   {
      if (!mBillBoardIcon.valid())
      {
         mBillBoardIcon = new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_MESHTERRAIN);
      }

      return mBillBoardIcon.get();
   }

}
