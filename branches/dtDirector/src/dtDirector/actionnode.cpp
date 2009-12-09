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

#include <dtDirector/actionnode.h>

#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   ActionNode::ActionNode()
       : Node()
       , mActive(false)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   ActionNode::~ActionNode()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void ActionNode::Init(const NodeType& nodeType, DirectorGraphData* graph)
   {
      Node::Init(nodeType, graph);

      // By default, Action nodes have one input and one output.
      mInputs.clear();
      mInputs.push_back(InputLink(this, "In"));

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Out"));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActionNode::BuildPropertyMap()
   {
      Node::BuildPropertyMap();
   }

   //////////////////////////////////////////////////////////////////////////
   void ActionNode::Update(float simDelta, float delta)
   {
      Node::Update(simDelta, delta);

      // Skip the udpate if this node is disabled.
      if (GetDisabled()) return;

      // If this node is active, perform an active update.
      if (mActive)
      {
         mActive = ActiveUpdate(simDelta, delta);
      }

      // Test all inputs for activation.
      for (int inputIndex = 0; inputIndex < (int)mInputs.size(); inputIndex++)
      {
         if (mInputs[inputIndex].Test())
         {
            OnInputActivated(inputIndex);
            mActive = true;
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool ActionNode::ActiveUpdate(float simDelta, float delta)
   {
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void ActionNode::OnInputActivated(int inputIndex)
   {
      if (mOutputs.size() > 0)
      {
         mOutputs[0].Activate();
      }
   }
}
