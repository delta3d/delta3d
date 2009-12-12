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

#include <sstream>
#include <algorithm>

#include <dtDirectorNodes/externalvaluenode.h>
#include <dtDirector/valuelink.h>

#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   ExternalValueNode::ExternalValueNode()
       : ValueNode()
   {
      mName = "External Variable";
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   ExternalValueNode::~ExternalValueNode()
   {
      Disconnect();
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ExternalValueNode::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ValueNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ExternalValueNode::BuildPropertyMap()
   {
      ValueNode::BuildPropertyMap();

      // The external value requires a value link so it can use it to link
      // externally with any values outside of it.
      mValues.clear();
      mValues.push_back(ValueLink(this, NULL, false, false, false));
   }

   //////////////////////////////////////////////////////////////////////////
   void ExternalValueNode::SetName(const std::string& name)
   {
      ValueNode::SetName(name);

      mValues[0].SetLabel(name);
   }

   //////////////////////////////////////////////////////////////////////////
   void ExternalValueNode::OnConnectionChange()
   {
      UpdateLinkType();
   }

   //////////////////////////////////////////////////////////////////////////
   int ExternalValueNode::GetPropertyCount(const std::string& name)
   {
      if (mValues.size())
      {
         return mValues[0].GetPropertyCount();
      }

      return Node::GetPropertyCount();
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProperty* ExternalValueNode::GetProperty(const std::string& name, int index)
   {
      if (mValues.size())
      {
         return mValues[0].GetProperty(index);
      }

      return Node::GetProperty(name, index);
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProperty* ExternalValueNode::GetProperty()
   {
      if (mValues.size())
      {
         if (mValues[0].GetLinks().size())
         {
            return mValues[0].GetLinks()[0]->GetProperty();
         }
      }

      return ValueNode::GetProperty();
   }

   //////////////////////////////////////////////////////////////////////////
   bool ExternalValueNode::CanBeType(dtDAL::DataType& type)
   {
      dtDAL::DataType& myType = GetPropertyType();
      if (myType == dtDAL::DataType::UNKNOWN ||
         myType == type)
      {
         return true;
      }
      
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::DataType& ExternalValueNode::GetPropertyType()
   {
      // If we are linked to a value link, use the type of that link.
      if (mLinks.size())
      {
         int count = (int)mLinks.size();
         for (int index = 0; index < count; index++)
         {
            if (mLinks[index]->GetPropertyType() != dtDAL::DataType::UNKNOWN)
            {
               return mLinks[index]->GetPropertyType();
            }
         }
      }

      // If we are linked to another value node, use that value's type.
      if (mValues.size() && mValues[0].GetLinks().size())
      {
         return mValues[0].GetLinks()[0]->GetPropertyType();
      }

      // If we have no connections yet, the type is undefined.
      return dtDAL::DataType::UNKNOWN;
   }

   //////////////////////////////////////////////////////////////////////////
   void ExternalValueNode::UpdateLinkType()
   {
      bool isOut = false;
      bool isTypeChecking = false;

      if (mLinks.size())
      {
         int count = (int)mLinks.size();
         for (int index = 0; index < count; index++)
         {
            isOut |= mLinks[index]->IsOutLink();
            isTypeChecking |= mLinks[index]->IsTypeChecking();
         }
      }

      // If we are not linked, reset to defaults.
      mValues[0].SetOutLink(isOut);
      mValues[0].SetTypeChecking(isTypeChecking);
   }
}
