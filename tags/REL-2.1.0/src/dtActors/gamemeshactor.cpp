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
#include <dtGame/gamemanager.h>
#include <dtGame/actorupdatemessage.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproxyicon.h>

#include <osg/MatrixTransform>

namespace dtActors
{
   //////////////////////////////////////////////////////////////////////////////
   /////////////////////////// BEGIN ACTOR //////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////////
   GameMeshActor::GameMeshActor(dtGame::GameActorProxy &proxy) : 
      dtGame::GameActor(proxy), 
      mAlreadyInScene(false),
      mUseCache(true), 
      mModel(new dtCore::Model)
   {
      GetMatrixNode()->addChild(&mModel->GetMatrixTransform());
   }

   ///////////////////////////////////////////////////////////////////////////////
   GameMeshActor::~GameMeshActor()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameMeshActor::SetMesh(const std::string &meshFile)
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
      osg::Node *model = mLoader.LoadFile(mLoader.GetFilename(), mUseCache);

      if (model != NULL)
      {
         if (mMeshNode.valid())
         {
            mModel->GetMatrixTransform().removeChild(mMeshNode.get());
         }
         
         mModel->GetMatrixTransform().addChild(model);
         mModel->SetDirty();

         mMeshNode = model;

         GetGameActorProxy().SetCollisionType( GetGameActorProxy().GetCollisionType() );
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
   void GameMeshActor::SetScale(const osg::Vec3 &xyz)
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

      GameMeshActor &myActor = static_cast<GameMeshActor &>(GetGameActor());

      AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::STATIC_MESH,
         "static mesh", "Static Mesh", dtDAL::MakeFunctor(myActor, &GameMeshActor::SetMesh),
         "The static mesh resource that defines the geometry", GROUPNAME));

      AddProperty(new dtDAL::BooleanActorProperty("use cache object", "Use Model Cache", 
         dtDAL::MakeFunctor(myActor, &GameMeshActor::SetUseCache),
         dtDAL::MakeFunctorRet(myActor, &GameMeshActor::GetUseCache),
         "Indicates whether we will try to use the cache when we load our model.", GROUPNAME));

      AddProperty(new dtDAL::Vec3ActorProperty("Scale", "Scale", 
         dtDAL::MakeFunctor(myActor, &GameMeshActor::SetScale), 
         dtDAL::MakeFunctorRet(myActor, &GameMeshActor::GetScale), 
         "Scales this visual model", "Transformable"));
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

      // set the default name of this object so they show up somewhat nicely in STAGE
      // obviously, it's not guaranteed to be unique
      static int actorCount = 0;
      std::ostringstream ss;
      ss << "StaticMesh" << actorCount++;
      SetName(ss.str());
   }

   ///////////////////////////////////////////////////////////////////////////////
   const dtDAL::ActorProxy::RenderMode &GameMeshActorProxy::GetRenderMode()
   {
      dtDAL::ResourceDescriptor *resource = GetResource("static mesh");
      if (resource != NULL)
      {
         if (resource->GetResourceIdentifier().empty() || GetActor()->GetOSGNode() == NULL)
               return dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON;
         else
               return dtDAL::ActorProxy::RenderMode::DRAW_ACTOR;
      }
      else
         return dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON;
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProxyIcon *GameMeshActorProxy::GetBillBoardIcon()
   {
      if(!mBillBoardIcon.valid())
         mBillBoardIcon = new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IconType::STATICMESH);   

      return mBillBoardIcon.get();
   }

}
