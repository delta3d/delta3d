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
#include <dtActors/animationgameactor.h>
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
   AnimationGameActor::AnimationGameActor(dtGame::GameActorProxy &proxy)
      :dtGame::GameActor(proxy)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   AnimationGameActor::~AnimationGameActor()
   {
   }
    

   //////////////////////////////////////////////////////////////////////////////
   void AnimationGameActor::AddedToScene(dtCore::Scene* scene)
   {
      dtGame::GameActor::AddedToScene(scene);
   }

   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////// END ACTOR ///////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////


   //////////////////////////////////////////////////////////////////////////////
   /////////////////////////// BEGIN PROXY //////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////////
   AnimationGameActorProxy::AnimationGameActorProxy()
   {
      SetClassName("dtActors::GameMeshActor");
   }

   //////////////////////////////////////////////////////////////////////////////
   AnimationGameActorProxy::~AnimationGameActorProxy()
   {
   }

   //////////////////////////////////////////////////////////////////////////////
   void AnimationGameActorProxy::BuildPropertyMap()
   {
      const std::string GROUPNAME = "GameMesh";

      dtGame::GameActorProxy::BuildPropertyMap();

      //GameMeshActor &myActor = static_cast<GameMeshActor &>(GetGameActor());

      //AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::STATIC_MESH,
      //   "static mesh", "Static Mesh", dtDAL::MakeFunctor(myActor, &GameMeshActor::SetMesh),
      //   "The static mesh resource that defines the geometry", GROUPNAME));

      //AddProperty(new dtDAL::BooleanActorProperty("use cache object", "Use Model Cache", 
      //   dtDAL::MakeFunctor(myActor, &GameMeshActor::SetUseCache),
      //   dtDAL::MakeFunctorRet(myActor, &GameMeshActor::GetUseCache),
      //   "Indicates whether we will try to use the cache when we load our model.", GROUPNAME));
   }

   //////////////////////////////////////////////////////////////////////////////
   void AnimationGameActorProxy::BuildInvokables()
   {
      dtGame::GameActorProxy::BuildInvokables();
   }

   //////////////////////////////////////////////////////////////////////////////
   void AnimationGameActorProxy::CreateActor()
   {
      SetActor(*new AnimationGameActor(*this));   
   }

}