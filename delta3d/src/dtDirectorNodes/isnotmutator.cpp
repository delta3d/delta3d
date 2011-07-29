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
#include <dtDirectorNodes/isnotmutator.h>

#include <dtCore/booleanactorproperty.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   IsNotMutator::IsNotMutator()
      : MutatorNode()
      , mValue(true)
      , mResult(false)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   IsNotMutator::~IsNotMutator()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IsNotMutator::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      MutatorNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IsNotMutator::BuildPropertyMap()
   {
      MutatorNode::BuildPropertyMap();

      // Create our value links.
      dtCore::BooleanActorProperty* valueProp = new dtCore::BooleanActorProperty(
         "Value", "Value",
         dtCore::BooleanActorProperty::SetFuncType(this, &IsNotMutator::SetValue),
         dtCore::BooleanActorProperty::GetFuncType(this, &IsNotMutator::GetValue),
         "The value to negate.");
      AddProperty(valueProp);

      mProperty = new dtCore::BooleanActorProperty(
         "Result", "Result",
         dtCore::BooleanActorProperty::SetFuncType(this, &IsNotMutator::SetResult),
         dtCore::BooleanActorProperty::GetFuncType(this, &IsNotMutator::GetResult),
         "The Result value.");
      AddProperty(mProperty);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, valueProp, false, false, true));

      OnLinkValueChanged("Value");
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string IsNotMutator::GetValueLabel()
   {
      return "!=";
   }

   //////////////////////////////////////////////////////////////////////////
   void IsNotMutator::OnLinkValueChanged(const std::string& linkName)
   {
      MutatorNode::OnLinkValueChanged(linkName);

      bool value = GetBoolean("Value");

      if (GetBoolean("Result") != !value)
      {
         SetBoolean(!value, "Result");

         OnValueChanged();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
