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
#include <dtDirectorNodes/externalvaluenode.h>
#include <dtDirectorNodes/referencevalue.h>

#include <dtCore/actorproperty.h>
#include <dtCore/enumactorproperty.h>

#include <dtDirector/nodemanager.h>
#include <dtDirector/valuelink.h>

#include <dtUtil/macros.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   ExternalValueNode::ExternalValueNode()
       : ValueNode()
       , mDefaultType(&dtCore::DataType::UNKNOWN)
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

      dtCore::EnumActorProperty<dtCore::DataType>* defaultTypeProp = new dtCore::EnumActorProperty<dtCore::DataType>(
         "DefaultType", "Default Type",
         dtCore::EnumActorProperty<dtCore::DataType>::SetFuncType(this, &ExternalValueNode::SetDefaultType),
         dtCore::EnumActorProperty<dtCore::DataType>::GetFuncType(this, &ExternalValueNode::GetDefaultType),
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

      UpdateLinkType();

      ValueNode::OnValueChanged();
   }

   //////////////////////////////////////////////////////////////////////////
   std::string ExternalValueNode::GetValueLabel()
   {
      std::string name = GetPropertyType().GetName() + "<br><b>" + GetName() + "</b>";

      if (!mValues.empty() && !mValues[0].GetLinks().empty())
      {
         ValueNode* valueNode = mValues[0].GetLinks()[0];
         while (valueNode)
         {
            // If we are connected to a reference node, show the value
            // begin referenced instead of the reference node.
            if (valueNode->GetType().GetFullName() == "Core.Reference")
            {
               ReferenceValue* refNode = dynamic_cast<ReferenceValue*>(valueNode);
               if (refNode)
               {
                  valueNode = refNode->GetReferencedValue();
                  continue;
               }
            }
            // If we are connected to another external value link node,
            // show the node being externally connected to instead.
            else if (valueNode->GetType().GetFullName() == "Core.Value Link")
            {
               ExternalValueNode* linkNode = dynamic_cast<ExternalValueNode*>(valueNode);
               if (linkNode && !linkNode->GetValueLinks().empty() &&
                  !linkNode->GetValueLinks()[0].GetLinks().empty())
               {
                  valueNode = linkNode->GetValueLinks()[0].GetLinks()[0];
                  continue;
               }
            }

            break;
         }

         if (valueNode)
         {
            return name + "<br><i>" + valueNode->GetValueLabel() +"</i>";
         }
      }

      return name;
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

   ///////////////////////////////////////////////////////////////////////////////
   std::string ExternalValueNode::GetFormattedValue()
   {
      if (!mValues.empty() && !mValues[0].GetLinks().empty())
      {
         ValueNode* valueNode = dynamic_cast<ValueNode*>(mValues[0].GetLinks()[0]);
         if (valueNode)
         {
            return valueNode->GetFormattedValue();
         }
      }

      return "";
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ExternalValueNode::SetFormattedValue(const std::string& value)
   {
      if (!mValues.empty() && !mValues[0].GetLinks().empty())
      {
         ValueNode* valueNode = dynamic_cast<ValueNode*>(mValues[0].GetLinks()[0]);
         if (valueNode)
         {
            valueNode->SetFormattedValue(value);
         }
      }
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

      return ValueNode::GetPropertyCount();
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::ActorProperty* ExternalValueNode::GetProperty(const std::string& name, int index, ValueNode** outNode)
   {
      if (mValues.size())
      {
         dtCore::ActorProperty* prop = mValues[0].GetProperty(index, outNode);
         if (prop && prop->GetName() == name) return prop;
      }

      return Node::GetProperty(name, index, outNode);
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::ActorProperty* ExternalValueNode::GetProperty(int index, ValueNode** outNode)
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
   bool ExternalValueNode::CanBeType(dtCore::DataType& type)
   {
      dtCore::DataType& myType = GetPropertyType();
      if (myType == dtCore::DataType::UNKNOWN ||
         myType == type)
      {
         return true;
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::DataType& ExternalValueNode::GetPropertyType()
   {
      // If we have a default type, use that
      dtCore::DataType& defaultType = GetDefaultType();
      if (defaultType != dtCore::DataType::UNKNOWN)
      {
         return defaultType;
      }

      // If we are linked to a value link, use the type of that link.
      if (mLinks.size())
      {
         int count = (int)mLinks.size();
         for (int index = 0; index < count; index++)
         {
            ValueLink* link = mLinks[index];
            Node* owner = link->GetOwner();
            ReferenceValue* refNode = dynamic_cast<ReferenceValue*>(owner);
            dtCore::DataType* type = NULL;
            if (refNode != NULL)
            {
               type = &refNode->GetNonReferencedPropertyType();
            }
            else if (owner != NULL && link->GetPropertyType() != dtCore::DataType::UNKNOWN)
            {
               type = &mLinks[index]->GetPropertyType();
            }

            if (type && *type != dtCore::DataType::UNKNOWN)
            {
               return *type;
            }
         }
      }

      // If we are linked to another value node, use that value's type.
      if (mValues.size() && mValues[0].GetLinks().size())
      {
         dtCore::DataType& type = mValues[0].GetLinks()[0]->GetPropertyType();
         if (type != dtCore::DataType::UNKNOWN)
         {
            return type;
         }
      }

      // If we have no connections yet, the type is undefined.
      return defaultType;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ExternalValueNode::SetDefaultType(const dtCore::DataType& value)
   {
      mDefaultType = &value;
      UpdateLinkType();
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::DataType& ExternalValueNode::GetDefaultType() const
   {
      return const_cast<dtCore::DataType&>(*mDefaultType);
   }

   //////////////////////////////////////////////////////////////////////////
   void ExternalValueNode::UpdateLinkType()
   {
      bool isOut = false;
      bool isTypeChecking = false;
      bool allowMultiple = false;

      osg::Vec3 color = GetType().GetColor();

      if (GetDefaultType() != dtCore::DataType::UNKNOWN)
      {
         dtCore::DataType& dataType = GetDefaultType();
         const NodeType* type = NodeManager::GetInstance().FindNodeType(dataType);
         if (type)
         {
            color = type->GetColor();
         }
      }

      if (mLinks.size())
      {
         int count = (int)mLinks.size();
         for (int index = 0; index < count; index++)
         {
            ValueLink* link = mLinks[index];
            if (link)
            {
               isOut |= link->IsOutLink();
               isTypeChecking |= link->IsTypeChecking();
               allowMultiple |= link->AllowMultiple();

               dtCore::DataType& dataType = link->GetPropertyType();
               const NodeType* type = NodeManager::GetInstance().FindNodeType(dataType);
               if (type)
               {
                  color = type->GetColor();
               }
            }
         }
      }

      // If we are not linked, reset to defaults.
      mValues[0].SetOutLink(isOut);
      mValues[0].SetTypeChecking(isTypeChecking);
      mValues[0].SetAllowMultiple(allowMultiple);

      if (!mValues[0].GetLinks().empty())
      {
         SetColor(mValues[0].GetLinks()[0]->GetColor());
      }
      else
      {
         SetColorRGB(color);
      }
   }
}
