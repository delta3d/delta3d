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

#include <dtDirectorNodes/externalvaluenode.h>

#include <dtDAL/actorproperty.h>
#include <dtDAL/enumactorproperty.h>

#include <dtDirector/valuelink.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   ExternalValueNode::ExternalValueNode()
       : ValueNode()
       , mDefaultType(&dtDAL::DataType::UNKNOWN)
   {
      mName = "Value";
      AddAuthor("Jeff P. Houde");
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   ExternalValueNode::~ExternalValueNode()
   {
      Disconnect();
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ExternalValueNode::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      Node::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ExternalValueNode::BuildPropertyMap()
   {
      ValueNode::BuildPropertyMap();

      dtDAL::EnumActorProperty<dtDAL::DataType>* defaultTypeProp = new dtDAL::EnumActorProperty<dtDAL::DataType>(
         "DefaultType", "Default Type",
         dtDAL::EnumActorProperty<dtDAL::DataType>::SetFuncType(this, &ExternalValueNode::SetDefaultType),
         dtDAL::EnumActorProperty<dtDAL::DataType>::GetFuncType(this, &ExternalValueNode::GetDefaultType),
         "The default type of this value link node.");
      AddProperty(defaultTypeProp);

      // The external value requires a value link so it can use it to link
      // externally with any values outside of it.
      mValues.clear();
      mValues.push_back(ValueLink(this, NULL, false, false, false, true));
   }

   //////////////////////////////////////////////////////////////////////////
   void ExternalValueNode::OnLinkValueChanged(const std::string& linkName)
   {
      ValueNode::OnLinkValueChanged(linkName);

      // If the linked value has changed, make sure we notify.
      OnValueChanged();
   }

   //////////////////////////////////////////////////////////////////////////
   void ExternalValueNode::SetName(const std::string& name)
   {
      if( !name.empty() )
      {
         ValueNode::SetName(name);
         mValues[0].SetName(name);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   std::string ExternalValueNode::GetValueLabel()
   {
      return mValues[0].GetDisplayName();
   }

   //////////////////////////////////////////////////////////////////////////
   void ExternalValueNode::OnConnectionChange()
   {
      UpdateLinkType();
      ValueNode::OnConnectionChange();
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
   dtDAL::ActorProperty* ExternalValueNode::GetProperty(const std::string& name, int index, ValueNode** outNode)
   {
      if (mValues.size())
      {
         dtDAL::ActorProperty* prop = mValues[0].GetProperty(index, outNode);
         if (prop && prop->GetName() == name) return prop;
      }

      return Node::GetProperty(name, index, outNode);
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProperty* ExternalValueNode::GetProperty(int index, ValueNode** outNode)
   {
      if (mValues.size())
      {
         if (mValues[0].GetLinks().size())
         {
            return mValues[0].GetLinks()[0]->GetProperty(index, outNode);
         }
      }

      return ValueNode::GetProperty(index, outNode);
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
      return GetDefaultType();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ExternalValueNode::SetDefaultType(const dtDAL::DataType& value)
   {
      mDefaultType = &value;
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtDAL::DataType& ExternalValueNode::GetDefaultType() const
   {
      return const_cast<dtDAL::DataType&>(*mDefaultType);
   }

   //////////////////////////////////////////////////////////////////////////
   void ExternalValueNode::UpdateLinkType()
   {
      bool isOut = false;
      bool isTypeChecking = false;
      bool allowMultiple = false;

      if (mLinks.size())
      {
         int count = (int)mLinks.size();
         for (int index = 0; index < count; index++)
         {
            isOut |= mLinks[index]->IsOutLink();
            isTypeChecking |= mLinks[index]->IsTypeChecking();
            allowMultiple |= mLinks[index]->AllowMultiple();
         }
      }

      // If we are not linked, reset to defaults.
      mValues[0].SetOutLink(isOut);
      mValues[0].SetTypeChecking(isTypeChecking);
      mValues[0].SetAllowMultiple(allowMultiple);
   }
}
