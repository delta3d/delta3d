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
#include <dtDirectorNodes/inputnode.h>

#include <dtCore/actorproxy.h>
#include <dtCore/stringactorproperty.h>

#include <dtDirector/inputlink.h>
#include <dtDirector/outputlink.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   InputNode::InputNode()
       : EventNode()
   {
      mName = "In";
      AddAuthor("Jeff P. Houde");
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   InputNode::~InputNode()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void InputNode::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      EventNode::Init(nodeType, graph);

      // The input node is a special event node that contains an input.
      mInputs.clear();
      mInputs.push_back(InputLink(this, "In", "Activate this node."));

      // Create our default output.
      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Out", "Activated when this node is activated."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InputNode::BuildPropertyMap()
   {
      EventNode::BuildPropertyMap();

      AddProperty(new dtCore::StringActorProperty(
         "Name", "Name", 
         dtCore::StringActorProperty::SetFuncType(this, &InputNode::SetName),
         dtCore::StringActorProperty::GetFuncType(this, &InputNode::GetName),
         "The name of the input link."));
   }

   //////////////////////////////////////////////////////////////////////////
   bool InputNode::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      if (Test("Out", NULL))
      {
         return Node::Update(simDelta, delta, input, firstUpdate);
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void InputNode::SetName(const std::string& name)
   {
      if( !name.empty() )
      {
         mName = name;
         mInputs[0].SetName(name);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool InputNode::UsesActorFilters()
   {
      return false;
   }
}
