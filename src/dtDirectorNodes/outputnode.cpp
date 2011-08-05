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
#include <dtDirectorNodes/outputnode.h>
#include <dtDirector/colors.h>

#include <dtCore/stringactorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   OutputNode::OutputNode()
       : ActionNode()
   {
      mName = "Out";
      SetColorRGB(Colors::BEIGE);
      AddAuthor("Jeff P. Houde");
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   OutputNode::~OutputNode()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void OutputNode::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mInputs.clear();
      mInputs.push_back(InputLink(this, "In", "Activate this node."));

      // This node has an output node, but it is not visible inside the tier
      // because nothing should be connected to it within that tier.
      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Out", "Activated when this node is activated."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OutputNode::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      AddProperty(new dtCore::StringActorProperty(
         "Name", "Name", 
         dtCore::StringActorProperty::SetFuncType(this, &OutputNode::SetName),
         dtCore::StringActorProperty::GetFuncType(this, &OutputNode::GetName),
         "The name of the output link."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool OutputNode::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      mOutputs[0].Activate();
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void OutputNode::SetName(const std::string& name)
   {
      if( !name.empty() )
      {
         mName = name;
         mOutputs[0].SetName(name);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool OutputNode::OutputsExposed()
   {
      return false;
   }
}

//////////////////////////////////////////////////////////////////////////
