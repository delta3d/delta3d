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
#include <dtDirectorNodes/remoteevent.h>

#include <dtDAL/actorproxy.h>
#include <dtDAL/stringactorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   RemoteEvent::RemoteEvent()
       : EventNode()
   {
      AddAuthor("Jeff P. Houde");
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   RemoteEvent::~RemoteEvent()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void RemoteEvent::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      SetEventName("Event Name");

      EventNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void RemoteEvent::BuildPropertyMap()
   {
      EventNode::BuildPropertyMap();
      RemoveProperty("Name");

      dtDAL::StringActorProperty* nameProp = new dtDAL::StringActorProperty(
         "EventName", "Event Name",
         dtDAL::StringActorProperty::SetFuncType(this, &RemoteEvent::SetEventName),
         dtDAL::StringActorProperty::GetFuncType(this, &RemoteEvent::GetEventName),
         "The name of the event.");
      AddProperty(nameProp);

      mValues.push_back(ValueLink(this, nameProp, false, false, true, false));

      ValueLink* link = GetValueLink("ActorFilters");
      if (link)
      {
         link->SetExposed(false);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void RemoteEvent::SetEventName(const std::string& eventName)
   {
      mEventName = mName = eventName;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& RemoteEvent::GetEventName() const
   {
      return mEventName;
   }
}
