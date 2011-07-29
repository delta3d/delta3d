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
#include <dtDirectorNodes/setarrayaction.h>

#include <dtCore/transform.h>
#include <dtCore/transformable.h>

#include <dtCore/arrayactorpropertybase.h>
#include <dtCore/intactorproperty.h>
#include <dtCore/stringactorproperty.h>

#include <dtDirector/arrayvaluenode.h>
#include <dtDirector/director.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   SetArrayAction::SetArrayAction()
      : ActionNode()
      , mIndex(-1)
   {
      AddAuthor("Jeff P. Houde");
   }

   /////////////////////////////////////////////////////////////////////////////
   SetArrayAction::~SetArrayAction()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetArrayAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      // Create multiple inputs for different operations.
      mInputs.clear();
      mInputs.push_back(InputLink(this, "Set", "Sets the contents of an array."));
      mInputs.push_back(InputLink(this, "Insert", "Inserts more values into an array."));
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetArrayAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtCore::IntActorProperty* arrayProp = new dtCore::IntActorProperty(
         "Array", "Array",
         dtCore::IntActorProperty::SetFuncType(this, &SetArrayAction::SetArray),
         dtCore::IntActorProperty::GetFuncType(this, &SetArrayAction::GetArray),
         "The array to set or insert into.");

      dtCore::IntActorProperty* indexProp = new dtCore::IntActorProperty(
         "Index", "Index",
         dtCore::IntActorProperty::SetFuncType(this, &SetArrayAction::SetIndex),
         dtCore::IntActorProperty::GetFuncType(this, &SetArrayAction::GetIndex),
         "The index of the array to set or insert into (-1 appends to end on insert).");
      AddProperty(indexProp);

      dtCore::StringActorProperty* valueProp = new dtCore::StringActorProperty(
         "Value", "Value",
         dtCore::StringActorProperty::SetFuncType(this, &SetArrayAction::SetValue),
         dtCore::StringActorProperty::GetFuncType(this, &SetArrayAction::GetValue),
         "The value to set or insert into the array.");
      AddProperty(valueProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, arrayProp, true, false, false, true));    
      mValues.push_back(ValueLink(this, indexProp, false, false, true, true));    
      mValues.push_back(ValueLink(this, valueProp, false, false, false, true));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool SetArrayAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      int arrayCount = GetPropertyCount("Array");
      int arrayIndex = GetInt("Index");
      std::string newValue = GetString("Value");

      if (arrayIndex < 0)
      {
         arrayIndex = arrayCount;
      }

      switch (input)
      {
      case INPUT_INSERT:
         {
            // Retrieve the actual array value node.
            ValueNode* valueNode = NULL;
            GetProperty("Array", arrayIndex, &valueNode);

            ArrayValueNode* arrayNode = dynamic_cast<ArrayValueNode*>(valueNode);
            if (arrayNode)
            {
               dtCore::ArrayActorPropertyBase* arrayProp = arrayNode->GetArrayProperty();
               if (arrayProp)
               {
                  arrayProp->Insert(arrayIndex);
                  arrayCount = GetPropertyCount("Array");
               }
            }
         }

      case INPUT_SET:
         {
            if (arrayIndex < arrayCount)
            {
               SetString(newValue, "Array", arrayIndex);
            }
         }
         break;
      }

      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetArrayAction::SetArray(int value)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   int SetArrayAction::GetArray() const
   {
      return 0;
   }

   /////////////////////////////////////////////////////////////////////////////
   void SetArrayAction::SetIndex(int value)
   {
      mIndex = value;
   }

   /////////////////////////////////////////////////////////////////////////////
   int SetArrayAction::GetIndex() const
   {
      return mIndex;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SetArrayAction::SetValue(const std::string& value)
   {
      mValue = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string SetArrayAction::GetValue() const
   {
      return mValue;
   }
}

////////////////////////////////////////////////////////////////////////////////
