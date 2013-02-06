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
#include <dtDirectorNodes/removearrayaction.h>

#include <dtCore/arrayactorpropertybase.h>
#include <dtCore/intactorproperty.h>

#include <dtDirector/arrayvaluenode.h>
#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   RemoveArrayAction::RemoveArrayAction()
      : ActionNode()
      , mIndex(-1)
   {
      AddAuthor("Jeff P. Houde");
   }

   /////////////////////////////////////////////////////////////////////////////
   RemoveArrayAction::~RemoveArrayAction()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void RemoveArrayAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);
   }

   /////////////////////////////////////////////////////////////////////////////
   void RemoveArrayAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtCore::IntActorProperty* arrayProp = new dtCore::IntActorProperty(
         "Array", "Array",
         dtCore::IntActorProperty::SetFuncType(this, &RemoveArrayAction::SetArray),
         dtCore::IntActorProperty::GetFuncType(this, &RemoveArrayAction::GetArray),
         "The array to remove from.");

      dtCore::IntActorProperty* indexProp = new dtCore::IntActorProperty(
         "Index", "Index",
         dtCore::IntActorProperty::SetFuncType(this, &RemoveArrayAction::SetIndex),
         dtCore::IntActorProperty::GetFuncType(this, &RemoveArrayAction::GetIndex),
         "The index of the array to remove (-1 clears the entire array).");
      AddProperty(indexProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, arrayProp, true, false, false, true));    
      mValues.push_back(ValueLink(this, indexProp, false, false, true, true));    
   }

   /////////////////////////////////////////////////////////////////////////////
   bool RemoveArrayAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      int arrayIndex = GetInt("Index");

      // Retrieve the actual array value node.
      ValueNode* valueNode = NULL;
      GetProperty("Array", arrayIndex, &valueNode);

      ArrayValueNode* arrayNode = dynamic_cast<ArrayValueNode*>(valueNode);
      if (arrayNode)
      {
         dtCore::ArrayActorPropertyBase* arrayProp = arrayNode->GetArrayProperty();
         if (arrayProp)
         {
            if (arrayIndex < 0)
            {
               arrayProp->Clear();
            }
            else
            {
               arrayProp->Remove(arrayIndex);
            }
         }
      }

      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   /////////////////////////////////////////////////////////////////////////////
   void RemoveArrayAction::SetArray(int value)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   int RemoveArrayAction::GetArray() const
   {
      return 0;
   }

   /////////////////////////////////////////////////////////////////////////////
   void RemoveArrayAction::SetIndex(int value)
   {
      mIndex = value;
   }

   /////////////////////////////////////////////////////////////////////////////
   int RemoveArrayAction::GetIndex() const
   {
      return mIndex;
   }
}

////////////////////////////////////////////////////////////////////////////////
