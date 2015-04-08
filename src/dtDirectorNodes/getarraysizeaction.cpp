/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009 MOVES Institute
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
#include <dtDirectorNodes/getarraysizeaction.h>

#include <dtCore/intactorproperty.h>

#include <dtDirector/director.h>
#include <dtDirector/arrayvaluenode.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   GetArraySizeAction::GetArraySizeAction()
      : ActionNode()
   {
      AddAuthor("Jeff P. Houde");
   }

   /////////////////////////////////////////////////////////////////////////////
   GetArraySizeAction::~GetArraySizeAction()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetArraySizeAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetArraySizeAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtCore::IntActorProperty* arrayProp = new dtCore::IntActorProperty(
         "Array", "Array",
         dtCore::IntActorProperty::SetFuncType(this, &GetArraySizeAction::SetArray),
         dtCore::IntActorProperty::GetFuncType(this, &GetArraySizeAction::GetArray),
         "The array to retrieve a size from.");

      dtCore::IntActorProperty* sizeProp = new dtCore::IntActorProperty(
         "Size", "Size",
         dtCore::IntActorProperty::SetFuncType(this, &GetArraySizeAction::SetSize),
         dtCore::IntActorProperty::GetFuncType(this, &GetArraySizeAction::GetSize),
         "The output size of the array.");

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, arrayProp, false, false, false, true));    
      mValues.push_back(ValueLink(this, sizeProp, true, true, true, true));    
   }

   /////////////////////////////////////////////////////////////////////////////
   bool GetArraySizeAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      int arrayCount = GetPropertyCount("Array");
      SetInt(arrayCount, "Size");

      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetArraySizeAction::SetArray(int value)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   int GetArraySizeAction::GetArray() const
   {
      return 0;
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetArraySizeAction::SetSize(int value)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   int GetArraySizeAction::GetSize() const
   {
      return 0;
   }
}

////////////////////////////////////////////////////////////////////////////////
