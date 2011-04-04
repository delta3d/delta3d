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

#include <dtDirector/nodetype.h>

namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   NodeType::NodeType(NodeType::NodeTypeEnum type,
                     const std::string& name,
                     const std::string& category,
                     const std::string& folder,
                     const std::string& desc,
                     const NodeType* parentType,
                     int red, int green, int blue)
      : ObjectType(name, category, desc, parentType)
      , mFolder(folder)
      , mType(type)
      , mColorRed(red)
      , mColorGreen(green)
      , mColorBlue(blue)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   const NodeType* NodeType::GetParentNodeType() const
   {
      return dynamic_cast<const NodeType*>(GetParentType());
   }

   ////////////////////////////////////////////////////////////////////////////////
   NodeType::NodeTypeEnum NodeType::GetNodeType() const
   {
      return mType;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& NodeType::GetFolder() const
   {
      return mFolder;
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeType::SetFolder(const std::string& folder)
   {
      mFolder = folder;
   }

   ////////////////////////////////////////////////////////////////////////////////
   int NodeType::GetColorRed() const
   {
      return mColorRed;
   }

   ////////////////////////////////////////////////////////////////////////////////
   int NodeType::GetColorGreen() const
   {
      return mColorGreen;
   }

   ////////////////////////////////////////////////////////////////////////////////
   int NodeType::GetColorBlue() const
   {
      return mColorBlue;
   }

   //////////////////////////////////////////////////////////////////////////
   NodeType::~NodeType()
   {
   }
}
