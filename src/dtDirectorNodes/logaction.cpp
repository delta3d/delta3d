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
#include <prefix/dtdirectornodesprefix.h>
#include <dtDirectorNodes/logaction.h>

#include <dtCore/stringactorproperty.h>

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
      dtCore::StringActorProperty* messageProp = new dtCore::StringActorProperty(
         "Message", "Message",
         dtCore::StringActorProperty::SetFuncType(this, &LogAction::SetMessage),
         dtCore::StringActorProperty::GetFuncType(this, &LogAction::GetMessage),
         "The message that will be logged.");
      AddProperty(messageProp);

      dtCore::StringActorProperty* valueProp = new dtCore::StringActorProperty(
         "Value", "Value",
         dtCore::StringActorProperty::SetFuncType(this, &LogAction::SetValue),
         dtCore::StringActorProperty::GetFuncType(this, &LogAction::GetValue),
         "The value(s) that will be logged.");
      mValues.push_back(ValueLink(this, valueProp, false, true, false));
   }

   //////////////////////////////////////////////////////////////////////////
   bool LogAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      if (!mMessage.empty())
      {
         LOG_ALWAYS(mMessage);
      }

      int count = GetPropertyCount("Value");
      for (int index = 0; index < count; index++)
      {
         std::string value = GetString("Value", index);
         if (!value.empty())
         {
            LOG_ALWAYS("Value: " + value);
         }
      }

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
   void LogAction::SetValue(const std::string& value)
   {
      mValue = value;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& LogAction::GetValue()
   {
      return mValue;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& LogAction::GetName()
   {
      return mMessage;
   }
}

////////////////////////////////////////////////////////////////////////////////
