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
      if (dtDAL::PropertyContainer::GetProperty(name))
      {
         return 1;
      }

      return 0;
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProperty* Node::GetProperty(const std::string& name, int index)
   {
      if (index == 0)
      {
         return dtDAL::PropertyContainer::GetProperty(name);
      }
      return NULL;
   }
}

//////////////////////////////////////////////////////////////////////////
