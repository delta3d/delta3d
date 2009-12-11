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

#include <dtDirectorNodes/inputnode.h>

#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/actorproxy.h>

#include <dtDirector/inputlink.h>
#include <dtDirector/outputlink.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   InputNode::InputNode()
       : EventNode()
   {
      mName = "In";
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   InputNode::~InputNode()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void InputNode::Init(const NodeType& nodeType, DirectorGraphData* graph)
   {
      EventNode::Init(nodeType, graph);

      // The input node is a special event node that contains an input.
      mInputs.clear();
      mInputs.push_back(InputLink(this, "In"));

      // Create our default output.
      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Out"));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void InputNode::BuildPropertyMap()
   {
      EventNode::BuildPropertyMap();

      AddProperty(new dtDAL::StringActorProperty(
         "Name", "Name", 
         dtDAL::StringActorProperty::SetFuncType(this, &InputNode::SetName),
         dtDAL::StringActorProperty::GetFuncType(this, &InputNode::GetName),
         "The name of the input link."));
   }

   //////////////////////////////////////////////////////////////////////////
   void InputNode::Update(float simDelta, float delta)
   {
      if (!GetEnabled()) return;

      EventNode::Update(simDelta, delta);

      // Check to see if our input has been activated.
      if (mInputs.size() && mInputs[0].Test())
      {
         if (mOutputs.size()) mOutputs[0].Activate();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void InputNode::SetName(const std::string& name)
   {
      mName = name;
      mInputs[0].SetName(name);
   }

   //////////////////////////////////////////////////////////////////////////
   bool InputNode::UsesInstigator()
   {
      return false;
   }
}
