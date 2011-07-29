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

#include <dtCore/actoridactorproperty.h>
#include <dtCore/actorproxyicon.h>
#include <dtCore/arrayactorproperty.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/datatype.h>
#include <dtCore/functor.h>
#include <dtCore/mapxml.h>
#include <dtCore/project.h>
#include <dtCore/resourceactorproperty.h>

#include <dtGame/basemessages.h>
#include <dtGame/gamemanager.h>
#include <dtGame/invokable.h>
#include <dtGame/message.h>
#include <dtGame/messagetype.h>

#include <dtDirector/director.h>

#include <dtUtil/exception.h>

namespace dtActors
{
   /////////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   DirectorActor::DirectorActor(dtGame::GameActorProxy& parent)
      : BaseClass(parent)
      , mNodeLogging(false)
      , mResourceIndex(0)
   {
      SetName("Director_Graph_Actor");
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
         if (director && director->GetActive())
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

      dtGame::GameManager* gm = GetGameActorProxy().GetGameManager();

      // Find the map that this actor belongs to.
      dtCore::Map* map = dtCore::Project::GetInstance().GetMapForActorProxy(GetUniqueId());

      int count = (int)mResourceList.size();
      for (int index = 0; index < count; index++)
      {
         dtCore::RefPtr<dtDirector::Director> director = new dtDirector::Director();
         if (director.valid())
         {
            dtCore::ResourceDescriptor& descriptor = mResourceList[index];

            director->Init(gm, map);

            director->SetNodeLogging(mNodeLogging);
            LOG_INFO(dtCore::Project::GetInstance().GetResourcePath(descriptor));
            director->LoadScript(dtCore::Project::GetInstance().GetResourcePath(descriptor));

            mDirectorList.push_back(director);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorActor::SetDirectorResource(const dtCore::ResourceDescriptor& value)
   {
      if (mResourceIndex >= 0 && mResourceIndex < (int)mResourceList.size())
      {
         mResourceList[mResourceIndex] = value;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::ResourceDescriptor DirectorActor::GetDirectorResource()
   {
      if (mResourceIndex >= 0 && mResourceIndex < (int)mResourceList.size())
      {
         return mResourceList[mResourceIndex];
      }

      return dtCore::ResourceDescriptor::NULL_RESOURCE;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorActor::SetDirectorIndex(int index)
   {
      mResourceIndex = index;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::ResourceDescriptor DirectorActor::GetDefaultDirector()
   {
      return dtCore::ResourceDescriptor::NULL_RESOURCE;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<dtCore::ResourceDescriptor> DirectorActor::GetDirectorArray() const
   {
      return mResourceList;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorActor::SetDirectorArray(const std::vector<dtCore::ResourceDescriptor>& value)
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

      AddProperty(new dtCore::BooleanActorProperty(
         "NodeLogging", "Node Logging",
         dtCore::BooleanActorProperty::SetFuncType(actor, &DirectorActor::SetNodeLogging),
         dtCore::BooleanActorProperty::GetFuncType(actor, &DirectorActor::GetNodeLogging),
         "Sets the Director Graphs to log the execution of their nodes.",
         "Director"));

      dtCore::ResourceActorProperty* scriptProp = new dtCore::ResourceActorProperty(
         dtCore::DataType::DIRECTOR, "DirectorGraph", "Director Script",
         dtCore::ResourceActorProperty::SetDescFuncType(actor, &DirectorActor::SetDirectorResource),
         dtCore::ResourceActorProperty::GetDescFuncType(actor, &DirectorActor::GetDirectorResource),
         "A Director Script Resource.", "Director");

      dtCore::ArrayActorPropertyBase* scriptArrayProp =
         new dtCore::ArrayActorProperty<dtCore::ResourceDescriptor>(
         "DirectorArray", "Director Script List",
         "The Director Graphs loaded by this actor.",
         dtCore::ArrayActorProperty<dtCore::ResourceDescriptor>::SetIndexFuncType(actor, &DirectorActor::SetDirectorIndex),
         dtCore::ArrayActorProperty<dtCore::ResourceDescriptor>::GetDefaultFuncType(actor, &DirectorActor::GetDefaultDirector),
         dtCore::ArrayActorProperty<dtCore::ResourceDescriptor>::GetArrayFuncType(actor, &DirectorActor::GetDirectorArray),
         dtCore::ArrayActorProperty<dtCore::ResourceDescriptor>::SetArrayFuncType(actor, &DirectorActor::SetDirectorArray),
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
            dtUtil::MakeFunctor(&DirectorActor::OnLoadDirectors, *actor)));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   const dtCore::BaseActorObject::RenderMode& DirectorActorProxy::GetRenderMode()
   {
      return dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::ActorProxyIcon* DirectorActorProxy::GetBillBoardIcon()
   {
      if(!mBillBoardIcon.valid())
      {
         mBillBoardIcon = new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_DIRECTOR);
      }

      return mBillBoardIcon.get();
   }
}
