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

#include <dtGame/actorupdatemessage.h>
#include <dtGame/gamemanager.h>

#include <osg/MatrixTransform>

namespace dtActors
{
   //////////////////////////////////////////////////////////////////////////////
   /////////////////////////// BEGIN ACTOR //////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////////
   GameMeshDrawable::GameMeshDrawable(dtGame::GameActorProxy& parent)
      : dtGame::GameActor(parent)
      , mUseCache(true)
      , mModel(new dtCore::Model)
      , mMeshNode(NULL)
   {
      GetMatrixNode()->addChild(&mModel->GetMatrixTransform());
   }

   ///////////////////////////////////////////////////////////////////////////////
   GameMeshDrawable::~GameMeshDrawable()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameMeshDrawable::SetMesh(const std::string& meshFile)
   {
      // Make sure the mesh changed.
      if (mLoader.GetFilename() != meshFile)
      {
         mLoader.SetMeshFilename(meshFile);

         // For the initial setting, load the mesh when we first enter the world so we can use the cache variable
         if (GetSceneParent())
         {
            LoadMesh();
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameMeshDrawable::LoadMesh()
   {
      osg::Node* model = mLoader.LoadFile(mLoader.GetFilename(), mUseCache);

      if (model != NULL)
      {
         if (mMeshNode.valid())
         {
            mModel->GetMatrixTransform().removeChild(mMeshNode.get());
         }

         mModel->GetMatrixTransform().addChild(model);
         mModel->SetDirty();

         mMeshNode = model;

         GetGameActorProxy().SetCollisionType(GetGameActorProxy().GetCollisionType());
      }
      else
      {
         LOG_ERROR("Unable to load model file: " + mLoader.GetFilename());
      }

      // send out an ActorUpdateMessage
      // GameMeshActor no longer does this by default. It is possible that the property was
      // set using a message, and it is almost certain that the user does not want to do a full
      // actor update.
      // if (mAlreadyInWorld)
      //    GetGameActorProxy().NotifyFullActorUpdate();
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameMeshDrawable::AddedToScene(dtCore::Scene* scene)
   {
      dtGame::GameActor::AddedToScene(scene);

      // Don't load the mesh if we're not
      // really being added to the scene
      if (scene != NULL)
      {
         if (!mLoader.GetFilename().empty())
         {
            LoadMesh();
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameMeshDrawable::SetScale(const osg::Vec3& xyz)
   {
      mModel->SetScale(xyz);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameMeshDrawable::SetModelRotation(const osg::Vec3& v3)
   {
      dtCore::Transform ourTransform;
      mModel->GetTransform(ourTransform);
      ourTransform.SetRotation(v3);
      mModel->SetTransform(ourTransform);
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 GameMeshDrawable::GetModelRotation()
   {
      osg::Vec3 v3;
      dtCore::Transform ourTransform;
      mModel->GetTransform(ourTransform);
      ourTransform.GetRotation(v3);
      return v3;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameMeshDrawable::SetModelTranslation(const osg::Vec3& v3)
   {
      dtCore::Transform ourTransform;
      mModel->GetTransform(ourTransform);
      ourTransform.SetTranslation(v3);
      mModel->SetTransform(ourTransform);
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 GameMeshDrawable::GetModelTranslation()
   {
      osg::Vec3 v3;
      dtCore::Transform ourTransform;
      mModel->GetTransform(ourTransform);
      ourTransform.GetTranslation(v3);
      return v3;
   }

   //////////////////////////////////////////////////////////////////////////////
   osg::Vec3 GameMeshDrawable::GetScale() const
   {
      osg::Vec3 scale;
      mModel->GetScale(scale);
      return scale;
   }

   //////////////////////////////////////////////////////////////////////////////
   osg::MatrixTransform& GameMeshDrawable::GetMatrixTransform()
   {
      return mModel->GetMatrixTransform();
   }

   //////////////////////////////////////////////////////////////////////////////
   const osg::MatrixTransform& GameMeshDrawable::GetMatrixTransform() const
   {
      return mModel->GetMatrixTransform();
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Node* GameMeshDrawable::GetMeshNode()
   {
      return mMeshNode.get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   const osg::Node* GameMeshDrawable::GetMeshNode() const
   {
      return mMeshNode.get();
   }

   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////// END ACTOR ///////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////


   //////////////////////////////////////////////////////////////////////////////
   /////////////////////////// BEGIN PROXY //////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////////
   GameMeshActor::GameMeshActor()
   {
      SetClassName("dtActors::GameMeshActor");
      SetName("StaticMesh");
   }

   //////////////////////////////////////////////////////////////////////////////
   GameMeshActor::~GameMeshActor()
   {
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameMeshActor::BuildPropertyMap()
   {
      const std::string GROUPNAME = "GameMesh";

      dtGame::GameActorProxy::BuildPropertyMap();

      GameMeshDrawable* myActor = NULL;
      GetDrawable(myActor);

      AddProperty(new dtCore::ResourceActorProperty(dtCore::DataType::STATIC_MESH,
         "static mesh", "Static Mesh",
         dtCore::ResourceActorProperty::SetDescFuncType(this, &GameMeshActor::SetMeshResource),
         dtCore::ResourceActorProperty::GetDescFuncType(this, &GameMeshActor::GetMeshResource),
         "The static mesh resource that defines the geometry", GROUPNAME));

      AddProperty(new dtCore::BooleanActorProperty("use cache object", "Use Model Cache",
         dtCore::BooleanActorProperty::SetFuncType(myActor, &GameMeshDrawable::SetUseCache),
         dtCore::BooleanActorProperty::GetFuncType(myActor, &GameMeshDrawable::GetUseCache),
         "Indicates whether we will try to use the cache when we load our model.", GROUPNAME));

      AddProperty(new dtCore::Vec3ActorProperty("Scale", "Scale",
         dtCore::Vec3ActorProperty::SetFuncType(myActor, &GameMeshDrawable::SetScale),
         dtCore::Vec3ActorProperty::GetFuncType(myActor, &GameMeshDrawable::GetScale),
         "Scales this visual model", "Transformable"));

      AddProperty(new dtCore::Vec3ActorProperty("Model Rotation", "Model Rotation",
         dtCore::Vec3ActorProperty::SetFuncType(myActor, &GameMeshDrawable::SetModelRotation),
         dtCore::Vec3ActorProperty::GetFuncType(myActor, &GameMeshDrawable::GetModelRotation),
         "Specifies the Rotation of the object",
         "Transformable"));

      AddProperty(new dtCore::Vec3ActorProperty("Model Translation", "Model Translation",
         dtCore::Vec3ActorProperty::SetFuncType(myActor, &GameMeshDrawable::SetModelTranslation),
         dtCore::Vec3ActorProperty::GetFuncType(myActor, &GameMeshDrawable::GetModelTranslation),
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
      SetDrawable(*new GameMeshDrawable(*this));

      //// set the default name of this object so they show up somewhat nicely in STAGE
      //// obviously, it's not guaranteed to be unique
      //static int actorCount = 0;
      //std::ostringstream ss;
      //ss << "StaticMesh" << actorCount++;
      //SetName(ss.str());
   }

   ///////////////////////////////////////////////////////////////////////////////
   const dtCore::BaseActorObject::RenderMode& GameMeshActor::GetRenderMode()
   {
      dtCore::ResourceDescriptor resource = GetMeshResource();
      if (resource.IsEmpty() == false)
      {
         if (resource.GetResourceIdentifier().empty() || GetDrawable()->GetOSGNode() == NULL)
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

   //////////////////////////////////////////////////////////////////////////
   DT_IMPLEMENT_ACCESSOR_GETTER(GameMeshActor, dtCore::ResourceDescriptor, MeshResource)
   void GameMeshActor::SetMeshResource(const dtCore::ResourceDescriptor& rd)
   {
      GameMeshDrawable* gmd = NULL;
      GetDrawable(gmd);
      gmd->SetMesh(dtCore::ResourceActorProperty::GetResourcePath(rd));
      mMeshResource = rd;
   }
}
