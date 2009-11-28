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

#include <dtDirector/node.h>
#include <dtDirector/nodemanager.h>

#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   Node::Node()
      : mDisabled(false)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   Node::Node(const Node& rhs)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   Node::~Node()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   Node& Node::operator=(const Node& rhs)
   {
      return *this;
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void Node::Init(const NodeType& nodeType)
   {
      SetType(nodeType);
      BuildPropertyMap();
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<Node> Node::Clone()
   {
      std::ostringstream error;

      // First tell the node manager to create a new node using this
      // nodes type.
      dtCore::RefPtr<Node> copy;

      try
      {
         copy = NodeManager::GetInstance().CreateNode(*mType).get();
      }
      catch(const dtUtil::Exception &e)
      {
         error << "Clone of Commander Node: " << mType->GetName() << " failed. Reason was: " << e.What();
         LOG_ERROR(error.str());
         return NULL;
      }

      copy->CopyPropertiesFrom(*this);

      return copy;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Node::BuildPropertyMap()
   {
      AddProperty(new dtDAL::StringActorProperty(
         "Comment", "Name",
         dtDAL::StringActorProperty::SetFuncType(this, &Node::SetComment),
         dtDAL::StringActorProperty::GetFuncType(this, &Node::GetComment),
         "Generic text field used to describe this node",
         "Base"));

      AddProperty(new dtDAL::BooleanActorProperty(
         "Disabled", "Disabled",
         dtDAL::BooleanActorProperty::SetFuncType(this, &Node::SetDisabled),
         dtDAL::BooleanActorProperty::GetFuncType(this, &Node::GetDisabled),
         "Disables the node from running in the script.",
         "Base"));

      AddProperty(new dtDAL::Vec2ActorProperty(
         "Position", "Position",
         dtDAL::Vec2ActorProperty::SetFuncType(this, &Node::SetPosition),
         dtDAL::Vec2ActorProperty::GetFuncType(this, &Node::GetPosition),
         "The UI Position of the Node.", "UI"));
   }

   //////////////////////////////////////////////////////////////////////////
   void Node::Update(float simDelta, float delta)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   const NodeType& Node::GetType()
   {
      return *mType;
   }

   //////////////////////////////////////////////////////////////////////////
   void Node::SetType(const NodeType& type)
   {
      mType = &type;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& Node::GetComment() const
   {
      return mComment;
   }

   //////////////////////////////////////////////////////////////////////////
   void Node::SetComment(const std::string& comment)
   {
      mComment = comment;
   }

   //////////////////////////////////////////////////////////////////////////
   bool Node::GetDisabled() const
   {
      return mDisabled;
   }

   //////////////////////////////////////////////////////////////////////////
   void Node::SetDisabled(bool disabled)
   {
      mDisabled = disabled;
   }

   //////////////////////////////////////////////////////////////////////////
   int Node::GetPropertyCount(const std::string& name)
   {
      // First iterate through all value links to see if this property
      // is redirected.
      for (int valueIndex = 0; valueIndex < (int)mValues.size(); valueIndex++)
      {
         dtDAL::ActorProperty* prop = mValues[valueIndex].GetDefaultProperty();
         if (prop && prop->GetName() == name)
         {
            return mValues[valueIndex].GetPropertyCount();
         }
      }

      // Did not find any overrides, so return the default.
      if (dtDAL::PropertyContainer::GetProperty(name))
      {
         return 1;
      }

      return 0;
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProperty* Node::GetProperty(const std::string& name, int index)
   {
      // First iterate through all value links to see if this property
      // is redirected.
      for (int valueIndex = 0; valueIndex < (int)mValues.size(); valueIndex++)
      {
         dtDAL::ActorProperty* prop = mValues[valueIndex].GetDefaultProperty();
         if (prop && prop->GetName() == name)
         {
            return mValues[valueIndex].GetProperty(index);
         }
      }

      // Did not find any overrides, so return the default.
      if (index == 0)
      {
         return dtDAL::PropertyContainer::GetProperty(name);
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   bool Node::GetBoolean(const std::string& name, int index)
   {
      return GetPropertyValue<bool>(name, index);
   }

   //////////////////////////////////////////////////////////////////////////
   int Node::GetInt(const std::string& name, int index)
   {
      return GetPropertyValue<int>(name, index);
   }

   //////////////////////////////////////////////////////////////////////////
   float Node::GetFloat(const std::string& name, int index)
   {
      return GetPropertyValue<float>(name, index);
   }

   //////////////////////////////////////////////////////////////////////////
   double Node::GetDouble(const std::string& name, int index)
   {
      return GetPropertyValue<double>(name, index);
   }

   //////////////////////////////////////////////////////////////////////////
   std::string Node::GetString(const std::string& name, int index)
   {
      dtDAL::ActorProperty* prop = GetProperty(name, index);
      if (prop) return prop->ToString();
      return "";
   }

   //////////////////////////////////////////////////////////////////////////
   void Node::SetBoolean(bool value, const std::string& name, int index)
   {
      SetPropertyValue<bool>(value, name, index);
   }

   //////////////////////////////////////////////////////////////////////////
   void Node::SetInt(int value, const std::string& name, int index)
   {
      SetPropertyValue<int>(value, name, index);
   }

   //////////////////////////////////////////////////////////////////////////
   void Node::SetFloat(float value, const std::string& name, int index)
   {
      SetPropertyValue<float>(value, name, index);
   }

   //////////////////////////////////////////////////////////////////////////
   void Node::SetDouble(double value, const std::string& name, int index)
   {
      SetPropertyValue<double>(value, name, index);
   }

   //////////////////////////////////////////////////////////////////////////
   void Node::SetString(const std::string& value, const std::string& name, int index)
   {
      if (index == -1)
      {
         int count = GetPropertyCount(name);
         for (index = 0; index < count; index++)
         {
            dtDAL::ActorProperty* prop = GetProperty(name, index);
            if (prop)
            {
               prop->FromString(value);
            }
         }
      }
      else
      {
         dtDAL::ActorProperty* prop = GetProperty(name, index);
         if (prop)
         {
            prop->FromString(value);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   InputLink* Node::GetInputLink(const std::string& name)
   {
      int count = (int)mInputs.size();
      for (int index = 0; index < count; index++)
      {
         if (mInputs[index].GetName() == name)
         {
            return &mInputs[index];
         }
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   OutputLink* Node::GetOutputLink(const std::string& name)
   {
      int count = (int)mOutputs.size();
      for (int index = 0; index < count; index++)
      {
         if (mOutputs[index].GetName() == name)
         {
            return &mOutputs[index];
         }
      }

      return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   ValueLink* Node::GetValueLink(const std::string& name)
   {
      int count = (int)mValues.size();
      for (int index = 0; index < count; index++)
      {
         if (mValues[index].GetName() == name)
         {
            return &mValues[index];
         }
      }

      return NULL;
   }
}

//////////////////////////////////////////////////////////////////////////
