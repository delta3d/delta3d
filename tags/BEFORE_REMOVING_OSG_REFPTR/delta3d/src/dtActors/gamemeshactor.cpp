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
 * @author Curtiss Murphy
 */
#include "dtActors/gamemeshactor.h"
//#include <dtGame/exceptionenum.h>
#include <dtGame/gamemanager.h>
#include <dtGame/actorupdatemessage.h>
#include <dtDAL/enginepropertytypes.h>
//#include "dtDAL/resourcedescriptor.h"
#include "dtDAL/actorproxyicon.h"


namespace dtActors
{
   //////////////////////////////////////////////////////////////////////////////
   /////////////////////////// BEGIN ACTOR //////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////////
   GameMeshActor::GameMeshActor(dtGame::GameActorProxy &proxy) : 
      dtGame::GameActor(proxy), 
      mAlreadyInWorld(false)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   GameMeshActor::~GameMeshActor()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GameMeshActor::SetMesh(const std::string &meshFile)
   {
      GameMeshActor::GameMeshLoader loader; 

      // Load up the mesh, but do NOT cache. See class comments.
      osg::Node *model = loader.LoadFile(meshFile,false);

      if (model != NULL)
      {
         if (GetMatrixNode()->getNumChildren() != 0)
            GetMatrixNode()->removeChild(0,GetMatrixNode()->getNumChildren());
         GetMatrixNode()->addChild(model);

         GetGameActorProxy().SetCollisionType( GetGameActorProxy().GetCollisionType() );
      }
      else
      {
         LOG_ERROR("Unable to load model file: " + meshFile);
      }

      // send out an ActorUpdateMessage
      if (mAlreadyInWorld)
         GetGameActorProxy().NotifyFullActorUpdate();
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameMeshActor::OnEnteredWorld()
   {
      mAlreadyInWorld = true;
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
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameMeshActorProxy::BuildInvokables()
   {
      dtGame::GameActorProxy::BuildInvokables();
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameMeshActorProxy::CreateActor()
   {
      mActor = new GameMeshActor(*this);

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
         if (resource->GetResourceIdentifier().empty() || mActor->GetOSGNode() == NULL)
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
