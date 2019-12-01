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

#include <dtDirector/groupnode.h>
#include <dtDirector/colors.h>

#include <dtCore/actorproperty.h>
#include <dtCore/vectoractorproperties.h>
#include <dtCore/colorrgbaactorproperty.h>

namespace dtDirector
{
   ///////////////////////////////////////////////////////////////////////////////////////
   GroupNode::GroupNode()
       : ActionNode()
       , mSize(100.0f, 100.0f)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   GroupNode::~GroupNode()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void GroupNode::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      // By default, Action nodes have one input and one output.
      mInputs.clear();
      mOutputs.clear();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GroupNode::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      // Remove unnecessary properties.
      RemoveProperty("Enabled");
      RemoveProperty("LogNode");
      RemoveProperty("Authors");

      mValues.clear();

      AddProperty(new dtCore::Vec2ActorProperty(
         "Size", "Size",
         dtCore::Vec2ActorProperty::SetFuncType(this, &GroupNode::SetSize),
         dtCore::Vec2ActorProperty::GetFuncType(this, &GroupNode::GetSize),
         "The UI size of the Node.", "Info"));

      AddProperty(new dtCore::ColorRgbaActorProperty(
         "Color", "Color",
         dtCore::ColorRgbaActorProperty::SetFuncType(this, &Node::SetColor),
         dtCore::ColorRgbaActorProperty::GetFuncType(this, &Node::GetColor),
         "The UI color of the Node."));
   }
}

//////////////////////////////////////////////////////////////////////////

