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

#include <dtDirector/mutatornode.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   MutatorNode::MutatorNode()
      : ValueNode()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   MutatorNode::~MutatorNode()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MutatorNode::BuildPropertyMap()
   {
      Node::BuildPropertyMap();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string MutatorNode::GetValueLabel()
   {
      return "";
   }

   ////////////////////////////////////////////////////////////////////////////////
   int MutatorNode::GetPropertyCount(const std::string& name)
   {
      int propertyCount = 0;

      // First iterate through all value links to see if this property
      // is redirected.
      for (int valueIndex = 0; valueIndex < (int)mValues.size(); valueIndex++)
      {
         dtDAL::ActorProperty* prop = mValues[valueIndex].GetDefaultProperty();
         if (prop && prop->GetName() == name)
         {
            propertyCount = mValues[valueIndex].GetPropertyCount();
         }
      }

      // Did not find any overrides, so return the default.
      if (propertyCount == 0)
      {
         if (name == "Value" || dtDAL::PropertyContainer::GetProperty(name))
         {
            return 1;
         }
      }

      return propertyCount;
   }
}

////////////////////////////////////////////////////////////////////////////////
