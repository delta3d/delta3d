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
 * Jeff P. Houde
 */

#include <dtActors/directoractorproxy.h>
#include <dtABC/application.h>
#include <dtGame/gamemanager.h>
#include <dtDAL/datatype.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/resourceactorproperty.h>
#include <dtDAL/arrayactorproperty.h>
#include <dtDAL/mapxml.h>
#include <dtDAL/project.h>
#include <dtUtil/exception.h>
#include <dtDAL/functor.h>
#include <dtDAL/actorproxyicon.h>

#include <dtGame/basemessages.h>
#include <dtGame/message.h>
#include <dtGame/messagetype.h>
#include <dtGame/invokable.h>

namespace dtActors
{
   /////////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   DirectorActor::DirectorActor(dtGame::GameActorProxy& parent)
      : BaseClass(parent)
      , mNodeLogging(false)
      , mRecording(false)
      , mResourceIndex(0)
   {
      SetName("Director_Graph_Actor");
      mPlayerActor = "";
      mCameraActor = "";
   }

   /////////////////////////////////////////////////////////////////////////////
   DirectorActor::~DirectorActor()
   {
   }

   ////////////////////////////////////////////////////
   void DirectorActor::OnTickLocal(const dtGame::TickMessage& tickMessage)
   {
      BaseClass::OnTickLocal(tickMessage);

      float delta = tickMessage.GetDeltaRealTime();
      float simDelta = tickMessage.GetDeltaSimTime();

      int count = (int)mDirectorList.size();
      for (int index = 0; index < count; index++)
      {
         dtDirector::Director* director = mDirectorList[index].get();
         if (director)
         {
            director->Update(simDelta, delta);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorActor::OnLoadDirectors(const dtGame::Message&)
   {
      LoadDirectors();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorActor::LoadDirectors()
   {
      mDirectorList.clear();

      dtCore::UniqueId playerID = GetPlayerActor();
      dtCore::UniqueId cameraID = GetCameraActor();

      dtGame::GameManager* gm = GetGameActorProxy().GetGameManager();
      if (gm)
      {
         dtCore::Camera* cam = gm->GetApplication().GetCamera();

         if (cam)
         {
            if (playerID.ToString().empty())
            {
               playerID = cam->GetUniqueId();
            }

            if (cameraID.ToString().empty())
            {
               cameraID = cam->GetUniqueId();
            }
         }
      }

      // Find the map that this actor belongs to.
      dtDAL::Map* map = dtDAL::Project::GetInstance().GetMapForActorProxy(GetUniqueId());

      int count = (int)mResourceList.size();
      for (int index = 0; index < count; index++)
      {
         dtCore::RefPtr<dtDirector::Director> director = new dtDirector::Director();
         if (director.valid())
         {
            dtDAL::ResourceDescriptor& descriptor = mResourceList[index];

            director->Init(gm, map);
            director->SetPlayer(playerID);
            //director->SetCamera(cameraID);

            director->SetNodeLogging(mNodeLogging);
            if (mRecording) director->StartRecording();
LOG_ALWAYS(dtDAL::Project::GetInstance().GetResourcePath(descriptor));
            director->LoadScript(dtDAL::Project::GetInstance().GetResourcePath(descriptor));

            mDirectorList.push_back(director);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorActor::SetPlayerActor(const dtCore::UniqueId& value)
   {
      mPlayerActor = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const dtCore::UniqueId& DirectorActor::GetPlayerActor()
   {
      return mPlayerActor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorActor::SetCameraActor(const dtCore::UniqueId& value)
   {
      mCameraActor = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const dtCore::UniqueId& DirectorActor::GetCameraActor()
   {
      return mCameraActor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorActor::SetDirectorResource(const dtDAL::ResourceDescriptor& value)
   {
      if (mResourceIndex >= 0 && mResourceIndex < (int)mResourceList.size())
      {
         mResourceList[mResourceIndex] = value;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::ResourceDescriptor DirectorActor::GetDirectorResource()
   {
      if (mResourceIndex >= 0 && mResourceIndex < (int)mResourceList.size())
      {
         return mResourceList[mResourceIndex];
      }

      return dtDAL::ResourceDescriptor::NULL_RESOURCE;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorActor::SetDirectorIndex(int index)
   {
      mResourceIndex = index;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::ResourceDescriptor DirectorActor::GetDefaultDirector()
   {
      return dtDAL::ResourceDescriptor::NULL_RESOURCE;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<dtDAL::ResourceDescriptor> DirectorActor::GetDirectorArray() const
   {
      return mResourceList;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorActor::SetDirectorArray(const std::vector<dtDAL::ResourceDescriptor>& value)
   {
      mResourceList = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<dtCore::RefPtr<dtDirector::Director> >& DirectorActor::GetInstancedDirectors()
   {
      return mDirectorList;
   }


   /////////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   /////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString DirectorActorProxy::CLASS_NAME("dtActors::DirectorActor");

   /////////////////////////////////////////////////////////////////////////////
   DirectorActorProxy::DirectorActorProxy()
      : BaseClass()
   {
      SetClassName(CLASS_NAME.Get());
   }

   /////////////////////////////////////////////////////////////////////////////
   DirectorActorProxy::~DirectorActorProxy()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void DirectorActorProxy::CreateActor()
   {
      SetActor(*new DirectorActor(*this));
   }

   /////////////////////////////////////////////////////////////////////////////
   void DirectorActorProxy::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      DirectorActor* actor = NULL;
      GetActor(actor);

      AddProperty(new dtDAL::BooleanActorProperty(
         "NodeLogging", "Node Logging",
         dtDAL::BooleanActorProperty::SetFuncType(actor, &DirectorActor::SetNodeLogging),
         dtDAL::BooleanActorProperty::GetFuncType(actor, &DirectorActor::GetNodeLogging),
         "Sets the Director Graphs to log the execution of their nodes.",
         "Director"));

      AddProperty(new dtDAL::BooleanActorProperty(
         "Recording", "Recording",
         dtDAL::BooleanActorProperty::SetFuncType(actor, &DirectorActor::SetRecording),
         dtDAL::BooleanActorProperty::GetFuncType(actor, &DirectorActor::GetRecording),
         "Sets the Director Graphs to record a replay file.",
         "Director"));

      AddProperty(new dtDAL::ActorIDActorProperty(
         "PlayerActor", "Player Actor",
         dtDAL::ActorIDActorProperty::SetFuncType(actor, &DirectorActor::SetPlayerActor),
         dtDAL::ActorIDActorProperty::GetFuncType(actor, &DirectorActor::GetPlayerActor),
         "dtCore::Transformable", "The Player actor, if NULL it will use the Applications Camera by default.",
         "Director"));

      AddProperty(new dtDAL::ActorIDActorProperty(
         "CameraActor", "Camera Actor",
         dtDAL::ActorIDActorProperty::SetFuncType(actor, &DirectorActor::SetCameraActor),
         dtDAL::ActorIDActorProperty::GetFuncType(actor, &DirectorActor::GetCameraActor),
         "dtCore::Transformable", "The Camera actor, if NULL it will use the Applications Camera by default.",
         "Director"));

      dtDAL::ResourceActorProperty* scriptProp = new dtDAL::ResourceActorProperty(
         dtDAL::DataType::DIRECTOR, "DirectorGraph", "Director Graph",
         dtDAL::ResourceActorProperty::SetDescFuncType(actor, &DirectorActor::SetDirectorResource),
         dtDAL::ResourceActorProperty::GetDescFuncType(actor, &DirectorActor::GetDirectorResource),
         "A Director Graph Resource.", "Director");

      dtDAL::ArrayActorPropertyBase* scriptArrayProp =
         new dtDAL::ArrayActorProperty<dtDAL::ResourceDescriptor>(
         "DirectorArray", "Director Graph List",
         "The Director Graphs loaded by this actor.",
         dtDAL::ArrayActorProperty<dtDAL::ResourceDescriptor>::SetIndexFuncType(actor, &DirectorActor::SetDirectorIndex),
         dtDAL::ArrayActorProperty<dtDAL::ResourceDescriptor>::GetDefaultFuncType(actor, &DirectorActor::GetDefaultDirector),
         dtDAL::ArrayActorProperty<dtDAL::ResourceDescriptor>::GetArrayFuncType(actor, &DirectorActor::GetDirectorArray),
         dtDAL::ArrayActorProperty<dtDAL::ResourceDescriptor>::SetArrayFuncType(actor, &DirectorActor::SetDirectorArray),
         scriptProp, "Director");
      AddProperty(scriptArrayProp);
   }

   //////////////////////////////////////////////////////////////////////////////
   void DirectorActorProxy::OnEnteredWorld()
   {
      BaseClass::OnEnteredWorld();

      if (!IsInSTAGE())
      {
         RegisterForMessages(dtGame::MessageType::TICK_LOCAL, dtGame::GameActorProxy::TICK_LOCAL_INVOKABLE);

         RegisterForMessages(dtGame::MessageType::INFO_MAP_LOADED, "Map Loaded");
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorActorProxy::BuildInvokables()
   {
      BaseClass::BuildInvokables();

      if (!IsInSTAGE())
      {
         DirectorActor* actor = NULL;
         GetActor(actor);

         AddInvokable(*new dtGame::Invokable("Map Loaded",
            dtDAL::MakeFunctor(*actor, &DirectorActor::OnLoadDirectors)));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   const dtDAL::ActorProxy::RenderMode& DirectorActorProxy::GetRenderMode()
   {
      return dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON;
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProxyIcon* DirectorActorProxy::GetBillBoardIcon()
   {
      if(!mBillBoardIcon.valid())
      {
         mBillBoardIcon = new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IMAGE_BILLBOARD_DIRECTOR);
      }

      return mBillBoardIcon.get();
   }
}
