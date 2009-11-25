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

#include <sstream>
#include <algorithm>

#include <dtDirectorNodes/namedevent.h>

#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/actorproxy.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   NamedEvent::NamedEvent()
       : EventNode()
       , mEventName("Event Name")
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   NamedEvent::~NamedEvent()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void NamedEvent::Init(const NodeType& nodeType)
   {
      EventNode::Init(nodeType);
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void NamedEvent::OnRemove() const
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NamedEvent::BuildPropertyMap()
   {
      EventNode::BuildPropertyMap();

      AddProperty(new dtDAL::StringActorProperty(
         "EventName", "Event Name",
         dtDAL::StringActorProperty::SetFuncType(this, &NamedEvent::SetEventName),
         dtDAL::StringActorProperty::GetFuncType(this, &NamedEvent::GetEventName),
         "The name of the event.", "Event"));
   }

   //////////////////////////////////////////////////////////////////////////
   void NamedEvent::SetEventName(const std::string& eventName)
   {
      mEventName = eventName;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& NamedEvent::GetEventName() const
   {
      return mEventName;
   }
}
