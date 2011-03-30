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

#include <dtDirectorNodes/referencevalue.h>

#include <dtDAL/stringactorproperty.h>

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

      dtDAL::StringActorProperty* refProp = new dtDAL::StringActorProperty(
         "Reference", "Referenced Value",
         dtDAL::StringActorProperty::SetFuncType(this, &ReferenceValue::SetReference),
         dtDAL::StringActorProperty::GetFuncType(this, &ReferenceValue::GetReference),
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
      //OnValueChanged();
   }

   //////////////////////////////////////////////////////////////////////////
   void ReferenceValue::SetName(const std::string& name)
   {
      ValueNode::SetName(name);
      //mValues[0].SetName(name);
   }

   //////////////////////////////////////////////////////////////////////////
   std::string ReferenceValue::GetValueLabel()
   {
      UpdateReference();

      if (mValues.size())
      {
         if (!mValues[0].GetLinks().empty())
         {
            ValueNode* valueNode = mValues[0].GetLinks()[0]->AsValueNode();
            if (valueNode)
            {
               return mReference + "<br>" + valueNode->GetValueLabel();
            }
         }
      }

      return "";
   }

   //////////////////////////////////////////////////////////////////////////
   void ReferenceValue::OnConnectionChange()
   {
      UpdateLinkType();
      ValueNode::OnConnectionChange();
   }

   //////////////////////////////////////////////////////////////////////////
   int ReferenceValue::GetPropertyCount(const std::string& name)
   {
      if (name == "Value" && mValues.size())
      {
         return mValues[0].GetPropertyCount();
      }

      return Node::GetPropertyCount(name);
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProperty* ReferenceValue::GetProperty(const std::string& name, int index, ValueNode** outNode)
   {
      if (name == "Value" && mValues.size())
      {
         dtDAL::ActorProperty* prop = mValues[0].GetProperty(index, outNode);
         //if (prop && prop->GetName() == name) return prop;
         if (prop) return prop;
      }

      return Node::GetProperty(name, index, outNode);
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProperty* ReferenceValue::GetProperty(int index, ValueNode** outNode)
   {
      //UpdateReference();

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
   bool ReferenceValue::CanBeType(dtDAL::DataType& type)
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
   dtDAL::DataType& ReferenceValue::GetPropertyType()
   {
      // If we are linked to another value node, use that value's type.
      if (mValues.size() && mValues[0].GetLinks().size())
      {
         dtDAL::DataType& type = mValues[0].GetLinks()[0]->GetPropertyType();
         if (type != dtDAL::DataType::UNKNOWN)
         {
            return type;
         }
      }

      // If we are linked to a value link, use the type of that link.
      if (mLinks.size())
      {
         int count = (int)mLinks.size();
         for (int index = 0; index < count; index++)
         {
            dtDAL::DataType& type = mLinks[index]->GetPropertyType();
            if (type != dtDAL::DataType::UNKNOWN)
            {
               return type;
            }
         }
      }

      // If we have no connections yet, the type is undefined.
      return dtDAL::DataType::UNKNOWN;
   }

   //////////////////////////////////////////////////////////////////////////
   void ReferenceValue::UpdateLinkType()
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

   //////////////////////////////////////////////////////////////////////////
   void ReferenceValue::SetReference(const std::string& value)
   {
      if (value != mName)
      {
         mReference = value;
      }

      // Now attempt to find the actual value and link with it.
      UpdateReference();
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& ReferenceValue::GetReference()
   {
      return mReference;
   }

   //////////////////////////////////////////////////////////////////////////
   void ReferenceValue::UpdateReference()
   {
      if (!mReference.empty())
      {
         // Search for the referenced node anywhere in the current graph
         // or any of its parents.
         ValueNode* node = NULL;
         DirectorGraph* graph = GetGraph();
         while (graph)
         {
            node = graph->GetValueNode(mReference, false);
            if (node) break;

            graph = graph->mParent;
         }

         if (node)
         {
            // Disconnect all links that are no longer valid based on type.
            if (mLinks.size())
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

            mValues[0].Connect(node);
            SetColor(node->GetColor());
            return;
         }
      }

      // If we can't find the node, make sure we are not
      // connected to anything.
      mValues[0].Disconnect();
      SetColorRGB(Colors::GRAY);
   }
}
