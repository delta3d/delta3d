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

#include <dtDirector/actionnode.h>
#include <dtDirector/colors.h>

#include <dtCore/actorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   ActionNode::ActionNode()
       : Node()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   ActionNode::~ActionNode()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ActionNode::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      Node::Init(nodeType, graph);

      // By default, Action nodes have one input and one output.
      mInputs.clear();
      mInputs.push_back(InputLink(this, "In", "Activate this node."));

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Out", "Activated when this node is activated."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   ActionNode* ActionNode::AsActionNode()
   {
      return this;
   }
}

//////////////////////////////////////////////////////////////////////////

