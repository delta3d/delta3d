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

#include <dtGame/actorupdatemessage.h>
#include <dtGame/basemessages.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/gamemanager.h>
#include <dtGame/invokable.h>
#include <dtGame/messagetype.h>
#include <dtGame/messagefactory.h>

#include <dtDAL/actoridactorproperty.h>
#include <dtDAL/arrayactorproperty.h>
#include <dtDAL/booleanactorproperty.h>
#include <dtDAL/doubleactorproperty.h>
#include <dtDAL/floatactorproperty.h>
#include <dtDAL/functor.h>
#include <dtDAL/gameeventactorproperty.h>
#include <dtDAL/groupactorproperty.h>
#include <dtDAL/map.h>
#include <dtDAL/namedparameter.h>
#include <dtDAL/project.h>
#include <dtDAL/stringactorproperty.h>

#include <sstream>

namespace dtActors
{
   //////////////////////////////////////////////////////////////////////////////
   TaskActor::TaskActor(dtGame::GameActorProxy& proxy) : dtGame::GameActor(proxy)
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
         TaskActorProxy& proxy = static_cast<TaskActorProxy&>(GetGameActorProxy());

         // If setting, then we need to get a new time stamp
         if (flag)
         {
            if (proxy.GetGameManager() == NULL)
            {
               LOG_ERROR("Error setting complete flag on Task Actor.  Game Manager was invalid on this actor.");
            }
            else
            {
               double currSimTime = proxy.GetGameManager()->GetSimulationTime();
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
         if (proxy.GetGameManager() == NULL)
         {
            proxy.NotifyFullActorUpdate();
         }

         // Send the Notify Completed Event if marking this task as completed.
         if (mComplete && mNotifyEventCompleted.valid())
         {
            proxy.SendGameEvent(*mNotifyEventCompleted);
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
         TaskActorProxy& proxy = static_cast<TaskActorProxy&>(GetGameActorProxy());

         // If setting, then we need to get a new time stamp
         if (flag)
         {
            if (proxy.GetGameManager() == NULL)
            {
               LOG_ERROR("Error setting failed flag on Task Actor.  Game Manager was invalid on this actor.");
            }
            else
            {
               double currSimTime = proxy.GetGameManager()->GetSimulationTime();
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
         if (proxy.GetGameManager() == NULL)
         {
            proxy.NotifyFullActorUpdate();
         }

         // Send the Notify Failed Event if marking this task as failed.
         if ( mFailed && mNotifyEventFailed.valid() )
         {
            proxy.SendGameEvent( *mNotifyEventFailed );
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
      proxy.NotifyFullActorUpdate();
   }

   ////////////////////////////END TASK ACTOR////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////

   ///////////////////////////BEGIN TASK PROXY///////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString TaskActorProxy::CLASS_NAME("dtActors::TaskActor");
   const dtUtil::RefString TaskActorProxy::PROPERTY_EVENT_NOTIFY_COMPLETED("NotifyCompletedEvent");
   const dtUtil::RefString TaskActorProxy::PROPERTY_EVENT_NOTIFY_FAILED("NotifyFailedEvent");

   //////////////////////////////////////////////////////////////////////////////
   TaskActorProxy::TaskActorProxy()
      : mSubTaskIndex(0)
   {
      SetClassName(TaskActorProxy::CLASS_NAME);
      SetHideDTCorePhysicsProps(true);
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

      //DisplayName...
      AddProperty(new dtDAL::StringActorProperty("DisplayName","Display Name",
         dtDAL::StringActorProperty::SetFuncType(&task,&TaskActor::SetDisplayName),
         dtDAL::StringActorProperty::GetFuncType(&task,&TaskActor::GetDisplayName),
         "Sets/gets the display name (ie. user viewable) of this task.",GROUPNAME));

      //Passing Score...
      AddProperty(new dtDAL::FloatActorProperty("PassingScore","Passing Score",
         dtDAL::FloatActorProperty::SetFuncType(&task,&TaskActor::SetPassingScore),
         dtDAL::FloatActorProperty::GetFuncType(&task,&TaskActor::GetPassingScore),
         "Sets/gets the passing score for this task.",GROUPNAME));

      //Score...
      AddProperty(new dtDAL::FloatActorProperty("Score","Score",
         dtDAL::FloatActorProperty::SetFuncType(&task,&TaskActor::SetScore),
         dtDAL::FloatActorProperty::GetFuncType(&task,&TaskActor::GetScore),
         "Sets/gets the current score of this task.",GROUPNAME));

      //Weight...
      AddProperty(new dtDAL::FloatActorProperty("Weight","Weight",
         dtDAL::FloatActorProperty::SetFuncType(&task,&TaskActor::SetWeight),
         dtDAL::FloatActorProperty::GetFuncType(&task,&TaskActor::GetWeight),
         "Sets/gets the current weight assigned to this task.",GROUPNAME));

      //Complete...
      AddProperty(new dtDAL::BooleanActorProperty("Complete","Complete",
         dtDAL::BooleanActorProperty::SetFuncType(&task,&TaskActor::SetComplete),
         dtDAL::BooleanActorProperty::GetFuncType(&task,&TaskActor::IsComplete),
         "Gets the complete status of this task.",GROUPNAME));

      //NotifyLMSOnUpdate
      AddProperty(new dtDAL::BooleanActorProperty("NotifyLMSOnUpdate","Notify LMS On Update",
         dtDAL::BooleanActorProperty::SetFuncType(&task,&TaskActor::SetNotifyLMSOnUpdate),
         dtDAL::BooleanActorProperty::GetFuncType(&task,&TaskActor::GetNotifyLMSOnUpdate),
         "Sets/gets the flag that determines if this task should notify an LMS when it is updated.",GROUPNAME));

      //Completed time...
      AddProperty(new dtDAL::DoubleActorProperty("CompleteTime","Complete Time",
                  dtDAL::DoubleActorProperty::SetFuncType(&task,&TaskActor::SetCompletedTimeStamp),
                  dtDAL::DoubleActorProperty::GetFuncType(&task,&TaskActor::GetCompletedTimeStamp),
                  "Gets the simulation time in which this task was completed.",GROUPNAME));

      //IsTopLevel...
      AddProperty(new dtDAL::BooleanActorProperty("IsTopLevel","Top Level Task",
         dtDAL::BooleanActorProperty::SetFuncType(),
         dtDAL::BooleanActorProperty::GetFuncType(this, &TaskActorProxy::IsTopLevelTask),
         "Sets/gets whether or not this task contains a parent task.",GROUPNAME));
      GetProperty("IsTopLevel")->SetReadOnly(true);

      // A Task in the Task List
      dtDAL::ActorIDActorProperty* actorProp = new dtDAL::ActorIDActorProperty(
         *this, "Task", "Task",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &TaskActorProxy::SetSubTask),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &TaskActorProxy::GetSubTask),
         "dtActors::TaskActor", "A sub task", GROUPNAME);

      // The Task List.
      AddProperty(new dtDAL::ArrayActorProperty<dtCore::UniqueId>(
         "SubTaskList", "Sub Task List", "List of sub tasks",
         dtDAL::ArrayActorProperty<dtCore::UniqueId>::SetIndexFuncType(this, &TaskActorProxy::TaskArraySetIndex),
         dtDAL::ArrayActorProperty<dtCore::UniqueId>::GetDefaultFuncType(this, &TaskActorProxy::TaskArrayGetDefault),
         dtDAL::ArrayActorProperty<dtCore::UniqueId>::GetArrayFuncType(this, &TaskActorProxy::TaskArrayGetValue),
         dtDAL::ArrayActorProperty<dtCore::UniqueId>::SetArrayFuncType(this, &TaskActorProxy::TaskArraySetValue),
         actorProp, GROUPNAME));

      // Notify Completed Event
      AddProperty(new dtDAL::GameEventActorProperty(*this,
         TaskActorProxy::PROPERTY_EVENT_NOTIFY_COMPLETED,
         "Notify Completed Event",
         dtDAL::GameEventActorProperty::SetFuncType(&task, &TaskActor::SetNotifyCompletedEvent),
         dtUtil::MakeFunctor<dtDAL::GameEvent* (TaskActor::*)(), TaskActor> // djmc new attempt
            (&TaskActor::GetNotifyCompletedEvent, task),
         "Sets and gets the game event that will be sent when this task completes.",GROUPNAME));

      // Notify Failed Event
      AddProperty(new dtDAL::GameEventActorProperty(*this,
         TaskActorProxy::PROPERTY_EVENT_NOTIFY_FAILED,
         "Notify Failed Event",
         dtDAL::GameEventActorProperty::SetFuncType(&task, &TaskActor::SetNotifyFailedEvent),
         dtUtil::MakeFunctor<dtDAL::GameEvent* (TaskActor::*)(), TaskActor>
            (&TaskActor::GetNotifyFailedEvent, task),
         "Sets and gets the game event that will be sent when this task fails.",GROUPNAME));

      // REMOVE USELESS PROPERTIES - These properties really should not show in
      // STAGE and ought to be completely removed from the object completely.
      // However, the overhead is part of sub-classing GameActor.
      RemoveProperty(dtDAL::TransformableActorProxy::PROPERTY_ROTATION);
      RemoveProperty(dtDAL::TransformableActorProxy::PROPERTY_TRANSLATION);
      RemoveProperty(dtDAL::TransformableActorProxy::PROPERTY_NORMAL_RESCALING);
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

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtDAL::ActorProperty> TaskActorProxy::GetDeprecatedProperty(const std::string& name)
   {
      dtCore::RefPtr<dtDAL::ActorProperty> prop = dtGame::GameActorProxy::GetDeprecatedProperty(name);

      if (!prop.valid())
      {
         if (name == "SubTasks")
         {
            // The SubTasks property was changed from a GroupActorProperty to
            // an ArrayActorProperty.
            prop = new dtDAL::GroupActorProperty("SubTasks", "Sub Task Actor List",
               dtDAL::GroupActorProperty::SetFuncType(this, &TaskActorProxy::SetSubTaskGroup),
               dtDAL::GroupActorProperty::GetFuncType(this, &TaskActorProxy::GetSubTaskGroup),
               "The list of subtasks.", "BaseTask", "TaskChildren");
         }
         else if (name == "TaskList")
         {
            // We have renamed this property, so redirect it.
            return GetProperty("SubTaskList");
         }
      }

      return prop;
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::OnEnteredWorld()
   {
      dtGame::Invokable* mapLoadedInvoke = new dtGame::Invokable("MapLoaded",
         dtUtil::MakeFunctor(&TaskActorProxy::OnMapLoaded, *this));

      AddInvokable(*mapLoadedInvoke);

      RegisterForMessages(dtGame::MessageType::INFO_MAP_LOADED, "MapLoaded");
   }

   ////////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::OnMapLoaded(const dtGame::Message& msg)
   {
      for (int taskIndex = 0; taskIndex < (int)mSubTasks.size(); taskIndex++)
      {
         TaskActorProxy* subTask = GetProxyById(mSubTasks[taskIndex]);

         if (subTask)
         {
            if (subTask->GetParentTask())
            {
               subTask->GetParentTask()->RemoveSubTask(*subTask);
            }

            subTask->SetParentTaskProxy(this);
         }
      }
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
      {
         return false;
      }
      else if (GetParentTask() != NULL) // ask our parent if we have one
      {
         return GetParentTask()->RequestScoreChange(*this, origTask);
      }
      else // a root task returns true by default
      {
         return true;
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::NotifyScoreChanged(const TaskActorProxy &childTask)
   {
      //Default implementation just needs to pass the notification up the chain.
      if (GetParentTask() != NULL)
      {
         GetParentTask()->NotifyScoreChanged(*this);
      }
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
      {
         return false;
      }
      else if (GetParentTask() != NULL) // ask our parent if we have one.
      {
         return GetParentTask()->IsChildTaskAllowedToChange(childTask);
      }
      else // default is to return true
      {
         return true;
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::AddSubTask(TaskActorProxy &subTask)
   {
      if (FindSubTask(subTask.GetGameActor().GetUniqueId()) != NULL)
      {
         throw dtGame::InvalidParameterException(
            "Cannot add a duplicate sub task." , __FILE__, __LINE__);
      }

      if (subTask.GetParentTask() != NULL)
      {
         subTask.GetParentTask()->RemoveSubTask(subTask);
      }

      subTask.SetParentTaskProxy(this);
      mSubTasks.push_back(subTask.GetId());
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::AddSubTask(dtCore::UniqueId id)
   {
      if (FindSubTask(id) != NULL)
      {
         throw dtGame::InvalidParameterException(
           "Cannot add a duplicate sub task." , __FILE__, __LINE__);
      }

      mSubTasks.push_back(id);

      TaskActorProxy* proxy = GetProxyById(id);
      if (proxy)
      {
         if (proxy->GetParentTask() != NULL)
         {
            proxy->GetParentTask()->RemoveSubTask(*proxy);
         }

         proxy->SetParentTaskProxy(this);
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::RemoveSubTask(const TaskActorProxy &subTask)
   {
      std::vector<dtCore::UniqueId>::iterator itor;

      for (itor=mSubTasks.begin(); itor!=mSubTasks.end(); ++itor)
      {
         if ((*itor) == subTask.GetId())
         {
            break;
         }
      }

      if (itor == mSubTasks.end())
      {
         LOG_WARNING("Task: " + subTask.GetGameActor().GetName() + " is not a sub task of task: " +
            GetGameActor().GetName() + "  Cannot remove.");
      }
      else
      {
         TaskActorProxy* proxy = GetProxyById((*itor));
         if (proxy)
         {
            proxy->SetParentTaskProxy(NULL);
         }
         mSubTasks.erase(itor);
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::RemoveSubTask(const std::string& name)
   {
      TaskActorProxy* proxy = NULL;
      std::vector<dtCore::UniqueId>::iterator itor;
      for (itor = mSubTasks.begin(); itor != mSubTasks.end(); ++itor)
      {
         proxy = GetProxyById((*itor));
         if (proxy->GetName() == name)
         {
            break;
         }
      }

      if (itor == mSubTasks.end())
      {
         LOG_WARNING("Task: " + name + " is not a sub task of task: " +
            GetGameActor().GetName() + "  Cannot remove.");
      }
      else if (proxy)
      {
         proxy->SetParentTaskProxy(NULL);
         mSubTasks.erase(itor);
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   TaskActorProxy* TaskActorProxy::FindSubTask(const std::string& name)
   {
      std::vector<dtCore::UniqueId>::iterator itor;

      for (itor = mSubTasks.begin(); itor != mSubTasks.end(); ++itor)
      {
         TaskActorProxy* proxy = GetProxyById((*itor));
         if (proxy->GetName() == name)
         {
            return proxy;
         }
      }
      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////////
   TaskActorProxy* TaskActorProxy::FindSubTask(const dtCore::UniqueId& id)
   {
      std::vector<dtCore::UniqueId>::iterator itor;
      for (itor = mSubTasks.begin(); itor != mSubTasks.end(); ++itor)
      {
         if ((*itor) == id)
         {
            return GetProxyById((*itor));
         }
      }
      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   TaskActorProxy* TaskActorProxy::GetProxyById(dtCore::UniqueId id) const
   {
      TaskActorProxy* proxy = NULL;
      if (IsInGM())
      {
         GetGameManager()->FindGameActorById(id, proxy);
      }
      return proxy;
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::GetAllSubTasks(std::vector<TaskActorProxy*>& toFill)
   {
      toFill.reserve(mSubTasks.size());
      toFill.clear();
      for (unsigned i = 0; i < mSubTasks.size(); ++i)
      {
         toFill.push_back(GetProxyById(mSubTasks[i]));
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::GetAllSubTasks(std::vector<const TaskActorProxy*>& toFill) const
   {
      toFill.reserve(mSubTasks.size());
      toFill.clear();
      for (unsigned i = 0; i < mSubTasks.size(); ++i)
      {
         toFill.push_back(GetProxyById(mSubTasks[i]));
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::SetSubTaskGroup(const dtDAL::NamedGroupParameter& subTasks)
   {
      //first remove all subtasks, even if the parameter may have some
      //of the same tasks in it.
      for (unsigned i = 0; i < mSubTasks.size(); ++i)
      {
         TaskActorProxy* proxy = GetProxyById(mSubTasks[i]);
         if (proxy)
         {
            proxy->SetParentTaskProxy(NULL);
         }
      }

      mSubTasks.clear();

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

   ////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtDAL::NamedGroupParameter> TaskActorProxy::GetSubTaskGroup() const
   {
      dtCore::RefPtr<dtDAL::NamedGroupParameter> result = new dtDAL::NamedGroupParameter("SubTasks");
      std::ostringstream ss;

      ss << mSubTasks.size();
      unsigned stringLength = ss.str().size();

      for (unsigned i = 0; i < mSubTasks.size(); ++i)
      {
         ss.str("");
         ss << i;
         std::string s = ss.str();
         s.insert(s.begin(), stringLength - s.size(), '0');
         result->AddParameter(*new dtDAL::NamedActorParameter(s, mSubTasks[i]));
      }
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::SendGameEvent(dtDAL::GameEvent& gameEvent)
   {
      dtGame::GameManager* gm = GetGameManager();
      if ( gm != NULL )
      {
         dtCore::RefPtr<dtGame::GameEventMessage> eventMessage;
         gm->GetMessageFactory().CreateMessage( dtGame::MessageType::INFO_GAME_EVENT, eventMessage );

         eventMessage->SetAboutActorId( GetId() );
         eventMessage->SetGameEvent( gameEvent );
         gm->SendMessage( *eventMessage );
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::SetSubTask(dtCore::UniqueId value)
   {
      if (mSubTaskIndex < (int)mSubTasks.size())
      {
         mSubTasks[mSubTaskIndex] = value;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId TaskActorProxy::GetSubTask()
   {
      if (mSubTaskIndex < (int)mSubTasks.size())
      {
         return mSubTasks[mSubTaskIndex];
      }

      return dtCore::UniqueId("");
   }

   ////////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::TaskArraySetIndex(int index)
   {
      mSubTaskIndex = index;

      // Get the actor at the current index and put it into the non-index slot.
      std::string name = "Task";
      name += dtUtil::ToString(mSubTaskIndex);
      dtDAL::BaseActorObject* proxy = GetLinkedActor(name);
      if (proxy)
      {
         SetLinkedActor("Task", proxy);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId TaskActorProxy::TaskArrayGetDefault()
   {
      return dtCore::UniqueId("");
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<dtCore::UniqueId> TaskActorProxy::TaskArrayGetValue()
   {
      return mSubTasks;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void TaskActorProxy::TaskArraySetValue(const std::vector<dtCore::UniqueId>& value)
   {
      //first remove all subtasks, even if the parameter may have some
      //of the same tasks in it.
      for (unsigned i = 0; i < mSubTasks.size(); ++i)
      {
         TaskActorProxy* proxy = GetProxyById(mSubTasks[i]);
         if (proxy)
         {
            proxy->SetParentTaskProxy(NULL);
         }
      }
      mSubTasks.clear();

      // Now add all the tasks.
      for (int index = 0; index < (int)value.size(); index++)
      {
         dtCore::UniqueId id = value[index];
         AddSubTask(id);
      }
   }
}
