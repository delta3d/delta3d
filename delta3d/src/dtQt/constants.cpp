/* -*-c++-*-
* Copyright (C) 2015, Caper Holdings LLC
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
* Chris Rodgers
*/

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtQt/constants.h>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   const QString Constants::ICON_NODE("");
   const QString Constants::ICON_BONE(":dtQt/icons/nodes/bone.png");
   const QString Constants::ICON_DOF(":dtQt/icons/nodes/dof.png");
   const QString Constants::ICON_GEODE(":dtQt/icons/nodes/geode.png");
   const QString Constants::ICON_GEOMETRY(":dtQt/icons/nodes/geometry.png");
   const QString Constants::ICON_GROUP(":dtQt/icons/nodes/group.png");
   const QString Constants::ICON_LOD(":dtQt/icons/nodes/lod.png");
   const QString Constants::ICON_MATRIX(":dtQt/icons/nodes/matrix.png");
   const QString Constants::ICON_OCCLUDER(":dtQt/icons/nodes/occluder.png");
   const QString Constants::ICON_OCCLUSION_QUERY(":dtQt/icons/nodes/occlusionquery.png");
   const QString Constants::ICON_SKELETON(":dtQt/icons/nodes/skeleton.png");
   const QString Constants::ICON_STATESET(":dtQt/icons/nodes/stateset.png");

   const QString* Constants::GetIconPathByClassName(const std::string& className)
   {
      using namespace dtUtil;

      const NodeType* nodeType = NodeType::GetNodeTypeByClassName(className);

      const QString* icon = &Constants::ICON_NODE;

      if (nodeType == &NodeType::MATRIX)
      {
         icon = &Constants::ICON_MATRIX;
      }
      else if (nodeType == &NodeType::GROUP)
      {
         icon = &Constants::ICON_GROUP;
      }
      else if (nodeType == &NodeType::GEODE)
      {
         icon = &Constants::ICON_GEODE;
      }
      else if (nodeType == &NodeType::GEOMETRY)
      {
         icon = &Constants::ICON_GEOMETRY;
      }
      else if (nodeType == &NodeType::DOF)
      {
         icon = &Constants::ICON_DOF;
      }
      else if (nodeType == &NodeType::LOD)
      {
         icon = &Constants::ICON_LOD;
      }
      else if (nodeType == &NodeType::BONE)
      {
         icon = &Constants::ICON_BONE;
      }
      else if (nodeType == &NodeType::SKELETON)
      {
         icon = &Constants::ICON_SKELETON;
      }
      else if (nodeType == &NodeType::OCCLUDER)
      {
         icon = &Constants::ICON_OCCLUDER;
      }
      else if (nodeType == &NodeType::OCCLUSION_QUERY)
      {
         icon = &Constants::ICON_OCCLUSION_QUERY;
      }

      return icon;
   }

}