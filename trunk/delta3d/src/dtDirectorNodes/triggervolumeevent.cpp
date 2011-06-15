/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author: Jeff P. Houde
 */
#include <prefix/dtdirectornodesprefix.h>
#include <dtDirectorNodes/triggervolumeevent.h>

#include <dtDAL/actoridactorproperty.h>
#include <dtDAL/actorproxy.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   TriggerVolumeEvent::TriggerVolumeEvent()
       : EventNode()
   {
      mTriggerVolume = "";

      AddAuthor("Jeff P. Houde");
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   TriggerVolumeEvent::~TriggerVolumeEvent()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void TriggerVolumeEvent::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      EventNode::Init(nodeType, graph);

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Enter", "Activates when an actor has entered the volume."));
      mOutputs.push_back(OutputLink(this, "Leave", "Activates when an actor has left the volume."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void TriggerVolumeEvent::BuildPropertyMap()
   {
      EventNode::BuildPropertyMap();

      dtDAL::ActorIDActorProperty* triggerProp = new dtDAL::ActorIDActorProperty(
         "TriggerVolume", "Trigger Volume",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &TriggerVolumeEvent::SetTriggerVolume),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &TriggerVolumeEvent::GetTriggerVolume),
         "dtActors::TriggerVolumeActorProxy", "The Trigger Volume.");
      AddProperty(triggerProp);

      mValues.push_back(ValueLink(this, triggerProp, false, true, true));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void TriggerVolumeEvent::OnTriggerEvent(dtCore::Transformable* instigator, dtActors::TriggerVolumeActor::TriggerEventType eventType)
   {
      switch (eventType)
      {
      case dtActors::TriggerVolumeActor::ENTER_EVENT:
         Trigger("Enter", &instigator->GetUniqueId(), true);
         break;

      case dtActors::TriggerVolumeActor::LEAVE_EVENT:
         Trigger("Leave", &instigator->GetUniqueId(), false);
         break;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void TriggerVolumeEvent::OnLinkValueChanged(const std::string& linkName)
   {
      if (linkName == "TriggerVolume")
      {
         // Un-register all old volumes.
         int count = (int)mVolumes.size();
         for (int index = 0; index < count; index++)
         {
            dtActors::TriggerVolumeActor* volume = mVolumes[index].get();
            if (volume) volume->UnregisterListener(this);
         }
         mVolumes.clear();

         // Now register all new volumes.
         count = GetPropertyCount("TriggerVolume");
         for (int index = 0; index < count; index++)
         {
            dtDAL::BaseActorObject* proxy = GetActor("TriggerVolume", index);
            if (proxy)
            {
               dtActors::TriggerVolumeActor* volume = NULL;
               proxy->GetActor(volume);
               if (volume)
               {
                  mVolumes.push_back(volume);
                  volume->RegisterListener(this, dtActors::TriggerVolumeActor::EventFuncType(this, &TriggerVolumeEvent::OnTriggerEvent));
               }
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void TriggerVolumeEvent::SetTriggerVolume(const dtCore::UniqueId& value)
   {
      mTriggerVolume = value;
      OnLinkValueChanged("TriggerVolume");
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId TriggerVolumeEvent::GetTriggerVolume()
   {
      return mTriggerVolume;
   }
}
