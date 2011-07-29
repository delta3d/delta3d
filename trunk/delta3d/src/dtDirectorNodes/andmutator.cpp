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
#include <dtDirectorNodes/andmutator.h>

#include <dtCore/booleanactorproperty.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   AndMutator::AndMutator()
      : MutatorNode()
      , mValue(false)
      , mResult(false)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   AndMutator::~AndMutator()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AndMutator::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      MutatorNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AndMutator::BuildPropertyMap()
   {
      MutatorNode::BuildPropertyMap();

      // Create our value links.
      dtCore::BooleanActorProperty* valuesProp = new dtCore::BooleanActorProperty(
         "Values", "Values",
         dtCore::BooleanActorProperty::SetFuncType(this, &AndMutator::SetValue),
         dtCore::BooleanActorProperty::GetFuncType(this, &AndMutator::GetValue),
         "Results in true if all values are also true.");
      AddProperty(valuesProp);

      mProperty = new dtCore::BooleanActorProperty(
         "Result", "Result",
         dtCore::BooleanActorProperty::SetFuncType(this, &AndMutator::SetResult),
         dtCore::BooleanActorProperty::GetFuncType(this, &AndMutator::GetResult),
         "The Result value.");
      AddProperty(mProperty);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, valuesProp, false, true));

      OnLinkValueChanged("Values");
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string AndMutator::GetValueLabel()
   {
      return "&";
   }

   //////////////////////////////////////////////////////////////////////////
   void AndMutator::OnLinkValueChanged(const std::string& linkName)
   {
      MutatorNode::OnLinkValueChanged(linkName);

      bool result = true;
      int count = GetPropertyCount("Values");
      for (int index = 0; index < count; ++index)
      {
         if (GetBoolean("Values", index) == false)
         {
            result = false;
            break;
         }
      }

      if (GetBoolean("Result") != result)
      {
         SetBoolean(result, "Result");

         OnValueChanged();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
