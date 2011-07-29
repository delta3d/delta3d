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
#include <dtDirectorNodes/setvalueaction.h>

#include <dtCore/stringactorproperty.h>

#include <dtDirector/director.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   SetValueAction::SetValueAction()
      : ActionNode()
      , mResultProp(NULL)
   {
      AddAuthor("Jeff P. Houde");
   }

   ////////////////////////////////////////////////////////////////////////////////
   SetValueAction::~SetValueAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SetValueAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SetValueAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtCore::StringActorProperty* sourceProp = new dtCore::StringActorProperty(
         "Source", "In Value",
         dtCore::StringActorProperty::SetFuncType(this, &SetValueAction::SetSrc),
         dtCore::StringActorProperty::GetFuncType(this, &SetValueAction::GetSrc),
         "The Source value.");

      mResultProp = new dtCore::StringActorProperty(
         "Destination", "Out Value",
         dtCore::StringActorProperty::SetFuncType(this, &SetValueAction::SetDest),
         dtCore::StringActorProperty::GetFuncType(this, &SetValueAction::GetDest),
         "The Destination value.");

      AddProperty(sourceProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, sourceProp, false, false, false));
      mValues.push_back(ValueLink(this, mResultProp.get(), true, true, false));
   }

   //////////////////////////////////////////////////////////////////////////
   bool SetValueAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      // Perform math!
      std::string source = GetString("Source");

      SetString(source, "Destination");

      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   //////////////////////////////////////////////////////////////////////////
   void SetValueAction::SetSrc(const std::string& value)
   {
      mSource = value;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& SetValueAction::GetSrc()
   {
      return mSource;
   }

   //////////////////////////////////////////////////////////////////////////
   void SetValueAction::SetDest(const std::string& value)
   {
      mDest = value;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& SetValueAction::GetDest()
   {
      return mDest;
   }
}

////////////////////////////////////////////////////////////////////////////////
