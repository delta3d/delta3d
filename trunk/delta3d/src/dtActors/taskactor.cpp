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
 * Matthew W. Campbell, Curtiss Murphy
 */
#include <dtActors/taskactor.h>

#include <dtGame/exceptionenum.h>
#include <dtGame/gamemanager.h>
#include <dtGame/actorupdatemessage.h>

#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/groupactorproperty.h>
#include <dtDAL/namedparameter.h>
#include <dtDAL/project.h>
#include <dtDAL/map.h>

#include <sstream>

namespace dtActors
{
   //////////////////////////////////////////////////////////////////////////////
   TaskActor::TaskActor(dtGame::GameActorProxy &proxy) : dtGame::GameActor(proxy)
   {
      SetName("Task");
      //SetDisplayName("Display Name");
      Reset();
   }

   TaskActor::~TaskActor()
   {
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActor::SetComplete(bool flag)
   {
      if (flag == mComplete) // Do nothing including no Actor Update or change to time stamp.
      {      
      }
      // Complete and Failed are mutually exclusive - makes a trinary state (complete, incomplete, and failed)
      else if (flag && IsFailed())
      {
         LOG_WARNING("Error setting complete since we are already marked Failed.");
      }
      else
      {
         // If setting, then we need to get a new time stamp
         if (flag)
         {
            if (GetGameActorProxy().GetGameManager() == NULL)
            {
               LOG_ERROR("Error setting complete flag on Task Actor.  Game Manager was invalid on this actor.");
            }
            else
            {
               double currSimTime = GetGameActorProxy().GetGameManager()->GetSimulationTime();
               SetCompletedTimeStamp(currSimTime);
            }
         }
         // don't overwrite complete time if we are already failed. Allows setting this back to false.
         else if (!IsFailed()) 
         {
            SetCompletedTimeStamp(-1.0);
         }

         mComplete = flag;

         // Significant change, so notify the world  
         if (GetGameActorProxy().GetGameManager() == NULL)
         {
            TaskActorProxy &proxy = static_cast<TaskActorProxy&>(GetGameActorProxy());
            proxy.NotifyActorUpdate();
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActor::SetFailed(bool flag)
   {
      if (flag == mFailed) // Do nothing including no Actor Update or change to time stamp. 
      {   
      }
      // Complete and Failed are mutually exclusive - makes a trinary state (complete, incomplete, and failed)
      else if (flag && IsComplete())
      {
         LOG_WARNING("Error setting failed since we are already marked Complete.");
      }
      else
      {
         // If setting, then we need to get a new time stamp
         if (flag)
         {
            if (GetGameActorProxy().GetGameManager() == NULL)
            {
               LOG_ERROR("Error setting failed flag on Task Actor.  Game Manager was invalid on this actor.");
            }
            else
            {
               double currSimTime = GetGameActorProxy().GetGameManager()->GetSimulationTime();
               SetCompletedTimeStamp(currSimTime);
            }
         }
         // don't overwrite complete time if we are already failed. Allows setting this back to false.
         else if (!IsComplete()) // don't overwrite complete time if we are already complete
         {
            SetCompletedTimeStamp(-1.0);
         }

         mFailed = flag;

         // Significant change, so notify the world  
         if (GetGameActorProxy().GetGameManager() == NULL)
         {
            TaskActorProxy &proxy = static_cast<TaskActorProxy&>(GetGameActorProxy());
            proxy.NotifyActorUpdate();
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActor::Reset()
   {
      mPassingScore = 1.0f;
      mScore = 0.0f;
      mWeight = 1.0f;
      mCompletedTimeStamp = -1.0;
      mComplete = false;
      mFailed = false;
      mNotifyLMSOnUpdate = false;

      TaskActorProxy &proxy = static_cast<TaskActorProxy&>(GetGameActorProxy());
      proxy.NotifyActorUpdate();
   }

   ////////////////////////////END TASK ACTOR////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   ///////////////////////////BEGIN TASK PROXY///////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////
   TaskActorProxy::TaskActorProxy()
   {
      SetClassName("dtActors::TaskActor");
      mParentTaskProxy = NULL;
   }

   //////////////////////////////////////////////////////////////////////////////
   TaskActorProxy::~TaskActorProxy()
   {
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::BuildPropertyMap()
   {
      const std::string GROUPNAME = "BaseTask";

      dtGame::GameActorProxy::BuildPropertyMap();
      TaskActor &task = static_cast<TaskActor &>(GetGameActor());

      //Description...
      AddProperty(new dtDAL::StringActorProperty("Description","Description",
         dtDAL::StringActorProperty::SetFuncType(&task,&TaskActor::SetDescription),
         dtDAL::StringActorProperty::GetFuncType(&task,&TaskActor::GetDescription),
         "Sets/gets the description of this task.",GROUPNAME));

      //DisplayName...
      AddProperty(new dtDAL::StringActorProperty("DisplayName","Display Name",
         dtDAL::StringActorProperty::SetFuncType(&task,&TaskActor::SetDisplayName),
         dtDAL::StringActorProperty::GetFuncType(&task,&TaskActor::GetDisplayName),
         "Sets/gets the display name (ie. user viewable) of this task.",GROUPNAME));

      //Passing Score...
      AddProperty(new dtDAL::FloatActorProperty("PassingScore","Passing Score",
         dtDAL::MakeFunctor(task,&TaskActor::SetPassingScore),
         dtDAL::MakeFunctorRet(task,&TaskActor::GetPassingScore),
         "Sets/gets the passing score for this task.",GROUPNAME));

      //Score...
      AddProperty(new dtDAL::FloatActorProperty("Score","Score",
         dtDAL::MakeFunctor(task,&TaskActor::SetScore),
         dtDAL::MakeFunctorRet(task,&TaskActor::GetScore),
         "Sets/gets the current score of this task.",GROUPNAME));

      //Weight...
      AddProperty(new dtDAL::FloatActorProperty("Weight","Weight",
         dtDAL::MakeFunctor(task,&TaskActor::SetWeight),
         dtDAL::MakeFunctorRet(task,&TaskActor::GetWeight),
         "Sets/gets the current weight assigned to this task.",GROUPNAME));

      //Complete...
      AddProperty(new dtDAL::BooleanActorProperty("Complete","Complete",
         dtDAL::MakeFunctor(task,&TaskActor::SetComplete),
         dtDAL::MakeFunctorRet(task,&TaskActor::IsComplete),
         "Gets the complete status of this task.",GROUPNAME));

      //NotifyLMSOnUpdate
      AddProperty(new dtDAL::BooleanActorProperty("NotifyLMSOnUpdate","Notify LMS On Update",
         dtDAL::MakeFunctor(task,&TaskActor::SetNotifyLMSOnUpdate),
         dtDAL::MakeFunctorRet(task,&TaskActor::GetNotifyLMSOnUpdate),
         "Sets/gets the flag that determines if this task should notify an LMS when it is updated.",GROUPNAME));

      //Completed time...
      AddProperty(new dtDAL::DoubleActorProperty("CompleteTime","Complete Time",
                  dtDAL::MakeFunctor(task,&TaskActor::SetCompletedTimeStamp),
                  dtDAL::MakeFunctorRet(task,&TaskActor::GetCompletedTimeStamp),
                  "Gets the simulation time in which this task was completed.",GROUPNAME));

      //IsTopLevel...
      AddProperty(new dtDAL::BooleanActorProperty("IsTopLevel","Top Level Task",
         dtDAL::BooleanActorProperty::SetFuncType(),
         dtDAL::BooleanActorProperty::GetFuncType(this, &TaskActorProxy::IsTopLevelTask),
         "Sets/gets whether or not this task contains a parent task.",GROUPNAME));
      GetProperty("IsTopLevel")->SetReadOnly(true);

      //SubTasks property.
      AddProperty(new dtDAL::GroupActorProperty("SubTasks", "Sub Task Actor List", 
         dtDAL::MakeFunctor(*this, &TaskActorProxy::SetSubTaskGroup),
         dtDAL::MakeFunctorRet(*this, &TaskActorProxy::GetSubTaskGroup),
         "The list of subtasks.", GROUPNAME, "TaskChildren"));

      // REMOVE USELESS PROPERTIES - These properties really should not show in
      // STAGE and ought to be completely removed from the object completely.
      // However, the overhead is part of sub-classing GameActor.
      RemoveProperty("Rotation");
      RemoveProperty("Translation");
      RemoveProperty("Normal Rescaling");
      RemoveProperty("Render Proxy Node");
      RemoveProperty("Show Collision Geometry"); //"ODE Show Collision Geometry"
      RemoveProperty(dtDAL::TransformableActorProxy::PROPERTY_COLLISION_TYPE);
      RemoveProperty(dtDAL::TransformableActorProxy::PROPERTY_COLLISION_RADIUS);
      RemoveProperty(dtDAL::TransformableActorProxy::PROPERTY_COLLISION_LENGTH);
      RemoveProperty(dtDAL::TransformableActorProxy::PROPERTY_COLLISION_BOX);
      RemoveProperty(dtDAL::TransformableActorProxy::PROPERTY_ENABLE_COLLISION);
      RemoveProperty("Enable Dynamics"); // "ODE Enable Dynamics"
      RemoveProperty("Mass"); // "ODE Mass"
      RemoveProperty("Center of Gravity"); // "ODE Center of Gravity"
      RemoveProperty("ShaderGroup");
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::BuildInvokables()
   {
      dtGame::GameActorProxy::BuildInvokables();
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::OnEnteredWorld()
   {
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::CreateActor()
   {
      SetActor(*new TaskActor(*this));
   }

   //////////////////////////////////////////////////////////////////////////////
   bool TaskActorProxy::RequestScoreChange(const TaskActorProxy &childTask, const TaskActorProxy &origTask)
   {
      TaskActor *taskActor; 
      GetActor(taskActor);

      // If we are failed, then the request is denied.
      if (taskActor->IsFailed())
         return false;
      else if (GetParentTask() != NULL) // ask our parent if we have one
         return GetParentTask()->RequestScoreChange(*this,origTask);
      else // a root task returns true by default
         return true;
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::NotifyScoreChanged(const TaskActorProxy &childTask)
   {
      //Default implementation just needs to pass the notification up the chain.
      if (GetParentTask() != NULL)
         GetParentTask()->NotifyScoreChanged(*this);
   }

   //////////////////////////////////////////////////////////////////////////////
   bool TaskActorProxy::IsCurrentlyMutable()
   {
      // Default implementation goes on the premise that a task is considered mutable if 
      // if it not already complete or failed and if our parent says we're allowed to change.
      TaskActor *taskActor;
      GetActor(taskActor);
      bool bResult = !taskActor->IsComplete() && !taskActor->IsFailed() && IsChildTaskAllowedToChange(*this);

      return bResult;
   }

   //////////////////////////////////////////////////////////////////////////////
   bool TaskActorProxy::IsChildTaskAllowedToChange(const TaskActorProxy &childTask) const
   {
      const TaskActor *myActor;
      GetActor(myActor);

      // Note, our child is allowed to change if we are complete, since a passing score could 
      // be improved, but not if we are failed, since we want to hold failure steady.
      if (myActor->IsFailed())
         return false;
      else if (GetParentTask() != NULL) // ask our parent if we have one. 
         return GetParentTask()->IsChildTaskAllowedToChange(childTask);
      else // default is to return true
         return true;
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::AddSubTask(TaskActorProxy &subTask)
   {
      if (FindSubTask(subTask.GetGameActor().GetUniqueId()) != NULL)
         throw dtUtil::Exception(dtGame::ExceptionEnum::INVALID_PARAMETER,
         "Cannot add a duplicate sub task." , __FILE__, __LINE__);

      if (subTask.GetParentTask() != NULL)
         subTask.GetParentTask()->RemoveSubTask(subTask);

      subTask.SetParentTaskProxy(this);
      mSubTaskProxies.push_back(&subTask);
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::RemoveSubTask(const TaskActorProxy &subTask)
   {
      std::vector<dtCore::RefPtr<TaskActorProxy> >::iterator itor;

      for (itor=mSubTaskProxies.begin(); itor!=mSubTaskProxies.end(); ++itor)
      {
         if ((*itor)->GetId() == subTask.GetId())
            break;
      }

      if (itor == mSubTaskProxies.end())
      {
         LOG_WARNING("Task: " + subTask.GetGameActor().GetName() + " is not a sub task of task: " +
            GetGameActor().GetName() + "  Cannot remove.");
      }
      else
      {
         (*itor)->SetParentTaskProxy(NULL);
         mSubTaskProxies.erase(itor);
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::RemoveSubTask(const std::string &name)
   {
      std::vector<dtCore::RefPtr<TaskActorProxy> >::iterator itor;
      for (itor=mSubTaskProxies.begin(); itor!=mSubTaskProxies.end(); ++itor)
      {
         if ((*itor)->GetName() == name)
            break;
      }

      if (itor == mSubTaskProxies.end())
      {
         LOG_WARNING("Task: " + name + " is not a sub task of task: " +
            GetGameActor().GetName() + "  Cannot remove.");
      }
      else
      {
         (*itor)->SetParentTaskProxy(NULL);
         mSubTaskProxies.erase(itor);
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   TaskActorProxy *TaskActorProxy::FindSubTask(const std::string &name)
   {
      std::vector<dtCore::RefPtr<TaskActorProxy> >::iterator itor;

      for (itor=mSubTaskProxies.begin(); itor!=mSubTaskProxies.end(); ++itor)
      {
         if((*itor)->GetName() == name)
            return (*itor).get();
      }
      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////////
   TaskActorProxy *TaskActorProxy::FindSubTask(const dtCore::UniqueId &id)
   {
      std::vector<dtCore::RefPtr<TaskActorProxy> >::iterator itor;
      for (itor=mSubTaskProxies.begin(); itor!=mSubTaskProxies.end(); ++itor)
      {
         if((*itor)->GetId() == id)
            return (*itor).get();
      }
      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::GetAllSubTasks(std::vector<TaskActorProxy*>& toFill)
   {
      toFill.reserve(mSubTaskProxies.size());
      toFill.clear();
      for (unsigned i = 0; i < mSubTaskProxies.size(); ++i)
      {
         toFill.push_back(mSubTaskProxies[i].get());
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::GetAllSubTasks(std::vector<const TaskActorProxy*>& toFill) const
   {
      toFill.reserve(mSubTaskProxies.size());
      toFill.clear();
      for (unsigned i = 0; i < mSubTaskProxies.size(); ++i)
      {
         toFill.push_back(mSubTaskProxies[i].get());
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::SetSubTaskGroup(const dtDAL::NamedGroupParameter& subTasks)
   {
      //first remove all subtasks, even if the parameter may have some
      //of the same tasks in it.
      for (unsigned i = 0; i < mSubTaskProxies.size(); ++i)
      {
         mSubTaskProxies[i]->SetParentTaskProxy(NULL);
      }
      
      mSubTaskProxies.clear();
      
      std::vector<const dtDAL::NamedParameter*> toFill;
      subTasks.GetParameters(toFill);

      for (unsigned i = 0; i < toFill.size(); ++i)
      {
         if (toFill[i]->GetDataType() == dtDAL::DataType::ACTOR)
         {
            const dtCore::UniqueId& id = static_cast<const dtDAL::NamedActorParameter*>(toFill[i])->GetValue();
            if (IsInGM())
            {
               TaskActorProxy* taskActor = NULL;
               GetGameManager()->FindGameActorById(id, taskActor);
               
               if (taskActor != NULL)
                  AddSubTask(*taskActor);
            }
            else 
            {
               TaskActorProxy* taskActor = NULL;
               dtDAL::Map* m = dtDAL::Project::GetInstance().GetMapForActorProxy(*this);
               if (m != NULL)
               {
                   m->GetProxyById(id, taskActor);
               
                  if (taskActor != NULL)
                     AddSubTask(*taskActor);
               }
            }
         }
      }      
   }
   
   //////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtDAL::NamedGroupParameter> TaskActorProxy::GetSubTaskGroup() const
   {
      dtCore::RefPtr<dtDAL::NamedGroupParameter> result = new dtDAL::NamedGroupParameter("SubTasks");
      std::ostringstream ss;
      
      ss << mSubTaskProxies.size();
      unsigned stringLength = ss.str().size();
      
      for (unsigned i = 0; i < mSubTaskProxies.size(); ++i)
      {
         ss.str("");
         ss << i;
         std::string s = ss.str();
         s.insert(s.begin(), stringLength - s.size(), '0');
         result->AddParameter(*new dtDAL::NamedActorParameter(s, mSubTaskProxies[i]->GetId()));
      }
      return result;
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::NotifyActorUpdate()
   {
      if (GetGameManager() == NULL || GetGameActor().IsRemote())
         return;

      dtCore::RefPtr<dtGame::Message> updateMsg =
            GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED);
      dtGame::ActorUpdateMessage *message = static_cast<dtGame::ActorUpdateMessage *>(updateMsg.get());
      PopulateActorUpdate(*message);
      GetGameManager()->SendMessage(*updateMsg);
   }
}
