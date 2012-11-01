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
#include <dtDirectorNodes/referencevalue.h>

#include <dtCore/propertymacros.h>

#include <dtDirector/director.h>
#include <dtDirector/valuelink.h>
#include <dtDirector/colors.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   ReferenceValue::ReferenceValue()
       : ValueNode()
   {
      AddAuthor("Jeff P. Houde");
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   ReferenceValue::~ReferenceValue()
   {
      Disconnect();
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ReferenceValue::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ValueNode::Init(nodeType, graph);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ReferenceValue::BuildPropertyMap()
   {
      ValueNode::BuildPropertyMap();
      RemoveProperty("Global");

      dtCore::StringActorProperty* refProp = new dtCore::StringActorProperty(
         "Reference", "Referenced Value",
         dtCore::StringActorProperty::SetFuncType(this, &ReferenceValue::SetReference),
         dtCore::StringActorProperty::GetFuncType(this, &ReferenceValue::GetReference),
         "This will set the referenced value, set this to the same name as the value you want to reference.");
      AddProperty(refProp);

      // The external value requires a value link so it can use it to link
      // externally with any values outside of it.
      mValues.clear();
      mValues.push_back(ValueLink(this, NULL, false, false, false, false));
   }

   //////////////////////////////////////////////////////////////////////////
   void ReferenceValue::OnLinkValueChanged(const std::string& linkName)
   {
      ValueNode::OnLinkValueChanged(linkName);

      // If the linked value has changed, make sure we notify.
      OnValueChanged();
   }

   //////////////////////////////////////////////////////////////////////////
   void ReferenceValue::SetName(const std::string& name)
   {
      ValueNode::SetName(name);
   }

   //////////////////////////////////////////////////////////////////////////
   std::string ReferenceValue::GetValueLabel()
   {
      if (!mValues.empty() && !mValues[0].GetLinks().empty())
      {
         ValueNode* valueNode = dynamic_cast<ValueNode*>(mValues[0].GetLinks()[0]);
         if (valueNode)
         {
            return "<i>"+ valueNode->GetTypeName() + "<br><b>" + valueNode->GetName() + "</b><br>" + valueNode->GetValueLabel() +"</i>";
         }
      }

      return "";
   }

   ///////////////////////////////////////////////////////////////////////////////
   std::string ReferenceValue::GetFormattedValue()
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
   void ReferenceValue::SetFormattedValue(const std::string& value)
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
   void ReferenceValue::OnConnectionChange()
   {
      UpdateReference();
      UpdateLinkType();
      ValueNode::OnConnectionChange();
   }

   //////////////////////////////////////////////////////////////////////////
   int ReferenceValue::GetPropertyCount(const std::string& name)
   {
      if (name == "Value" && !mValues.empty())
      {
         return mValues[0].GetPropertyCount();
      }

      return Node::GetPropertyCount(name);
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::ActorProperty* ReferenceValue::GetProperty(const std::string& name, int index, ValueNode** outNode)
   {
      if (name == "Value" && !mValues.empty())
      {
         dtCore::ActorProperty* prop = mValues[0].GetProperty(index, outNode);
         //if (prop && prop->GetName() == name) return prop;
         if (prop) return prop;
      }

      return Node::GetProperty(name, index, outNode);
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::ActorProperty* ReferenceValue::GetProperty(int index, ValueNode** outNode)
   {
      if (!mValues.empty() && !mValues[0].GetLinks().empty())
      {
         return mValues[0].GetLinks()[0]->GetProperty(index, outNode);
      }

      return ValueNode::GetProperty(index, outNode);
   }

   //////////////////////////////////////////////////////////////////////////
   bool ReferenceValue::CanBeType(dtCore::DataType& type)
   {
      dtCore::DataType& myType = GetPropertyType();

      return myType == dtCore::DataType::UNKNOWN || myType == type;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::DataType& ReferenceValue::GetPropertyType()
   {
      // If we are linked to another value node, use that value's type.
      if (!mValues.empty() && !mValues[0].GetLinks().empty())
      {
         dtCore::DataType& type = mValues[0].GetLinks()[0]->GetPropertyType();
         if (type != dtCore::DataType::UNKNOWN)
         {
            return type;
         }
      }

      return GetNonReferencedPropertyType();
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::DataType& ReferenceValue::GetNonReferencedPropertyType()
   {
      // If we are linked to a value link, use the type of that link.
      if (!mLinks.empty())
      {
         int count = (int)mLinks.size();
         for (int index = 0; index < count; index++)
         {
            dtCore::DataType& type = mLinks[index]->GetPropertyType();
            if (type != dtCore::DataType::UNKNOWN)
            {
               return type;
            }
         }
      }

      // If we have no connections yet, the type is undefined.
      return dtCore::DataType::UNKNOWN;
   }

   //////////////////////////////////////////////////////////////////////////
   void ReferenceValue::UpdateLinkType()
   {
      bool isOut = false;
      bool isTypeChecking = false;

      if (!mLinks.empty())
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

   //////////////////////////////////////////////////////////////////////////
   void ReferenceValue::SetReference(const std::string& value)
   {
      if (mReference != value)
      {
         mReference = value;

         // Now attempt to find the actual value and link with it.
         UpdateReference();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& ReferenceValue::GetReference()
   {
      return mReference;
   }

   //////////////////////////////////////////////////////////////////////////
   ValueNode* ReferenceValue::GetReferencedValue()
   {
      // Search for the referenced node anywhere in the current graph
      // or any of its parents.
      ValueNode* node      = NULL;
      DirectorGraph* graph = GetGraph();
      while (graph)
      {
         if ((node = graph->GetValueNode(mReference, false, true)) != NULL)
         {
            break;
         }
         graph = graph->GetParent();
      }

      return node;
   }

   //////////////////////////////////////////////////////////////////////////
   void ReferenceValue::UpdateReference()
   {
      if (!mReference.empty())
      {
         ValueNode* node = GetReferencedValue();
         if (node)
         {
            // Disconnect all links that are no longer valid based on type.
            if (!mLinks.empty())
            {
               int count = (int)mLinks.size();
               for (int index = 0; index < count; index++)
               {
                  ValueLink* link = mLinks[index];
                  if (!link || !link->GetOwner()) continue;

                  if (!link->GetOwner()->CanConnectValue(link, node))
                  {
                     mLinks[index]->Disconnect(this);
                     index--;
                     count--;
                  }
               }
            }

            if (node != this)
            {
               mValues[0].Connect(node);
               SetColor(node->GetColor());
            }
            return;
         }
      }

      // If we can't find the node, make sure we are not
      // connected to anything.
      mValues[0].Disconnect();
      SetColorRGB(Colors::GRAY);
   }
}
