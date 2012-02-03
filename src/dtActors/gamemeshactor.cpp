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
   GameMeshActor::GameMeshActor(dtGame::GameActorProxy& proxy)
      : dtGame::GameActor(proxy)
      , mAlreadyInScene(false)
      , mUseCache(true)
      , mModel(new dtCore::Model)
      , mMeshNode(NULL)
   {
      GetMatrixNode()->addChild(&mModel->GetMatrixTransform());
      SetName("StaticMesh");
   }

   ///////////////////////////////////////////////////////////////////////////////
   GameMeshActor::~GameMeshActor()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameMeshActor::SetMesh(const std::string& meshFile)
   {
      // Make sure the mesh changed.
      if (mLoader.GetFilename() != meshFile)
      {
         mLoader.SetMeshFilename(meshFile);

         // For the initial setting, load the mesh when we first enter the world so we can use the cache variable
         if (mAlreadyInScene)
         {
            LoadMesh();
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameMeshActor::LoadMesh()
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
   void GameMeshActor::AddedToScene(dtCore::Scene* scene)
   {
      dtGame::GameActor::AddedToScene(scene);

      // Don't load the mesh if we're not
      // really being added to the scene
      if (scene != NULL)
      {
         mAlreadyInScene = true;

         if (!mLoader.GetFilename().empty())
         {
            LoadMesh();
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameMeshActor::SetScale(const osg::Vec3& xyz)
   {
      mModel->SetScale(xyz);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameMeshActor::SetModelRotation(const osg::Vec3& v3)
   {
      dtCore::Transform ourTransform;
      mModel->GetTransform(ourTransform);
      ourTransform.SetRotation(v3);
      mModel->SetTransform(ourTransform);
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 GameMeshActor::GetModelRotation()
   {
      osg::Vec3 v3;
      dtCore::Transform ourTransform;
      mModel->GetTransform(ourTransform);
      ourTransform.GetRotation(v3);
      return v3;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameMeshActor::SetModelTranslation(const osg::Vec3& v3)
   {
      dtCore::Transform ourTransform;
      mModel->GetTransform(ourTransform);
      ourTransform.SetTranslation(v3);
      mModel->SetTransform(ourTransform);
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 GameMeshActor::GetModelTranslation()
   {
      osg::Vec3 v3;
      dtCore::Transform ourTransform;
      mModel->GetTransform(ourTransform);
      ourTransform.GetTranslation(v3);
      return v3;
   }

   //////////////////////////////////////////////////////////////////////////////
   osg::Vec3 GameMeshActor::GetScale() const
   {
      osg::Vec3 scale;
      mModel->GetScale(scale);
      return scale;
   }

   //////////////////////////////////////////////////////////////////////////////
   osg::MatrixTransform& GameMeshActor::GetMatrixTransform()
   {
      return mModel->GetMatrixTransform();
   }

   //////////////////////////////////////////////////////////////////////////////
   const osg::MatrixTransform& GameMeshActor::GetMatrixTransform() const
   {
      return mModel->GetMatrixTransform();
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Node* GameMeshActor::GetMeshNode()
   {
      return mMeshNode.get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   const osg::Node* GameMeshActor::GetMeshNode() const
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
   GameMeshActorProxy::GameMeshActorProxy()
   {
      SetClassName("dtActors::GameMeshActor");
   }

   //////////////////////////////////////////////////////////////////////////////
   GameMeshActorProxy::~GameMeshActorProxy()
   {
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameMeshActorProxy::BuildPropertyMap()
   {
      const std::string GROUPNAME = "GameMesh";

      dtGame::GameActorProxy::BuildPropertyMap();

      GameMeshActor& myActor = static_cast<GameMeshActor &>(GetGameActor());

      AddProperty(new dtCore::ResourceActorProperty(*this, dtCore::DataType::STATIC_MESH,
         "static mesh", "Static Mesh", dtCore::ResourceActorProperty::SetFuncType(&myActor, &GameMeshActor::SetMesh),
         "The static mesh resource that defines the geometry", GROUPNAME));

      AddProperty(new dtCore::BooleanActorProperty("use cache object", "Use Model Cache",
         dtCore::BooleanActorProperty::SetFuncType(&myActor, &GameMeshActor::SetUseCache),
         dtCore::BooleanActorProperty::GetFuncType(&myActor, &GameMeshActor::GetUseCache),
         "Indicates whether we will try to use the cache when we load our model.", GROUPNAME));

      AddProperty(new dtCore::Vec3ActorProperty("Scale", "Scale",
         dtCore::Vec3ActorProperty::SetFuncType(&myActor, &GameMeshActor::SetScale),
         dtCore::Vec3ActorProperty::GetFuncType(&myActor, &GameMeshActor::GetScale),
         "Scales this visual model", "Transformable"));

      AddProperty(new dtCore::Vec3ActorProperty("Model Rotation", "Model Rotation",
         dtCore::Vec3ActorProperty::SetFuncType(&myActor, &GameMeshActor::SetModelRotation),
         dtCore::Vec3ActorProperty::GetFuncType(&myActor, &GameMeshActor::GetModelRotation),
         "Specifies the Rotation of the object",
         "Transformable"));

      AddProperty(new dtCore::Vec3ActorProperty("Model Translation", "Model Translation",
         dtCore::Vec3ActorProperty::SetFuncType(&myActor, &GameMeshActor::SetModelTranslation),
         dtCore::Vec3ActorProperty::GetFuncType(&myActor, &GameMeshActor::GetModelTranslation),
         "Specifies the Translation of the object",
         "Transformable"));
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameMeshActorProxy::BuildInvokables()
   {
      dtGame::GameActorProxy::BuildInvokables();
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameMeshActorProxy::CreateActor()
   {
      SetActor(*new GameMeshActor(*this));

      //// set the default name of this object so they show up somewhat nicely in STAGE
      //// obviously, it's not guaranteed to be unique
      //static int actorCount = 0;
      //std::ostringstream ss;
      //ss << "StaticMesh" << actorCount++;
      //SetName(ss.str());
   }

   ///////////////////////////////////////////////////////////////////////////////
   const dtCore::BaseActorObject::RenderMode& GameMeshActorProxy::GetRenderMode()
   {
      dtCore::ResourceDescriptor resource = GetResource("static mesh");
      if (resource.IsEmpty() == false)
      {
         if (resource.GetResourceIdentifier().empty() || GetActor()->GetOSGNode() == NULL)
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
   dtCore::ActorProxyIcon* GameMeshActorProxy::GetBillBoardIcon()
   {
      if (!mBillBoardIcon.valid())
      {
         dtCore::ActorProxyIcon::ActorProxyIconConfig cfg(false,false,1.f);
         mBillBoardIcon = new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_STATICMESH, cfg);
      }

      return mBillBoardIcon.get();
   }
}
