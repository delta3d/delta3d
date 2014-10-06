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
 * Matthew W. Campbell
 */
#include <prefix/dtgameprefix.h>
#include <dtGame/taskcomponent.h>

#include <dtCore/actortype.h>
#include <dtCore/booleanactorproperty.h>

#include <dtGame/actorupdatemessage.h>
#include <dtGame/basemessages.h>
#include <dtGame/gameactorproxy.h>
#include <dtGame/message.h>
#include <dtGame/messagetype.h>
#include <dtGame/messagefactory.h>

namespace dtGame
{
   //////////////////////////////////////////////////////////////////////////
   TaskComponent::TaskComponent(dtCore::SystemComponentType& type)
   : GMComponent(type)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   TaskComponent::~TaskComponent()
   {
      ClearTaskList();
   }

   //////////////////////////////////////////////////////////////////////////
   void TaskComponent::ProcessMessage(const Message &message)
   {
      const dtCore::UniqueId &id = message.GetAboutActorId();
      dtGame::GameActorProxy *proxy;

      if (message.GetMessageType() == MessageType::INFO_ACTOR_CREATED ||
         message.GetMessageType() == MessageType::INFO_ACTOR_UPDATED ||
         message.GetMessageType() == MessageType::INFO_ACTOR_PUBLISHED)
      {
         proxy = GetGameManager()->FindGameActorById(id);

         if (proxy == NULL)
         {
            //LOG_WARNING("Newly created actor could not be found in the game manager.");
            return;
         }
      
         if (proxy->GetActorType().InstanceOf("dtcore.Tasks","Task Actor"))
         {
            InsertTaskActor(*proxy);
         }
      }
      else if (message.GetMessageType() == MessageType::INFO_ACTOR_DELETED)
      {
         const dtCore::UniqueId &id = message.GetAboutActorId();
         dtGame::GameActorProxy *proxy = GetGameManager()->FindGameActorById(id);

         if (proxy == NULL)
         {
            LOG_WARNING("Actor to delete could not be found in the game manager.");
            return;
         }

         if (proxy->GetActorType().InstanceOf("dtcore.Tasks","Task Actor"))
         RemoveTaskActor(*proxy);
      }
      else if (message.GetMessageType() == MessageType::INFO_MAP_LOADED)
      {
         //If we got this message it means a new map was sucessfully loaded.
         //So we need to clear our list of tasks and check the game manager
         //for a new list.
         HandleMapLoaded();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void TaskComponent::InsertTaskActor(GameActorProxy &taskProxy)
   {
      std::map<std::string,dtCore::RefPtr<dtGame::GameActorProxy> >::iterator itor =
            mTaskList.find(taskProxy.GetName());

      if (itor != mTaskList.end())
      {
         //LOG_ERROR("Cannot add new task to task component.  Duplicate name encountered:  " + taskProxy.GetName());
         return;
      }

      mTaskList.insert(std::make_pair(taskProxy.GetName(),&taskProxy));
      const dtCore::BooleanActorProperty *prop =
         static_cast<const dtCore::BooleanActorProperty *>(taskProxy.GetProperty("IsTopLevel"));
      if (prop != NULL && prop->GetValue())
         mTopLevelTaskList.insert(std::make_pair(taskProxy.GetName(),&taskProxy));
   }

   //////////////////////////////////////////////////////////////////////////
   void TaskComponent::RemoveTaskActor(GameActorProxy &toRemove)
   {
      std::map<std::string,dtCore::RefPtr<dtGame::GameActorProxy> >::iterator itor =
            mTaskList.find(toRemove.GetName());

      if (itor != mTaskList.end())
      {
         mTaskList.erase(itor);
      }

      itor = mTopLevelTaskList.find(toRemove.GetName());
      if (itor != mTopLevelTaskList.end())
      {
         mTopLevelTaskList.erase(itor);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void TaskComponent::HandleMapLoaded()
   {
      ClearTaskList();
      
      std::vector<GameActorProxy*> toFill;
      std::vector<GameActorProxy*>::iterator itor;
      GetGameManager()->GetAllGameActors(toFill);
      for (itor=toFill.begin(); itor!=toFill.end(); ++itor)
      {
         if ((*itor)->GetActorType().InstanceOf("dtcore.Tasks","Task Actor"))
            InsertTaskActor(*(*itor));
      }

   }

   //////////////////////////////////////////////////////////////////////////
   void TaskComponent::GenerateTaskUpdates()
   {
      std::map<std::string,dtCore::RefPtr<dtGame::GameActorProxy> >::iterator itor;
      for (itor=mTaskList.begin(); itor!=mTaskList.end(); ++itor)
      {
         dtCore::RefPtr<ActorUpdateMessage> updateMsg = static_cast<ActorUpdateMessage *>
            (GetGameManager()->GetMessageFactory().CreateMessage(MessageType::INFO_ACTOR_UPDATED).get());
         itor->second->PopulateActorUpdate(*updateMsg);
         GetGameManager()->SendMessage(*updateMsg);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void TaskComponent::CheckTaskHierarchy()
   {
      std::map<std::string,dtCore::RefPtr<dtGame::GameActorProxy> >::iterator itor;      

      itor = mTopLevelTaskList.begin();
      while (itor != mTopLevelTaskList.end())
      {         
         if (itor->second->GetProperty("IsTopLevel")->ToString() == "false")
            mTopLevelTaskList.erase(itor++);
         else
            ++itor;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void TaskComponent::GetTopLevelTasks(std::vector<dtCore::RefPtr<GameActorProxy> > &toFill)
   {
      std::map<std::string,dtCore::RefPtr<dtGame::GameActorProxy> >::iterator taskItor;

      toFill.clear();
      for (taskItor=mTopLevelTaskList.begin(); taskItor!=mTopLevelTaskList.end(); ++taskItor)
      {
         toFill.push_back(taskItor->second);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void TaskComponent::GetAllTasks(std::vector<dtCore::RefPtr<GameActorProxy> > &toFill)
   {
      std::map<std::string,dtCore::RefPtr<dtGame::GameActorProxy> >::iterator taskItor;

      toFill.clear();
      for (taskItor=mTaskList.begin(); taskItor!=mTaskList.end(); ++taskItor)
      {
         toFill.push_back(taskItor->second);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   GameActorProxy* TaskComponent::GetTaskByName(const std::string &name)
   {
      std::map<std::string,dtCore::RefPtr<dtGame::GameActorProxy> >::iterator taskItor =
            mTaskList.find(name);

      if (taskItor != mTaskList.end())
         return taskItor->second.get();
      else
         return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void TaskComponent::ClearTaskList()
   {
      mTaskList.clear();
      mTopLevelTaskList.clear();
   }
}
