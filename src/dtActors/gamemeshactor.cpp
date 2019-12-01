/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * Curtiss Murphy
 */
#include <dtActors/gamemeshactor.h>

#include <dtCore/transform.h>

#include <dtCore/actorproxyicon.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/functor.h>
#include <dtCore/resourceactorproperty.h>
#include <dtCore/vectoractorproperties.h>
#include <dtCore/object.h>

#include <dtGame/actorupdatemessage.h>
#include <dtGame/gamemanager.h>

#include <osg/MatrixTransform>

namespace dtActors
{
   //////////////////////////////////////////////////////////////////////////////
   GameMeshActor::GameMeshActor()
   {
      static dtUtil::RefString name("StaticMesh");
      static dtUtil::RefString className("dtActors::GameMeshActor");
      SetName(name);
      SetClassName(className);
   }

   //////////////////////////////////////////////////////////////////////////////
   GameMeshActor::~GameMeshActor()
   {
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameMeshActor::BuildPropertyMap()
   {
      static dtUtil::RefString GROUPNAME = "GameMesh";

      dtGame::GameActorProxy::BuildPropertyMap();

      dtCore::Object* draw = nullptr;
      GetDrawable(draw);

      AddProperty(new dtCore::ResourceActorProperty(dtCore::DataType::STATIC_MESH,
         "static mesh", "Static Mesh",
         dtCore::ResourceActorProperty::SetDescFuncType(draw, &dtCore::Object::SetMeshResource),
         dtCore::ResourceActorProperty::GetDescFuncType(draw, &dtCore::Object::GetMeshResource),
         "The static mesh resource that defines the geometry", GROUPNAME));

      AddProperty(new dtCore::BooleanActorProperty("use cache object", "Use Model Cache",
         dtCore::BooleanActorProperty::SetFuncType(draw, &dtCore::Object::SetUseCache),
         dtCore::BooleanActorProperty::GetFuncType(draw, &dtCore::Object::GetUseCache),
         "Indicates whether we will try to use the cache when we load our model.", GROUPNAME));

      AddProperty(new dtCore::BooleanActorProperty("RecenterGeometry", "RecenterGeometry",
         dtCore::BooleanActorProperty::SetFuncType(draw, &dtCore::Object::SetRecenterGeometryUponLoad),
         dtCore::BooleanActorProperty::GetFuncType(draw, &dtCore::Object::GetRecenterGeometryUponLoad),
         "If the loading process should recenter the geometry to make the origin the center of the bounding box.", GROUPNAME));

      AddProperty(new dtCore::BooleanActorProperty("GenerateTangents", "GenerateTangents",
         dtCore::BooleanActorProperty::SetFuncType(draw, &dtCore::Object::SetGenerateTangents),
         dtCore::BooleanActorProperty::GetFuncType(draw, &dtCore::Object::GetGenerateTangents),
         "If the loading process should re-center the geometry to make the origin the center of the bounding box.", GROUPNAME));

      AddProperty(new dtCore::Vec3ActorProperty("Scale", "Scale",
         dtCore::Vec3ActorProperty::SetFuncType(draw, &dtCore::Object::SetScale),
         dtCore::Vec3ActorProperty::GetFuncType(draw, &dtCore::Object::GetScale),
         "Scales this visual model", "Transformable"));

      AddProperty(new dtCore::Vec3ActorProperty("Model Rotation", "Model Rotation",
         dtCore::Vec3ActorProperty::SetFuncType(draw, &dtCore::Object::SetModelRotation),
         dtCore::Vec3ActorProperty::GetFuncType(draw, &dtCore::Object::GetModelRotation),
         "Specifies the Rotation of the object",
         "Transformable"));

      AddProperty(new dtCore::Vec3ActorProperty("Model Translation", "Model Translation",
         dtCore::Vec3ActorProperty::SetFuncType(draw, &dtCore::Object::SetModelTranslation),
         dtCore::Vec3ActorProperty::GetFuncType(draw, &dtCore::Object::GetModelTranslation),
         "Specifies the Translation of the object",
         "Transformable"));
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameMeshActor::BuildInvokables()
   {
      dtGame::GameActorProxy::BuildInvokables();
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameMeshActor::CreateDrawable()
   {
      SetDrawable(*new dtCore::Object());
   }

   ///////////////////////////////////////////////////////////////////////////////
   const dtCore::BaseActorObject::RenderMode& GameMeshActor::GetRenderMode()
   {
      dtCore::ResourceDescriptor resource = GetDrawable<dtCore::Object>()->GetMeshResource();
      if (resource.IsEmpty() == false)
      {
         if (resource.GetResourceIdentifier().empty() || GetDrawable()->GetOSGNode() == nullptr)
         {
               return dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON;
         }
         else
         {
               return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR;
         }
      }
      else
         return dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::ActorProxyIcon* GameMeshActor::GetBillBoardIcon()
   {
      if (!mBillBoardIcon.valid())
      {
         dtCore::ActorProxyIcon::ActorProxyIconConfig cfg(false,false,1.0f);
         mBillBoardIcon = new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_STATICMESH, cfg);
      }

      return mBillBoardIcon.get();
   }

}
