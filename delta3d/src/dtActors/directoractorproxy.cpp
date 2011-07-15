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

#include <dtDAL/actoridactorproperty.h>
#include <dtDAL/actorproxyicon.h>
#include <dtDAL/arrayactorproperty.h>
#include <dtDAL/booleanactorproperty.h>
#include <dtDAL/datatype.h>
#include <dtDAL/functor.h>
#include <dtDAL/mapxml.h>
#include <dtDAL/project.h>
#include <dtDAL/resourceactorproperty.h>

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
      dtDAL::Map* map = dtDAL::Project::GetInstance().GetMapForActorProxy(GetUniqueId());

      int count = (int)mResourceList.size();
      for (int index = 0; index < count; index++)
      {
         dtCore::RefPtr<dtDirector::Director> director = new dtDirector::Director();
         if (director.valid())
         {
            dtDAL::ResourceDescriptor& descriptor = mResourceList[index];

            director->Init(gm, map);

            director->SetNodeLogging(mNodeLogging);
            LOG_INFO(dtDAL::Project::GetInstance().GetResourcePath(descriptor));
            director->LoadScript(dtDAL::Project::GetInstance().GetResourcePath(descriptor));

            mDirectorList.push_back(director);
         }
      }
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

      dtDAL::ResourceActorProperty* scriptProp = new dtDAL::ResourceActorProperty(
         dtDAL::DataType::DIRECTOR, "DirectorGraph", "Director Script",
         dtDAL::ResourceActorProperty::SetDescFuncType(actor, &DirectorActor::SetDirectorResource),
         dtDAL::ResourceActorProperty::GetDescFuncType(actor, &DirectorActor::GetDirectorResource),
         "A Director Script Resource.", "Director");

      dtDAL::ArrayActorPropertyBase* scriptArrayProp =
         new dtDAL::ArrayActorProperty<dtDAL::ResourceDescriptor>(
         "DirectorArray", "Director Script List",
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
            dtUtil::MakeFunctor(&DirectorActor::OnLoadDirectors, *actor)));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   const dtDAL::BaseActorObject::RenderMode& DirectorActorProxy::GetRenderMode()
   {
      return dtDAL::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON;
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
