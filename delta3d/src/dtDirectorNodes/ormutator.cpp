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
#include <dtDirectorNodes/ormutator.h>

#include <dtCore/booleanactorproperty.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   OrMutator::OrMutator()
      : MutatorNode()
      , mValue(false)
      , mResult(false)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   OrMutator::~OrMutator()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OrMutator::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      MutatorNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OrMutator::BuildPropertyMap()
   {
      MutatorNode::BuildPropertyMap();

      // Create our value links.
      dtCore::BooleanActorProperty* valuesProp = new dtCore::BooleanActorProperty(
         "Values", "Values",
         dtCore::BooleanActorProperty::SetFuncType(this, &OrMutator::SetValue),
         dtCore::BooleanActorProperty::GetFuncType(this, &OrMutator::GetValue),
         "The values to test.");
      AddProperty(valuesProp);

      mProperty = new dtCore::BooleanActorProperty(
         "Result", "Result",
         dtCore::BooleanActorProperty::SetFuncType(this, &OrMutator::SetResult),
         dtCore::BooleanActorProperty::GetFuncType(this, &OrMutator::GetResult),
         "The Result value.");
      AddProperty(mProperty);

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, valuesProp, false, true, true));

      OnLinkValueChanged("Values");
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string OrMutator::GetValueLabel()
   {
      return "|";
   }

   //////////////////////////////////////////////////////////////////////////
   void OrMutator::OnLinkValueChanged(const std::string& linkName)
   {
      MutatorNode::OnLinkValueChanged(linkName);

      bool result = false;
      int count = GetPropertyCount("Values");
      for (int index = 0; index < count; ++index)
      {
         if (GetBoolean("Values", index))
         {
            result = true;
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
