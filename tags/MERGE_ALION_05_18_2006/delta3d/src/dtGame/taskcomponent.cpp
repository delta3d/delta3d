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
 * @author Matthew W. Campbell
 */
 
#include "dtGame/taskcomponent.h"
#include "dtGame/message.h"
#include "dtGame/messagetype.h"
#include "dtGame/basemessages.h"
#include "dtGame/actorupdatemessage.h"
#include <dtDAL/enginepropertytypes.h>

namespace dtGame
{

   //////////////////////////////////////////////////////////////////////////
   TaskComponent::TaskComponent()
   {
   }
   
   //////////////////////////////////////////////////////////////////////////
   TaskComponent::~TaskComponent()
   {
         
   }
   
   //////////////////////////////////////////////////////////////////////////
   void TaskComponent::ProcessMessage(const Message &message)
   {
      if (message.GetMessageType() == MessageType::INFO_ACTOR_CREATED ||
          message.GetMessageType() == MessageType::INFO_ACTOR_PUBLISHED ||
          message.GetMessageType() == MessageType::INFO_ACTOR_UPDATED)
      {         
         const dtCore::UniqueId &id = message.GetAboutActorId();
         dtGame::GameActorProxy *proxy = GetGameManager()->FindGameActorById(id);
         
         if (proxy == NULL)
         {
            LOG_WARNING("Newly created actor could not be found in the game manager.");
            return;
         }
         
         if (proxy->GetActorType().InstanceOf("dtcore.Tasks","Task Actor"))
            InsertTaskActor(*proxy);         
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
   void TaskComponent::InsertTaskActor(const GameActorProxy &taskProxy)
   {  
      mTaskList.insert(taskProxy.GetId());
      const dtDAL::BooleanActorProperty *prop =             
         static_cast<const dtDAL::BooleanActorProperty *>(taskProxy.GetProperty("IsTopLevel"));
      if (prop != NULL && prop->GetValue())
         mTopLevelTaskList.insert(taskProxy.GetId());    
   }
   
   //////////////////////////////////////////////////////////////////////////
   void TaskComponent::RemoveTaskActor(const GameActorProxy &toRemove)
   {  
      std::set<dtCore::UniqueId>::iterator itor = 
         mTaskList.find(toRemove.GetId());
      mTaskList.erase(itor);
      
      itor = mTopLevelTaskList.find(toRemove.GetId());
      mTopLevelTaskList.erase(itor);
   }
 
   //////////////////////////////////////////////////////////////////////////
   void TaskComponent::HandleMapLoaded()
   {
      mTopLevelTaskList.clear();
      mTaskList.clear();
      
      std::vector<dtCore::RefPtr<GameActorProxy> > toFill;
      std::vector<dtCore::RefPtr<GameActorProxy> >::iterator itor;
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
      dtGame::GameActorProxy *proxy = NULL;
      std::set<dtCore::UniqueId>::iterator itor;
      
      for (itor=mTaskList.begin(); itor!=mTaskList.end(); ++itor)
      {
         proxy = GetGameManager()->FindGameActorById(*itor);
         if (proxy != NULL)
         {
            dtCore::RefPtr<ActorUpdateMessage> updateMsg = static_cast<ActorUpdateMessage *>
               (GetGameManager()->GetMessageFactory().CreateMessage(MessageType::INFO_ACTOR_UPDATED).get());
            proxy->PopulateActorUpdate(*updateMsg);
            GetGameManager()->ProcessMessage(*updateMsg);
         }
      }
   }
   
   //////////////////////////////////////////////////////////////////////////
   void TaskComponent::GetTopLevelTasks(std::vector<dtCore::RefPtr<GameActorProxy> > &toFill)
   {
      std::set<dtCore::UniqueId>::iterator taskItor;
      
      toFill.clear();
      for (taskItor=mTopLevelTaskList.begin(); taskItor!=mTopLevelTaskList.end(); ++taskItor)
      {
         GameActorProxy *proxy = GetGameManager()->FindGameActorById(*taskItor);
         if (proxy != NULL)
            toFill.push_back(proxy);
      }
   }
   
   //////////////////////////////////////////////////////////////////////////
   void TaskComponent::GetAllTasks(std::vector<dtCore::RefPtr<GameActorProxy> > &toFill)
   {
      std::set<dtCore::UniqueId>::iterator taskItor;
      
      toFill.clear();
      for (taskItor=mTaskList.begin(); taskItor!=mTaskList.end(); ++taskItor)
      {
         GameActorProxy *proxy = GetGameManager()->FindGameActorById(*taskItor);
         if (proxy != NULL)
            toFill.push_back(proxy);
      }
   }
   
   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<GameActorProxy> TaskComponent::GetTaskByName(const std::string &name)
   {
      std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > toFill;
      GetGameManager()->FindActorsByName(name,toFill);
      if (!toFill.empty() && toFill[0]->IsGameActorProxy())
         return dynamic_cast<GameActorProxy *>(toFill[0].get());
      else
         return NULL;
   }
     
}
