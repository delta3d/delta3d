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
 * Author: Eric R. Heine
 */

#include <dtDirectorNodes/logaction.h>

#include <dtDAL/stringactorproperty.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   LogAction::LogAction()
      : ActionNode()
   {
      AddAuthor("Eric R. Heine");
   }

   ////////////////////////////////////////////////////////////////////////////////
   LogAction::~LogAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LogAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      SetMessage("");
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LogAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtDAL::StringActorProperty* messageProp = new dtDAL::StringActorProperty(
         "Message", "Message",
         dtDAL::StringActorProperty::SetFuncType(this, &LogAction::SetMessage),
         dtDAL::StringActorProperty::GetFuncType(this, &LogAction::GetMessage),
         "The message that will be logged.");
      AddProperty(messageProp);
   }

   //////////////////////////////////////////////////////////////////////////
   bool LogAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      LOG_ALWAYS(mMessage);

      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   //////////////////////////////////////////////////////////////////////////
   void LogAction::SetMessage(const std::string& value)
   {
      mMessage = value;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& LogAction::GetMessage()
   {
      return mMessage;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& LogAction::GetName()
   {
      return mMessage;
   }
}

////////////////////////////////////////////////////////////////////////////////
