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
#include <dtDirectorNodes/getarrayaction.h>

#include <dtCore/arrayactorpropertybase.h>
#include <dtCore/intactorproperty.h>
#include <dtCore/stringactorproperty.h>

#include <dtDirector/director.h>
#include <dtDirector/arrayvaluenode.h>

namespace dtDirector
{
   /////////////////////////////////////////////////////////////////////////////
   GetArrayAction::GetArrayAction()
      : ActionNode()
      , mIndex(-1)
   {
      AddAuthor("Jeff P. Houde");
   }

   /////////////////////////////////////////////////////////////////////////////
   GetArrayAction::~GetArrayAction()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetArrayAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetArrayAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Create our value links.
      dtCore::IntActorProperty* arrayProp = new dtCore::IntActorProperty(
         "Array", "Array",
         dtCore::IntActorProperty::SetFuncType(this, &GetArrayAction::SetArray),
         dtCore::IntActorProperty::GetFuncType(this, &GetArrayAction::GetArray),
         "The array to retrieve a value from.");

      dtCore::IntActorProperty* indexProp = new dtCore::IntActorProperty(
         "Index", "Index",
         dtCore::IntActorProperty::SetFuncType(this, &GetArrayAction::SetIndex),
         dtCore::IntActorProperty::GetFuncType(this, &GetArrayAction::GetIndex),
         "The index of the array.");
      AddProperty(indexProp);

      dtCore::StringActorProperty* valueProp = new dtCore::StringActorProperty(
         "Value", "Value",
         dtCore::StringActorProperty::SetFuncType(this, &GetArrayAction::SetValue),
         dtCore::StringActorProperty::GetFuncType(this, &GetArrayAction::GetValue),
         "The retrieved value.");
      AddProperty(valueProp);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, arrayProp, false, false, false, true));    
      mValues.push_back(ValueLink(this, indexProp, false, false, true, true));    
      mValues.push_back(ValueLink(this, valueProp, true, true, false, true));
   }

   /////////////////////////////////////////////////////////////////////////////
   bool GetArrayAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      int arrayCount = GetPropertyCount("Array");
      int arrayIndex = GetInt("Index");

      if (arrayIndex >= 0 && arrayIndex < arrayCount)
      {
         std::string val = GetString("Array", arrayIndex);
         SetString(val, "Value");
      }

      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetArrayAction::SetArray(int value)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   int GetArrayAction::GetArray() const
   {
      return 0;
   }

   /////////////////////////////////////////////////////////////////////////////
   void GetArrayAction::SetIndex(int value)
   {
      mIndex = value;
   }

   /////////////////////////////////////////////////////////////////////////////
   int GetArrayAction::GetIndex() const
   {
      return mIndex;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GetArrayAction::SetValue(const std::string& value)
   {
      mValue = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string GetArrayAction::GetValue() const
   {
      return mValue;
   }
}

////////////////////////////////////////////////////////////////////////////////
